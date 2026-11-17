
#include "csm.h"
#include "CSM/csmdetector.h"
#include "CSM/csmfrequency.h"
#include "CSM/csmsignalmodel.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/hierarchy.h"
#include "core/Hierarchy/EntityProfiles/radio.h"
#include "core/Simulation/simulation.h"
#include <cmath>
#include <string>
#include <unordered_map>

namespace {

const double kRad2Deg = 180.0 / M_PI;

// Name of a configured modulation scheme (configuration, not a classification).
std::string modulationName(radio::ModulationScheme m)
{
    using M = radio::ModulationScheme;
    switch (m) {
    case M::AM:         return "AM";
    case M::FM:         return "FM";
    case M::BPSK:       return "BPSK";
    case M::QPSK:       return "QPSK";
    case M::PSK8:       return "PSK8";
    case M::QAM16:      return "QAM16";
    case M::QAM64:      return "QAM64";
    case M::FSK2:       return "FSK2";
    case M::FSK4:       return "FSK4";
    case M::GMSK:       return "GMSK";
    case M::OFDM_BPSK:  return "OFDM-BPSK";
    case M::OFDM_QPSK:  return "OFDM-QPSK";
    case M::OFDM_QAM16: return "OFDM-QAM16";
    case M::OFDM_QAM64: return "OFDM-QAM64";
    }
    return std::string();
}

radio::Position toModelPosition(Transform& t)
{
    radio::Position p;
    p.x = t.translation().z() * 1000.0;
    p.y = t.translation().x() * 1000.0;
    p.altitude = t.translation().y() * 1000.0;
    return p;
}

std::string emitterKey(const std::string& platformId, const std::string& radioId)
{
    return platformId + '\x1f' + radioId;
}

struct Candidate {
    radio::Position pos;
    radio::RadioConfig cfg;
};

} // namespace

CSM::CSM(Hierarchy* h) : Sensor(h) {
    subType = SubType::CSM;
    azimuth = 360;
    frequency = 0.15f;
    bandwidth = 100.0f;
    noiseFigure = 6.0f;
}
void CSM::dropPlatformTargets()
{
    for (int i = ewtargets.size() - 1; i >= 0; --i) {
        if (ewtargets.at(i).entity) ewtargets.removeAt(i);
    }
    ewdetects.clear();
}

void CSM::clearScanState()
{
    lastScan_ = csm::ScanResult();
    spectrumCache_.clear();
    held_.clear();
    lastMonitor_ = csm::monitor::Window();
    dropPlatformTargets();
}

void CSM::setOperatingMode(csm::OperatingMode m)
{
    if (m == opMode_) return;
    opMode_ = m;
    // A switch must not leave receive results (or a spectrum) behind.
    clearScanState();
}

bool CSM::interferenceSource(csm::InterfererInput& out) const
{
    return csm::signal::makeInterferer(isTransmitting(), txFrequencyGhz, txBandwidthMhz,
                                       txPowerDbm, antennaGain, out);
}


bool CSM::jamBandHz(double& loHz, double& hiHz) const
{
    csm::InterfererInput in;
    if (!interferenceSource(in)) return false;
    loHz = in.frequencyHz - in.bandwidthHz * 0.5;
    hiHz = in.frequencyHz + in.bandwidthHz * 0.5;
    return true;
}



void CSM::scan()
{
    if (!Active || !parentEntity || !root) {
        clearScanState();
        return;
    }
    if (opMode_ == csm::OperatingMode::Transmit) {
        clearScanState();
        lastMonitor_.reason = "CSM is in TRANSMIT mode (receiver suspended)";
        return;
    }
    auto obsIt = root->Platforms.find(parentEntity->ID);
    if (obsIt == root->Platforms.end() || !obsIt->second) {
        clearScanState();
        return;
    }
    Platform* observer = obsIt->second;
    Transform& source = observer->transform;
    const radio::Position observerPos = toModelPosition(source);

    // --- Snapshot every radio as a pointer-free EmitterInput ---------------
    std::vector<csm::EmitterInput> emitters;
    std::unordered_map<std::string, Candidate> candidates;
    emitters.reserve(root->Radios.size());

    for (auto const& kv : root->Radios) {
        Radio* r = kv.second;
        if (!r || !r->parentEntity) continue;
        // Hierarchy::updateJammingDisplaySuppression() marks radios inside an
        // active CSM jammer's range/band. Drop them from observations so the
        // CSM table matches the Radio display (jammed => not listed).
        if (r->displaySuppressedByJam) continue;
        auto pit = root->Platforms.find(r->parentEntity->ID);
        if (pit == root->Platforms.end() || !pit->second) continue;
        Platform* platform = pit->second;

        csm::EmitterInput e;
        e.platformId = platform->ID;
        e.platformName = platform->Name;
        e.radioId = r->ID;
        e.radioName = r->Name;
        e.platformActive = platform->Active;
        e.radioActive = r->Active;
        const QVector3D localPos =
            source.inverseTransformPoint(platform->transform.matrix.translation());
        e.distanceM = distanceBetween(source.getLatitude(), source.getLongitude(),
                                      platform->transform.getLatitude(),
                                      platform->transform.getLongitude());
        e.bearingDeg = std::atan2(localPos.x(), localPos.z()) * kRad2Deg;
        e.inCoverage = detectCheck(localPos, static_cast<float>(e.distanceM / 1000.0));
        radio::RadioConfig cfg;
        if (r->lib_radio) {
            cfg = r->lib_radio->getConfiguration();
            e.frequencyHz = cfg.frequency_hz;
            e.bandwidthHz = cfg.bandwidth_hz;
            e.minFreqHz = cfg.min_freq_hz;
            e.maxFreqHz = cfg.max_freq_hz;
            e.canTransmit = (cfg.mode != radio::RadioMode::RECEIVER_ONLY) &&
                            r->lib_radio->isPoweredOn();
            e.modulation = modulationName(cfg.modulation_scheme);
            e.txPowerDbm = cfg.tx_power_dbm;
            e.txPowerDegradationDb = cfg.power_degradation_db;
            e.txDutyCycle = cfg.tx_duty_cycle;
            e.txAntennaGainDbi = cfg.antenna.gain_dbi;
        }

        candidates[emitterKey(e.platformId, e.radioId)] = { toModelPosition(platform->transform), cfg };
        emitters.push_back(std::move(e));
    }
    const double nowS = Simulation::simulationTime;
    csm::monitor::Settings ms;
    ms.autoRange = autoMonitor;
    ms.mode = scanMode ? csm::monitor::Mode::Scan : csm::monitor::Mode::Monitor;
    ms.centreHz = csm::freq::ghzToHz(frequency);
    ms.spanHz = (std::isfinite(bandwidth) && bandwidth > 0.0f)
                    ? static_cast<double>(bandwidth) * csm::freq::kHzPerMHz
                    : 0.0;
    ms.scanStepHz = csm::freq::mhzToHz(scanStepMhz);
    ms.scanDwellS = scanDwellS;
    lastMonitor_ = csm::monitor::resolve(ms, nowS);

    radio::PropagationModel* model = Radio::model;
    csm::MonitorConfig mon;
    mon.centreHz = lastMonitor_.valid ? lastMonitor_.centreHz : csm::kNaN;
    mon.bandwidthHz = lastMonitor_.valid ? lastMonitor_.spanHz : 0.0;
    mon.rxGainDbi = antennaGain;
    mon.noiseFigureDb = noiseFigure;
    mon.temperatureC = model ? model->getConfig().temperature_c : csm::kNaN;
    mon.sensitivityDbm = sensitivityDbm;   // NaN = unset
    mon.minSnrDb = minSnrDb;

    // Path loss from the existing propagation moel
    const radio::RadioConfig rxCfg;
    auto pathLoss = [&](const csm::EmitterInput& e) -> double {
        if (!model) return csm::kNaN;
        auto it = candidates.find(emitterKey(e.platformId, e.radioId));
        if (it == candidates.end()) return csm::kNaN;
        return model->pathLossDb(it->second.pos, observerPos, it->second.cfg, rxCfg);
    };
    std::vector<csm::InterfererInput> interferers;
    for (auto const& kv : root->Sensors) {
        const CSM* j = (kv.second && kv.second != this && kv.second->subType == SubType::CSM)
        ? dynamic_cast<const CSM*>(kv.second) : nullptr;
        if (!j || !j->parentEntity || j->parentEntity->ID == parentEntity->ID) continue;
        csm::InterfererInput in;
        if (!j->interferenceSource(in)) continue;
        auto jp = root->Platforms.find(j->parentEntity->ID);
        if (jp == root->Platforms.end() || !jp->second || !jp->second->Active) continue;
        Platform* jam = jp->second;
        const QVector3D localPos =
            source.inverseTransformPoint(jam->transform.matrix.translation());
        const double distM = distanceBetween(source.getLatitude(), source.getLongitude(),
                                             jam->transform.getLatitude(),
                                             jam->transform.getLongitude());
        in.inCoverage = detectCheck(localPos, static_cast<float>(distM / 1000.0));
        if (!in.inCoverage) continue;
        if (model) {
            radio::RadioConfig txCfg;       // only what the propagation model reads
            txCfg.frequency_hz = in.frequencyHz;
            txCfg.bandwidth_hz = in.bandwidthHz;
            txCfg.tx_power_dbm = in.txPowerDbm;
            txCfg.antenna.gain_dbi = in.txGainDbi;
            in.pathLossDb = model->pathLossDb(toModelPosition(jam->transform), observerPos,
                                              txCfg, rxCfg);
        }
        interferers.push_back(in);
    }

    csm::ScanResult current = csm::Detector::evaluate(emitters, parentEntity->ID, mon, pathLoss,
                                                      nowS, &interferers);
    csm::spectrum::Input specIn;
    specIn.monitorCentreHz = mon.centreHz;
    specIn.monitorSpanHz = mon.bandwidthHz;
    specIn.noiseFigureDb = mon.noiseFigureDb;
    specIn.temperatureC = mon.temperatureC;
    for (const csm::Observation& o : current.observations) {
        csm::spectrum::EmitterSignal es;
        es.label = (o.platformName.empty() ? o.platformId : o.platformName) + "/" +
                   (o.radioName.empty() ? o.radioId : o.radioName);
        if (o.frequencyHz.valid()) es.frequencyHz = o.frequencyHz.value;
        if (o.bandwidthHz.valid()) es.bandwidthHz = o.bandwidthHz.value;
        if (o.levelDbm.valid())    es.powerDbm = o.levelDbm.value;
        if (o.modulationSource == csm::Provenance::Configured) es.modulation = o.modulation;
        specIn.emitters.push_back(std::move(es));
    }
    for (const csm::InterfererInput& in : interferers) {
        const double p = csm::signal::receivedPowerDbm(in.txPowerDbm, 0.0, 1.0, in.txGainDbi,
                                                       mon.rxGainDbi, in.pathLossDb);
        if (!std::isfinite(p)) continue;
        csm::spectrum::InterfererSignal is;
        is.centreHz = in.frequencyHz;
        is.bandwidthHz = in.bandwidthHz;
        is.powerDbm = p;
        specIn.interferers.push_back(is);
    }
    spectrumCache_.update(specIn);

    // --- Result for the table / legacy targets ------------------------------
    if (scanMode && lastMonitor_.valid) {
        // Hold each emitter's last detection for one sweep + one dwell, then drop it.
        if (nowS < lastScanTimeS_) held_.clear();            // simulation restarted
        for (const csm::Observation& o : current.observations)
            held_[emitterKey(o.platformId, o.radioId)] = o;
        const double holdS = lastMonitor_.sweepPeriodS + static_cast<double>(scanDwellS);
        for (auto it = held_.begin(); it != held_.end();) {
            if (nowS - it->second.observedAtS > holdS) it = held_.erase(it);
            else ++it;
        }
        lastScan_ = csm::ScanResult();
        lastScan_.stats = current.stats;
        lastScan_.interference = current.interference;
        for (const auto& kv : held_) {
            const csm::Observation& o = kv.second;
            lastScan_.observations.push_back(o);
            if (lastScan_.platforms.empty() || lastScan_.platforms.back().platformId != o.platformId) {
                csm::PlatformDetection p;
                p.platformId = o.platformId;
                p.distanceM = o.distanceM.value;
                p.bearingDeg = o.bearingDeg.value;
                lastScan_.platforms.push_back(p);
            }
            ++lastScan_.platforms.back().radioCount;
        }
        lastScan_.stats.detectedRadios = static_cast<int>(lastScan_.observations.size());
        lastScan_.stats.detectedPlatforms = static_cast<int>(lastScan_.platforms.size());
    } else {
        held_.clear();
        lastScan_ = std::move(current);
    }
    lastScanTimeS_ = nowS;

    // --- Platform-level aggregate for legacy ewtargets/ewdetects ------------
    dropPlatformTargets();

    for (const csm::PlatformDetection& pd : lastScan_.platforms) {
        auto pit = root->Platforms.find(pd.platformId);
        if (pit == root->Platforms.end() || !pit->second) continue;

        Target target{};
        target.entity = pit->second;
        target.angle = static_cast<float>(pd.bearingDeg);
        target.radius = static_cast<float>(pd.distanceM / 1000.0);   // km, as before
        ewdetects.insert(pit->second);
        ewtargets.append(target);
    }
}

QJsonObject CSM::toJson() const {
    QJsonObject obj;
    obj["active"] = Active;
    obj["name"] = QString::fromStdString(Name);
    obj["SensorType"] = "CSM";
    obj["id"] = QString::fromStdString(ID);

    // --- Default (shared geometry / antenna) --------------------------------
    QJsonObject d;
    d["type"] = "Section";
    d["range"] = toParm(range, "km", 0, 500,
                        "Coverage range for receive geometry; also used as jam range when Jam Range is 0");
    d["azimuth"] = toParm(azimuth, "deg", 0, 360);
    d["antennaGain"] = toParm(antennaGain, "dbi", -50, 100,
                              "Shared antenna gain (receive link budget and TRANSMIT interference)");
    obj["default"] = d;

    // --- Receiver (electronic-support monitor) ------------------------------
    QJsonObject r;
    r["type"] = "Section";
    r["frequency"] = toParm(frequency, "Ghz", 0.1, 100,
                            "Manual monitoring centre (when Monitor Auto = 0). Not a radio TX frequency");
    r["bandwidth"] = toParm(bandwidth, "Mhz", 0, 50000,
                            "Manual monitoring span (when Monitor Auto = 0). 0 = tuned point");
    r["noiseFigure"] = toParm(noiseFigure, "db", 0, 50);
    r["monitorAuto"] = toParm(autoMonitor ? 1.0f : 0.0f, "0/1", 0, 1,
                              "1 = automatic 100 MHz-6 GHz; 0 = use Frequency/Bandwidth above");
    r["scanMode"] = toParm(scanMode ? 1.0f : 0.0f, "0/1", 0, 1,
                           "0 = MONITOR whole band; 1 = SCAN stepped window");
    r["scanStep"] = toParm(scanStepMhz, "Mhz", 1, 5000, "SCAN window width");
    r["scanDwell"] = toParm(scanDwellS, "s", 0.05f, 60, "SCAN dwell per step");
    r["minSnr"] = toParm(minSnrDb, "db", -20, 60,
                         "Minimum SNR/SINR over signal bandwidth to keep a detection");
    r["sensitivityEnabled"] = toParm(std::isfinite(sensitivityDbm) ? 1.0f : 0.0f, "0/1", 0, 1,
                                     "1 = apply Sensitivity threshold below");
    r["sensitivity"] = toParm(std::isfinite(sensitivityDbm) ? sensitivityDbm : -100.0f, "dBm", -200, 0,
                              "Minimum received level for a detection");
    obj["receiver"] = r;

    // --- Jammer (TRANSMIT) — one Enable checkbox like ESM --------------------
    QJsonObject j;
    j["type"] = "Section";
    // Plain bool => Inspector checkbox (same pattern as ESM jammer Enabled).
    // ON  => OperatingMode::Transmit + txEnabled
    // OFF => OperatingMode::Receive  + !txEnabled
    const bool jamOn = (opMode_ == csm::OperatingMode::Transmit) && txEnabled;
    j["enabled"] = jamOn;
    j["frequency"] = toParm(txFrequencyGhz, "Ghz", 0.001f, 100, "Jammer centre frequency");
    j["bandwidth"] = toParm(txBandwidthMhz, "Mhz", 0.001f, 50000,
                            "Jammer occupied bandwidth (power spread in this band)");
    j["power"] = toParm(txPowerDbm, "dBm", -50, 90,
                        "Jammer transmit power (antenna gain from Default)");
    j["jamRange"] = toParm(jamRangeKm, "km", 0, 500,
                           "Distance for radio suppression (display + observation). 0 = use Default Range");
    j["jamFrequencyGate"] = jamFrequencyGate;  // checkbox: only overlap band vs all in range
    obj["jammer"] = j;

    QJsonObject add = AdditionalParameters;
    add["type"] = "Section";
    obj["AdditionalParameters"] = add;
    return obj;
}


void CSM::fromJson(const QJsonObject& obj) {
    if (obj.contains("id"))
        ID = obj["id"].toString().toStdString();
    if (obj.contains("active"))
        Active = obj["active"].toBool();

    auto readFiniteObj = [](const QJsonObject& sec, const char* key, float& dst) {
        if (!sec.contains(key)) return;
        const float v = valueFromParm(sec[key].toObject());
        if (std::isfinite(v)) dst = v;
    };

    if (obj.contains("default") && obj["default"].isObject()) {
        QJsonObject d = obj["default"].toObject();
        if (d.contains("range"))
            range = valueFromParm(d["range"].toObject());
        if (d.contains("azimuth"))
            azimuth = valueFromParm(d["azimuth"].toObject());
        if (d.contains("antennaGain"))
            antennaGain = valueFromParm(d["antennaGain"].toObject());

        // Legacy flat default still accepted (pre section split)
        if (d.contains("frequency"))
            frequency = valueFromParm(d["frequency"].toObject());
        if (d.contains("bandwidth"))
            bandwidth = valueFromParm(d["bandwidth"].toObject());
        if (d.contains("noiseFigure"))
            noiseFigure = valueFromParm(d["noiseFigure"].toObject());
        if (d.contains("monitorAuto"))
            autoMonitor = valueFromParm(d["monitorAuto"].toObject()) >= 0.5f;
        else if (!obj.contains("receiver"))
            autoMonitor = csm::monitor::isLegacyUnconfigured(frequency, bandwidth);
        if (d.contains("scanMode"))
            scanMode = valueFromParm(d["scanMode"].toObject()) >= 0.5f;
        if (d.contains("scanStep"))
            scanStepMhz = valueFromParm(d["scanStep"].toObject());
        if (d.contains("scanDwell"))
            scanDwellS = valueFromParm(d["scanDwell"].toObject());
        if (d.contains("minSnr"))
            minSnrDb = valueFromParm(d["minSnr"].toObject());
        if (d.contains("sensitivityEnabled")) {
            const bool en = valueFromParm(d["sensitivityEnabled"].toObject()) >= 0.5f;
            sensitivityDbm = (en && d.contains("sensitivity"))
                                 ? valueFromParm(d["sensitivity"].toObject())
                                 : std::numeric_limits<float>::quiet_NaN();
        }
        if (d.contains("operatingMode"))
            setOperatingMode(valueFromParm(d["operatingMode"].toObject()) >= 0.5f
                                 ? csm::OperatingMode::Transmit : csm::OperatingMode::Receive);
        if (d.contains("txEnabled"))
            txEnabled = valueFromParm(d["txEnabled"].toObject()) >= 0.5f;
        readFiniteObj(d, "txFrequency", txFrequencyGhz);
        readFiniteObj(d, "txBandwidth", txBandwidthMhz);
        readFiniteObj(d, "txPower", txPowerDbm);
        readFiniteObj(d, "jamRange", jamRangeKm);
        if (d.contains("jamFrequencyGate"))
            jamFrequencyGate = valueFromParm(d["jamFrequencyGate"].toObject()) >= 0.5f;
    }

    if (obj.contains("receiver") && obj["receiver"].isObject()) {
        QJsonObject r = obj["receiver"].toObject();
        if (r.contains("frequency"))
            frequency = valueFromParm(r["frequency"].toObject());
        if (r.contains("bandwidth"))
            bandwidth = valueFromParm(r["bandwidth"].toObject());
        if (r.contains("noiseFigure"))
            noiseFigure = valueFromParm(r["noiseFigure"].toObject());
        if (r.contains("monitorAuto"))
            autoMonitor = valueFromParm(r["monitorAuto"].toObject()) >= 0.5f;
        if (r.contains("scanMode"))
            scanMode = valueFromParm(r["scanMode"].toObject()) >= 0.5f;
        if (r.contains("scanStep"))
            scanStepMhz = valueFromParm(r["scanStep"].toObject());
        if (r.contains("scanDwell"))
            scanDwellS = valueFromParm(r["scanDwell"].toObject());
        if (r.contains("minSnr"))
            minSnrDb = valueFromParm(r["minSnr"].toObject());
        if (r.contains("sensitivityEnabled")) {
            const bool en = valueFromParm(r["sensitivityEnabled"].toObject()) >= 0.5f;
            sensitivityDbm = (en && r.contains("sensitivity"))
                                 ? valueFromParm(r["sensitivity"].toObject())
                                 : std::numeric_limits<float>::quiet_NaN();
        }
    }

    if (obj.contains("jammer") && obj["jammer"].isObject()) {
        QJsonObject j = obj["jammer"].toObject();
        // Single Enable checkbox (bool or legacy toParm 0/1)
        if (j.contains("enabled")) {
            bool on = false;
            if (j["enabled"].isBool())
                on = j["enabled"].toBool();
            else if (j["enabled"].isObject())
                on = valueFromParm(j["enabled"].toObject()) >= 0.5f;
            else
                on = j["enabled"].toDouble() >= 0.5;
            txEnabled = on;
            setOperatingMode(on ? csm::OperatingMode::Transmit : csm::OperatingMode::Receive);
        }
        // Legacy keys (old scenarios): still honour if present
        if (j.contains("operatingMode") && j["operatingMode"].isObject())
            setOperatingMode(valueFromParm(j["operatingMode"].toObject()) >= 0.5f
                                 ? csm::OperatingMode::Transmit : csm::OperatingMode::Receive);
        if (j.contains("txEnabled") && j["txEnabled"].isObject())
            txEnabled = valueFromParm(j["txEnabled"].toObject()) >= 0.5f;
        if (j.contains("frequency"))
            readFiniteObj(j, "frequency", txFrequencyGhz);
        if (j.contains("txFrequency"))
            readFiniteObj(j, "txFrequency", txFrequencyGhz);
        if (j.contains("bandwidth"))
            readFiniteObj(j, "bandwidth", txBandwidthMhz);
        if (j.contains("txBandwidth"))
            readFiniteObj(j, "txBandwidth", txBandwidthMhz);
        if (j.contains("power"))
            readFiniteObj(j, "power", txPowerDbm);
        if (j.contains("txPower"))
            readFiniteObj(j, "txPower", txPowerDbm);
        readFiniteObj(j, "jamRange", jamRangeKm);
        if (j.contains("jamFrequencyGate")) {
            if (j["jamFrequencyGate"].isBool())
                jamFrequencyGate = j["jamFrequencyGate"].toBool();
            else if (j["jamFrequencyGate"].isObject())
                jamFrequencyGate = valueFromParm(j["jamFrequencyGate"].toObject()) >= 0.5f;
        }
    }

    if (jamRangeKm < 0.0f) jamRangeKm = 0.0f;

    if (obj.contains("AdditionalParameters"))
        AdditionalParameters = obj["AdditionalParameters"].toObject();
}


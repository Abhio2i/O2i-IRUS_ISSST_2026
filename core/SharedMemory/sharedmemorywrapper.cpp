#include "sharedmemorywrapper.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/adsbsensor.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/aesaradar.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/aissensor.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/radar.h"
#include "core/Hierarchy/EntityProfiles/platform.h"
#include "core/Hierarchy/EntityProfiles/radio.h"
#include "qapplication.h"
#include "qthread.h"
#include "sharedMemorylib/sharedMemory.h"
#include "sharedMemorylib/EntityHandler.h"
#include <csignal>
#include <iostream>
#include <core/Debug/console.h>
#include <cmath>
#include <cstring>
#include <iterator>
#include <unordered_set>
#include "core/Hierarchy/Utils/entityutils.h"
#include <core/Config/scenarioconfig.h>
namespace {
constexpr double AESA_DEG2RAD         = 3.14159265358979323846 / 180.0;
constexpr double AESA_ACC_TAU_S       = 1.0;   // acceleration smoothing (s)
constexpr int    AESA_ACC_MIN_UPDATES = 3;     // updates before acceleration is valid
constexpr double AESA_PRUNE_S         = 5.0;   // drop accel state unseen this long (s)
constexpr double AESA_ACC_MIN_DT_S    = 1.0;   // minimum time between velocity samples (s)

template <size_t N>
void copyStr(char (&dst)[N], const std::string& src)
{
    std::strncpy(dst, src.c_str(), N - 1);
    dst[N - 1] = '\0';
}

int aesaModeToInt(aesa::RadarMode m)
{
    switch (m) {
    case aesa::RadarMode::SURVEILLANCE: return 0;
    case aesa::RadarMode::TWS:          return 1;
    case aesa::RadarMode::LOCK_ON:      return 2;
    }
    return 0;
}

const aesa::BeamWaveform& aesaWaveformFor(const aesa::RadarConfig& cfg, aesa::RadarMode m)
{
    switch (m) {
    case aesa::RadarMode::LOCK_ON: return cfg.fireControlWaveform;
    case aesa::RadarMode::TWS:     return cfg.trackWaveform;
    default:                       return cfg.searchWaveform;
    }
}
// Radar offset (stabilised frame: North, East, Up in metres) -> lat/lon,
// with the engine's own flat-Earth conversion (same as Transform).
// Engine world: x = East, y = Up, z = North (km). Reverse of collectTargets().
void aesaOffsetToLatLon(const QVector3D& ownPosKm, double north_m, double east_m,
                        double up_m, double& lat, double& lon)
{
    const double xKm = ownPosKm.x() + east_m  / 1000.0;
    const double yKm = ownPosKm.y() + up_m    / 1000.0;
    const double zKm = ownPosKm.z() + north_m / 1000.0;
    const GeoPos g   = flatXYZToGeo(xKm, yKm * KMtoFT, zKm);
    lat = g.lat;
    lon = g.lon;
}
} // namespace
SharedMemoryWrapper::SharedMemoryWrapper() {
    shm.create();
    handler = shm.entityHandler;
    std::cout << "[SHM] Entity size: " << sizeof(SharedStructs::Entity)
              << "  SharedData size: " << sizeof(SharedStructs::SharedData) << std::endl;
    // handler->addEntity(SharedStructs::Entity()); // Ek dummy entity add karein
    // std::cout << "Program running... Press Ctrl+C to stop." << std::endl;

    // while(true) {
    //     if(handler->getEntityByIndex(0) != nullptr) {
    //         SharedStructs::Entity* entities = handler->getEntityByIndex(0);
    //         entities->dynamicsData.speed += 1.0f;
    //         std::cout << "Current Speed: " << entities->dynamicsData.speed << "\r" << std::flush;
    //         if(entities->dynamicsData.speed > 10){
    //             break;
    //         }
    //     }
    //     usleep(100000); // 100ms sleep taaki CPU 100% na ho jaye
    // }

}

SharedMemoryWrapper::~SharedMemoryWrapper() {
    std::cout << "sharedmemory delete";
    Console::log("sharedmemory delete");
    shm.cleanup();
    sharedComponent.clear();
}

void SharedMemoryWrapper::entityAdded(QString /*parentID*/, Entity* entity) {

    if (entity->type == Constants::EntityType::Platform) {
        Platform* platform = dynamic_cast<Platform*>(entity);
        // Remote DIS entities have no dynamicModel/trajectory/sensors.
        // SharedMemory is only for local simulation entities.
        // if (platform->isRemoteDISEntity) return;//by Aman
        SharedComponent component;
        component.name = platform->Name;
        component.base = entity;
        component.platform = platform;
        component.transform = &platform->transform;
        component.rigidbody = &platform->rigidbody;
        component.collider = &platform->collider;
        component.dynamicModel = &platform->dynamicModel;
        component.trajectory = &platform->trajectory;
        sharedComponent[platform->ID] = component;
        radarIdToEntity_[AESARadar::platformToRadarID(platform->ID)] = platform->ID;
        SharedStructs::Entity sharedEntity{};
        sharedEntity.setID(entity->ID.c_str());
        sharedEntity.setName(entity->Name.c_str());
        handler->addEntity(sharedEntity);
    }
}

void SharedMemoryWrapper::entityRemoved(QString ID) {
    std::string key = ID.toStdString();
    auto physIt = sharedComponent.find(key);
    if (physIt != sharedComponent.end()) {
        radarIdToEntity_.erase(AESARadar::platformToRadarID(key));
        handler->removeEntityByID(ID.toStdString());
        sharedComponent.erase(physIt);
    }
}

void SharedMemoryWrapper::SimulationUpdate(float deltaTime){
    //return;
    if(ScenarioConfig::sharedData){
        // qDebug()<<"share";
    for (auto& [id, comp] : sharedComponent) {
        SharedStructs::Entity* entity = handler->getEntityByID(id);
        if (!entity) continue;
        updateTransformData(comp,entity);
        updateDynamicData(comp,entity);
        updateTrajectoryData(comp,entity);
        updateSensorData(comp,entity);
        updateRadioData(comp,entity);
        updateAESAData(comp,entity);
    }
    }

    for (auto it = aesaAccel_.begin(); it != aesaAccel_.end(); )
        it = (std::abs(aesaClock_ - it->second.lastSeen) > AESA_PRUNE_S)
                 ? aesaAccel_.erase(it) : std::next(it);
}

void SharedMemoryWrapper::updateTransformData(SharedComponent comp,SharedStructs::Entity* entity){
    QVector3D position = comp.transform->translation();
    QVector3D rotation = comp.transform->toEulerAngles();
    entity->transformData.position.set(position.x(),position.y(),position.z());
    entity->transformData.rotation.set(rotation.x(),rotation.y(),rotation.z());
    entity->transformData.geoCords.set(comp.transform->getLatitude(),
                                       comp.transform->getLongitude(),
                                       comp.transform->getAltitude(),
                                       comp.transform->getHeading()
                                       );
}

void SharedMemoryWrapper::updateDynamicData(SharedComponent comp,SharedStructs::Entity* entity){
    //Maximums
    entity->dynamicsData.setMinSpeed(comp.dynamicModel->minSpeed);
    entity->dynamicsData.setMaxSpeed(comp.dynamicModel->maxSpeed);
    entity->dynamicsData.setMaxAcceleration(comp.dynamicModel->Acceleration);
    entity->dynamicsData.setMaxDeceleration(comp.dynamicModel->Decceleration);
    entity->dynamicsData.setMaxAltitude(comp.dynamicModel->maxAltitude);
    entity->dynamicsData.setClimbRate(comp.dynamicModel->climbRate);

    entity->dynamicsData.setSpeed(comp.dynamicModel->currentSpeed);
    entity->dynamicsData.setAcceleration(comp.dynamicModel->currentAcceralation);
    entity->dynamicsData.setCurrentHeightAboveGround(comp.dynamicModel->currentHeightAboveGround);
    entity->dynamicsData.setPitch(comp.dynamicModel->pitch);
    entity->dynamicsData.setRoll(comp.dynamicModel->roll);
    entity->dynamicsData.setYaw(comp.dynamicModel->yaw);
    entity->dynamicsData.setTrueHeading(comp.dynamicModel->TrueHeading);
    entity->dynamicsData.setPitchRate(comp.dynamicModel->Pitchrate);
    entity->dynamicsData.setRollRate(comp.dynamicModel->Rollrate);
    entity->dynamicsData.setYawRate(comp.dynamicModel->Yawrate);
    entity->dynamicsData.setHorizonPitchRate(comp.dynamicModel->horizonPitchRate);
    entity->dynamicsData.setHorizonRollRate(comp.dynamicModel->horizonRollRate);
    entity->dynamicsData.setTurnRate(comp.dynamicModel->angleOfAttack);
    entity->dynamicsData.setAngleOfAttack(comp.dynamicModel->sideslipAngle);
    entity->dynamicsData.setFlightPathAngle(comp.dynamicModel->flightPathAngle);
    entity->dynamicsData.setTrueAirSpeed(comp.dynamicModel->TrueAirSpeed);
    entity->dynamicsData.setGroundVelocity(comp.dynamicModel->GroundVelocity);
    entity->dynamicsData.setEastVelocity(comp.dynamicModel->EastVelocity);
    entity->dynamicsData.setVerticalVelocity(comp.dynamicModel->VerticalVelocity);
    entity->dynamicsData.setNorthVelocity(comp.dynamicModel->NorthVelocity);
    entity->dynamicsData.setDriftAngle(comp.dynamicModel->DriftAngle);


    QVector3D& groundVelocity = comp.dynamicModel->groundVelocity;
    entity->dynamicsData.groundVelocity.set(groundVelocity.x(),groundVelocity.y(),groundVelocity.z());

    QVector3D& airVelocity = comp.dynamicModel->airVelocity;
    entity->dynamicsData.airVelocity.set(airVelocity.x(),airVelocity.y(),airVelocity.z());

    QVector3D& bodyAirVelocity = comp.dynamicModel->bodyAirVelocity;
    entity->dynamicsData.bodyAirVelocity.set(bodyAirVelocity.x(),bodyAirVelocity.y(),bodyAirVelocity.z());

    QVector3D& bodyGroundVelocity = comp.dynamicModel->bodyGroundVelocity;
    entity->dynamicsData.bodyGroundVelocity.set(bodyGroundVelocity.x(),bodyGroundVelocity.y(),bodyGroundVelocity.z());

    QVector3D& localVelocity = comp.dynamicModel->localVelocity;
    entity->dynamicsData.localVelocity.set(localVelocity.x(),localVelocity.y(),localVelocity.z());

    QVector3D& acceleration = comp.dynamicModel->acceleration;
    entity->dynamicsData.acceleration.set(acceleration.x(),acceleration.y(),acceleration.z());

    QVector3D& bodyAcceleration = comp.dynamicModel->bodyAcceleration;
    entity->dynamicsData.bodyAcceleration.set(bodyAcceleration.x(),bodyAcceleration.y(),bodyAcceleration.z());

    QVector3D& velocity = comp.dynamicModel->velocity;
    entity->dynamicsData.velocity.set(velocity.x(),velocity.y(),velocity.z());

    QVector3D& angularVelocity = comp.dynamicModel->angularVelocity;
    entity->dynamicsData.angularVelocity.set(angularVelocity.x(),angularVelocity.y(),angularVelocity.z());


    // int i =0;
    // for (auto it = comp.dynamicModel->AdditionalParameters.begin(); it != comp.dynamicModel->AdditionalParameters.end(); ++it) {
    //     SharedStructs::parameter* param = new SharedStructs::parameter();
    //     param->setName(it.key().toStdString().c_str());
    //     param->setValue(it.value().toString().toStdString());
    //     entity->dynamicsData.updateCustomParameter(i,*param);
    //     i++;
    // }
    // entity->dynamicsData.setParamCount(i);

}

void SharedMemoryWrapper::updateTrajectoryData(SharedComponent comp,SharedStructs::Entity* entity){
    if(comp.trajectory->Trajectories.size() != entity->trajectoryData.getWaypointCount()){
        if(comp.trajectory->Trajectories.size()<99){
            int i = 0;
            for (const Waypoints* waypoint : comp.trajectory->Trajectories) {
                if (waypoint) {
                    SharedStructs::waypoint* wp = new SharedStructs::waypoint();
                    wp->position.set(waypoint->position->x,waypoint->position->y,waypoint->position->z);
                    wp->geoCords.set(waypoint->geocord->latitude,
                                     waypoint->geocord->longitude,
                                     waypoint->geocord->altitude,
                                     waypoint->geocord->Heading);
                    wp->setspeed(waypoint->speed);
                    entity->trajectoryData.addWaypoint(*wp,i);
                    i++;
                }
            }
            entity->trajectoryData.setWaypointCount(i);
        }else{
            entity->trajectoryData.setWaypointCount(0);
        }
    }
}

void SharedMemoryWrapper::updateSensorData(SharedComponent comp, SharedStructs::Entity* entity)
{
    int aisDataCount = 0;
    int adsbDataCount = 0;

    for (auto const& pair :comp.platform->sensors.sensors)
    {
        Sensor* s = pair.second;
        if (!s) continue;

        if (s->subType == Sensor::SubType::AIS )
        {
            if (aisDataCount >= 100) // Actual radio array capacity ke according
                continue;
            AISSensor* r = dynamic_cast<AISSensor*>(s);
            if (!r) {  continue; }   // guard: skip if cast fails

            for(auto const& ais : r->detect ){
                if (ais.entity)entity->ais[aisDataCount].setID(ais.entity->ID.c_str());
                entity->ais[aisDataCount].mmsi = ais.mmsi;
                entity->ais[aisDataCount].imo_number = ais.imo_number;
                entity->ais[aisDataCount].setName(ais.name);
                entity->ais[aisDataCount].setCallSign(ais.call_sign);
                entity->ais[aisDataCount].setDestination(ais.destination);
                entity->ais[aisDataCount].dims.length = ais.dims.length;
                entity->ais[aisDataCount].dims.beam = ais.dims.beam;
                entity->ais[aisDataCount].dims.draught = ais.dims.draught;
                entity->ais[aisDataCount].ship_type = static_cast<decltype(entity->ais[aisDataCount].ship_type)>(
                    ais.ship_type
                    );;
                entity->ais[aisDataCount].pos.latitude = ais.pos.latitude;
                entity->ais[aisDataCount].pos.longitude = ais.pos.longitude;
                entity->ais[aisDataCount].pos.timestamp = ais.pos.timestamp;
                entity->ais[aisDataCount].sog = ais.sog;
                entity->ais[aisDataCount].cog = ais.cog;
                entity->ais[aisDataCount].heading = ais.heading;
                entity->ais[aisDataCount].rot = ais.rot;
                entity->ais[aisDataCount].nav_status = static_cast<decltype(entity->ais[aisDataCount].nav_status)>(
                    ais.nav_status
                    );
                entity->ais[aisDataCount].eta.month = ais.eta.month;
                entity->ais[aisDataCount].eta.day = ais.eta.day;
                entity->ais[aisDataCount].eta.hour = ais.eta.hour;
                entity->ais[aisDataCount].eta.minute = ais.eta.minute;
                entity->ais[aisDataCount].repeat_indicator = ais.repeat_indicator;
                entity->ais[aisDataCount].ais_channel = ais.ais_channel;
                entity->ais[aisDataCount].is_verified = ais.is_verified;
                entity->ais[aisDataCount].Maneuver_Indicator = 2;
                entity->ais[aisDataCount].radio_status = 1;
                entity->ais[aisDataCount].positionAccuracy = ais.positionAccuracy;
                aisDataCount++;
            }
        }
        else
            if (s->subType == Sensor::SubType::ADSB )
            {
                if (adsbDataCount >= 100) // Actual radio array capacity ke according
                continue;
                ADSBSensor* r = dynamic_cast<ADSBSensor*>(s);
                if (!r) {  continue; }   // guard: skip if cast fails

                for (const auto& adsb : r->detect)
                {
                    auto& dst = entity->adsb[adsbDataCount];

                    dst.setMessageType(adsb.messageType.c_str());
                    dst.setTransmissionType(adsb.transmissiontype.c_str());
                    dst.setSessionId(adsb.session_id.c_str());
                    dst.setAircraftId(adsb.aircraft_id.c_str());
                    dst.setHexIdent(adsb.hex_ident.c_str());
                    dst.setFlightId(adsb.fligh_Id.c_str());

                    dst.setGeneratedDate(adsb.generated_date.c_str());
                    dst.setGeneratedTime(adsb.generated_time.c_str());
                    dst.setLoggedDate(adsb.logged_date.c_str());
                    dst.setLoggedTime(adsb.logged_time.c_str());

                    dst.setCallSign(adsb.call_sign.c_str());
                    dst.setSquawk(adsb.squawk.c_str());

                    dst.altitude         = adsb.altitude;
                    dst.ground_speed     = adsb.ground_speed;
                    dst.track            = adsb.track;
                    dst.lat              = adsb.lat;
                    dst.lon              = adsb.lon;
                    dst.climb_rate       = adsb.climb_rate;

                    dst.alert            = adsb.alert;
                    dst.emergency        = adsb.emergency;
                    dst.spi              = adsb.spi;
                    dst.is_on_ground     = adsb.is_on_ground;
                    dst.emitter_category = adsb.emitter_category;
                    dst.vx = adsb.vx;
                    dst.vy = adsb.vy;
                    dst.vz = adsb.vz;
                    dst.ax = adsb.ax;
                    dst.ay = adsb.ay;
                    dst.az = adsb.az;

                    adsbDataCount++;
                }
            }

    }
    entity->aisCount = aisDataCount;
    entity->adsbCount = adsbDataCount;

}

void SharedMemoryWrapper::updateRadioData(SharedComponent comp, SharedStructs::Entity* entity)
{
    int radioCount = 0;


    for (auto const& pair :comp.platform->radios.radios)
    {
        if (radioCount >= 100) // Actual radio array capacity ke according
            break;
        Radio* s = pair.second;
        if (!s) continue;

        Radio* r = dynamic_cast<Radio*>(s);
        if (!r) {  continue; }   // guard: skip if cast fails

        for(auto const& radio : r->targets ){
            auto& dst = entity->radios[radioCount];
            // Geometry
            dst.distance_m = radio.distance_m;
            dst.radius_m = radio.radius_m;
            dst.azimuth_deg = radio.azimuth_deg;

            // Scanner
            dst.scanner_max_range_m = radio.scanner_max_range_m;
            dst.scanner_frequency_hz = radio.scanner_frequency_hz;

            // Signal parameters
            dst.path_loss_db = radio.path_loss_db;
            dst.rx_power_dbm = radio.rx_power_dbm;
            dst.noise_floor_dbm = radio.noise_floor_dbm;
            dst.snr_db = radio.snr_db;
            dst.rain_attenuation_db = radio.rain_attenuation_db;
            dst.wind_attenuation_db_per_km = radio.wind_attenuation_db_per_km;
            dst.los_horizon_distance_m = radio.los_horizon_distance_m;
            dst.polarization_loss_db = radio.polarization_loss_db;
            dst.required_snr_threshold_db = radio.required_snr_threshold_db;

            // Detection flags
            dst.network_match = radio.network_match;
            dst.frequency_match = radio.frequency_match;
            dst.range_ok = radio.range_ok;
            dst.beam_ok = radio.beam_ok;
            dst.sensitivity_ok = radio.sensitivity_ok;
            dst.squelch_ok = radio.squelch_ok;
            dst.snr_ok = radio.snr_ok;

            // Footprint
            dst.is_footprint = radio.is_footprint;
            dst.heading_deg = radio.heading_deg;
            dst.beamwidth_deg = radio.beamwidth_deg;

            // Messages
            radioCount++;

        }


    }
    entity->radioCount = radioCount;

}

// =============================================================================
// Acceleration from change of Kalman velocity between track updates.
// =============================================================================
bool SharedMemoryWrapper::aesaAcceleration(const std::string& key, const double v[3],
                                           int hitCount, bool validated, double now, double a[3])
{
    AesaAccelState& s = aesaAccel_[key];

    if (!validated || !s.hasV || now < s.t)
    {
        s = AesaAccelState{};
        for (int i = 0; i < 3; ++i) { s.v[i] = v[i]; a[i] = 0.0; }
        s.t = now; s.hits = hitCount; s.hasV = validated; s.lastSeen = now;
        return false;
    }

    if (hitCount != s.hits && (now - s.t) >= AESA_ACC_MIN_DT_S)    {
        const double dt   = now - s.t;
        const double beta = dt / (AESA_ACC_TAU_S + dt);
        for (int i = 0; i < 3; ++i)
        {
            const double raw = (v[i] - s.v[i]) / dt;
            s.a[i] += beta * (raw - s.a[i]);
            s.v[i]  = v[i];
        }
        s.t = now; s.hits = hitCount; ++s.updates;
    }
    s.lastSeen = now;

    for (int i = 0; i < 3; ++i) a[i] = s.a[i];
    return s.updates >= AESA_ACC_MIN_UPDATES;
}

// =============================================================================
// AESA radar -> shared memory (same pattern as AIS / ADS-B).
// =============================================================================
void SharedMemoryWrapper::updateAESAData(SharedComponent comp, SharedStructs::Entity* entity)
{
    if (!comp.platform) return;

    int radarCount   = 0;
    int contactCount = 0;
    const int maxRadars   = static_cast<int>(std::size(entity->aesaRadar));
    const int maxContacts = static_cast<int>(std::size(entity->aesaContact));
    const double now = static_cast<double>(comp.platform->time);
    aesaClock_ = now;
    const QVector3D ownPosKm = comp.transform->translation();   // same as the radar uses

    for (auto const& pair : comp.platform->sensors.sensors)
    {
        Sensor* s = pair.second;
        if (!s || s->subType != Sensor::SubType::AESA) continue;
        if (radarCount >= maxRadars) break;
        AESARadar* r = dynamic_cast<AESARadar*>(s);
        if (!r) continue;

        const aesa::RadarOutput   out = r->getRadarOutput();
        const aesa::RadarConfig   cfg = r->getRadarConfig();
        const aesa::BeamWaveform& wf  = aesaWaveformFor(cfg, out.mode);

        // ---------- radar block ----------
        const int rIdx = radarCount++;
        auto& sys = entity->aesaRadar[rIdx];
        sys = SharedStructs::AESARadarData{};
        copyStr(sys.radarId,   r->ID);
        copyStr(sys.radarName, r->Name);
        sys.emitterIndex          = r->emitterIndex;
        sys.mode                  = aesaModeToInt(out.mode);
        sys.lockedTargetId        = cfg.lockedTargetID;
        sys.simTime_s             = now;
        sys.beamAz_deg            = r->getCurrentAzimuth();
        sys.beamEl_deg            = r->getCurrentElevation();
        sys.frequency_Hz          = cfg.frequency_Hz;
        sys.receiverBandwidth_Hz  = cfg.antennaBandwidth;
        sys.antennaGain_dBi       = cfg.antennaGain;
        sys.beamWidth_deg         = cfg.beamWidth;
        sys.pulseWidth_s          = wf.pulseWidth_s;
        sys.prf_Hz                = wf.prf_Hz;
        sys.prf2_Hz               = wf.prf2_Hz;
        sys.waveformBandwidth_Hz  = wf.bandwidth_Hz;
        sys.dutyCycle             = out.currentDutyCycle;
        sys.maxDutyCycle          = cfg.maxDutyCycle;
        sys.displayRange_km       = out.displayRange_km;
        sys.minAzimuth_deg        = cfg.minAzimuth;
        sys.maxAzimuth_deg        = cfg.maxAzimuth;
        sys.minElevation_deg      = cfg.minElevation;
        sys.maxElevation_deg      = cfg.maxElevation;
        sys.mountAzimuth_deg      = r->mountAzimuth();
        sys.mountElevation_deg    = r->mountElevation();
        sys.numElements           = cfg.numElements;
        sys.failedModules         = cfg.failedModules;
        sys.peakPowerPerElement_W = cfg.peakPowerPerElement_W;
        sys.noiseFigure_dB        = cfg.noiseFigure_dB;
        sys.targetPfa             = cfg.targetPfa;
        sys.frequencyAgility      = cfg.frequencyAgility ? 1 : 0;
        sys.hopStart_Hz           = cfg.hopStartFrequency;
        sys.hopStop_Hz            = cfg.hopStopFrequency;
        sys.radarHeight_m         = cfg.radarHeight;
        sys.platformSpeed_m_s     = cfg.platformSpeed_m_s;

        int radarContacts = 0;

        auto setEntityId = [this](SharedStructs::AESAContactData& c, uint32_t id) {
            auto it = radarIdToEntity_.find(id);
            if (it != radarIdToEntity_.end()) copyStr(c.targetEntityId, it->second);
        };

        // ---------- SURVEILLANCE: detections ----------
        if (out.mode == aesa::RadarMode::SURVEILLANCE)
        {
            std::unordered_set<uint32_t> added;
            for (const auto& d : out.detections)
            {
                if (contactCount >= maxContacts) break;
                if (d.isDRFMGhost) continue;
                if (!added.insert(d.targetID).second) continue;

                auto& c = entity->aesaContact[contactCount++];
                c = SharedStructs::AESAContactData{};
                c.radarIndex = rIdx;
                c.targetId   = d.targetID;
                c.source     = 0;
                setEntityId(c, d.targetID);

                const double h = d.heading * AESA_DEG2RAD;
                c.PR_vel_n_s  = d.speedOverGround * std::cos(h);
                c.PR_vel_e_w  = d.speedOverGround * std::sin(h);
                c.PR_vel_vert = 0.0;
                c.velSource   = 2;
                c.PR_rcs      = d.rcs_m2;
                c.rcsValid    = (d.rcs_m2 > 0.0) ? 1 : 0;

                c.range_m             = d.range;
                c.azimuth_deg         = d.azimuth;
                c.elevation_deg       = d.elevation;
                c.altitude_m          = d.range * std::sin(d.elevation * AESA_DEG2RAD) + cfg.radarHeight;
                const double azR   = d.azimuth   * AESA_DEG2RAD;
                const double elR   = d.elevation * AESA_DEG2RAD;
                const double horiz = d.range * std::cos(elR);
                aesaOffsetToLatLon(ownPosKm, horiz * std::cos(azR), horiz * std::sin(azR),
                                   d.range * std::sin(elR), c.lat_deg, c.lon_deg);
                c.speedOverGround_m_s = d.speedOverGround;
                c.heading_deg         = d.heading;
                c.radialVelocity_m_s  = d.radialVelocity;
                c.snr_linear          = d.snr;
                c.snr_dB              = (d.snr > 0.0) ? 10.0 * std::log10(d.snr) : -999.0;
                c.Pk                  = d.Pk;
                c.cpa_distance_m      = d.cpa_distance;
                c.time_to_cpa_s       = d.time_to_cpa;
                c.targetAspect_deg    = d.targetAspect;
                c.ambiguous           = d.isAmbiguous ? 1 : 0;
                c.inDopplerBlind      = d.inDopplerBlind ? 1 : 0;
                ++radarContacts;
            }
        }
        // ---------- TWS / LOCK_ON: tracks ----------
        else
        {
            const bool lockOn = (out.mode == aesa::RadarMode::LOCK_ON);
            if (!(lockOn && out.lockBroken))
            {
                for (const auto& tr : out.tracks)
                {
                    if (contactCount >= maxContacts) break;
                    if (tr.isDRFMSuspect) continue;
                    if (lockOn && tr.id != cfg.lockedTargetID) continue;

                    auto& c = entity->aesaContact[contactCount++];
                    c = SharedStructs::AESAContactData{};
                    c.radarIndex = rIdx;
                    c.targetId   = tr.id;
                    c.source     = lockOn ? 2 : 1;
                    setEntityId(c, tr.id);

                    c.PR_vel_n_s  = tr.vx;   // North
                    c.PR_vel_e_w  = tr.vy;   // East
                    c.PR_vel_vert = tr.vz;   // Up
                    c.velSource   = 1;
                    c.PR_rcs      = tr.rcs_m2;
                    c.rcsValid    = (tr.rcs_m2 > 0.0) ? 1 : 0;

                    const double v[3] = { tr.vx, tr.vy, tr.vz };
                    double a[3] = { 0.0, 0.0, 0.0 };
                    const std::string key = r->ID + ":" + std::to_string(tr.id);
                    if (aesaAcceleration(key, v, tr.hitCount, tr.isValidated, now, a))
                    {
                        c.PR_acc_n_s  = a[0];
                        c.acc_e_w     = a[1];
                        c.PR_acc_vert = a[2];
                        c.accValid    = 1;
                    }

                    c.range_m             = tr.range;
                    c.azimuth_deg         = tr.azimuth;
                    c.elevation_deg       = tr.elevation;
                    c.altitude_m          = tr.z + cfg.radarHeight;
                    aesaOffsetToLatLon(ownPosKm, tr.x, tr.y, tr.z, c.lat_deg, c.lon_deg);
                    c.speedOverGround_m_s = tr.speedOverGround;
                    c.heading_deg         = tr.heading;
                    c.radialVelocity_m_s  = tr.radialVelocity;
                    c.Pk                  = tr.Pk;
                    c.cpa_distance_m      = tr.cpa_distance;
                    c.time_to_cpa_s       = tr.time_to_cpa;
                    c.targetAspect_deg    = tr.targetAspect;
                    c.validated           = tr.isValidated ? 1 : 0;
                    c.manoeuvring         = tr.isManoeuvring ? 1 : 0;
                    c.externalTrack       = tr.isExternalTrack ? 1 : 0;
                    c.ambiguous           = tr.wasAmbiguous ? 1 : 0;
                    c.hitCount            = tr.hitCount;
                    c.trackQuality        = tr.trackQuality;
                    c.iffResponse         = static_cast<int32_t>(tr.iff.response);
                    c.iffSquawk           = tr.iff.squawk;
                    c.iffConfidence       = tr.iff.confidence;
                    ++radarContacts;
                }
            }
        }
        sys.contactCount = radarContacts;
    }

    entity->aesaRadarCount   = radarCount;
    entity->aesaContactCount = contactCount;
}

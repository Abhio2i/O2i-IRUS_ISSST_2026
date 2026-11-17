#ifndef CSM_H
#define CSM_H
#include "core/Hierarchy/EntityProfiles/sensor.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmtypes.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmspectrum.h"
#include "core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmmonitor.h"
#include <limits>
#include<cmath>
#include <map>
#include <memory>
#include <string>
class CSM: public Sensor
{
    Q_OBJECT
public:
    explicit CSM(Hierarchy* h);
    void scan() override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
    QJsonObject AdditionalParameters;
    float sensitivityDbm = std::numeric_limits<float>::quiet_NaN();
    float minSnrDb = 0.0f;


    bool autoMonitor = false;

    bool scanMode = false;
    float scanStepMhz = 100.0f;
    float scanDwellS = 0.5f;


    csm::OperatingMode operatingMode() const { return opMode_; }
    void setOperatingMode(csm::OperatingMode m);


    bool  txEnabled = true;
    float txFrequencyGhz = 0.15f;
    float txBandwidthMhz = 10.0f;
    float txPowerDbm = 30.0f;


    bool isTransmitting() const { return Active && opMode_ == csm::OperatingMode::Transmit && txEnabled; }


    bool interferenceSource(csm::InterfererInput& out) const;


    float jamRangeKm = 100.0f;  // km; 0 still means "use sensor Range"
    bool  jamFrequencyGate = true;
    float effectiveJamRangeKm() const {
        const float r = jamRangeKm > 0.0f ? jamRangeKm : range;
        return (std::isfinite(r) && r > 0.0f) ? r : 0.0f;
    }
    bool jamBandHz(double& loHz, double& hiHz) const;
    const csm::monitor::Window& lastMonitor() const
    {
        static const csm::monitor::Window kNone;
        return Active ? lastMonitor_ : kNone;
    }


    const csm::ScanResult& lastScanResult() const
    {
        static const csm::ScanResult kNone;
        return Active ? lastScan_ : kNone;
    }


    std::shared_ptr<const csm::spectrum::Result> lastSpectrum() const
    {
        return Active ? spectrumCache_.result()
                      : std::shared_ptr<const csm::spectrum::Result>();
    }

private:
    void clearScanState();
    void dropPlatformTargets();
    csm::OperatingMode opMode_ = csm::OperatingMode::Receive;
    csm::ScanResult lastScan_;
    csm::spectrum::Cache spectrumCache_;
    csm::monitor::Window lastMonitor_;
    std::map<std::string, csm::Observation> held_;
    double lastScanTimeS_ = -1.0;
};

#endif // CSM_H

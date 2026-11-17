#ifndef SONOBUOY_H
#define SONOBUOY_H

#include "core/Hierarchy/EntityProfiles/weapon.h"
#include <unordered_set>

class Sonobuoy: public Weapon
{
    Q_OBJECT
public:
    struct SonobuoyOutput{
        uint32_t buoyID;             // Unique identifier for the buoy
        double timestamp;            // Simulation time of transmission
        Platform *entity;
        // Positional Data
        double latitude;         // Current GPS lat (including drift)
        double longitude;        // Current GPS lon
        float depth;             // Hydrophone deployment depth (meters)

        float radius;
        // Acoustic Output
        float bearing;           // Magnetic bearing to detected noise (0-360)
        float signalStrength;    // Signal-to-noise ratio in dB
        float dopplerShift;      // Frequency shift for moving targets
        float* rawSpectrum;      // Pointer to frequency bin data (LOFAR)
        bool isScuttled;             // Status of the buoy (Active/Dead)
    };

    explicit Sonobuoy(Hierarchy* h);
    ~Sonobuoy();
    // WeaponType weaponType = WeaponType::Sonobuoy;

    bool drop = false;
    float starttime = 0;
    float time = 0.f;//s
    float range =  30;//km
    float TransmissionRange = 100;//km
    float Depth = 500;//m
    float Life = 2.f;//hours

    std::unordered_set<Platform*> detects;
    QVector<SonobuoyOutput> detection;

    void dropped();
    void Update()override;

    void spawn() override;
    void SyncByJson(const QJsonObject& obj) override{ Q_UNUSED(obj);};
    std::vector<std::string> getSupportedComponents() override;

    void addSupportedcomponent() override;

    void addComponent(std::string name) override;

    void removeComponent(std::string name) override;

    QJsonObject getComponent(std::string name) override;

    void updateComponent(QString name, const QJsonObject& obj) override;

    QJsonObject toJson()                        const override;
    void        fromJson(const QJsonObject&)          override;
    QJsonObject AdditionalParameters;   //!< User-defined extension parameters
    void SaveState();
    void RestoreState();
private:
    bool statesaved = false;
    QJsonObject InitalState;
};

#endif // SONOBUOY_H

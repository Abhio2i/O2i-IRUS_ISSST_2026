#ifndef CHAFF_H
#define CHAFF_H

#include "core/Hierarchy/EntityProfiles/weapon.h"

class Chaff: public Weapon
{
    Q_OBJECT
public:
    explicit Chaff(Hierarchy* h);
    ~Chaff();

    float CloudRadius = 10.0f;           // meters
    float CloudDensity = 1.0f;           // normalized (0-1)
    float DeploymentTime = 0.0f;         // seconds
    float ExpansionRate = 1.0f;          // m/s
    float EffectiveLifetime = 30.0f;     // seconds
    float RadarSignature = 1.0f;         // normalized
    float DriftVelocity = 0.0f;          // m/s
    float DispersionFactor = 1.0f;       // dimensionless

    int ParticleCount = 100;

    void Update()override{};
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

#endif // CHAFF_H

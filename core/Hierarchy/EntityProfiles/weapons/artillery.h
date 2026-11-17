#ifndef ARTILLERY_H
#define ARTILLERY_H

#include "core/Hierarchy/EntityProfiles/weapon.h"

class Artillery: public Weapon
{
    Q_OBJECT
public:
    explicit Artillery(Hierarchy* h);
    ~Artillery();

    float MuzzleVelocity = 300.0f;       // m/s
    float ProjectileMass = 5.0f;         // kg
    float Caliber = 5.0f;                // mm
    float ElevationAngle = 45.0f;        // degrees
    float AzimuthAngle = 0.0f;           // degrees
    float DragCoefficient = 0.01f;       // dimensionless
    float MaxFlightTime = 120.0f;        // seconds
    bool GravityEnabled = true;

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

#endif // ARTILLERY_H

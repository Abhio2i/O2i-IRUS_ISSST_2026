#ifndef ROCKET_H
#define ROCKET_H

#include "core/Hierarchy/EntityProfiles/weapon.h"
class Rocket: public Weapon
{
    Q_OBJECT
public:
    explicit Rocket(Hierarchy* h);
    ~Rocket();

    float LaunchSpeed = 100.0f;          // m/s
    float CurrentSpeed = 100.0f;         // m/s
    float Acceleration = 20.0f;          // m/s²
    float BurnDuration = 10.0f;          // seconds
    float Mass = 20.0f;                  // kg
    float DragCoefficient = 0.1f;        // dimensionless
    float LaunchElevation = 45.0f;       // degrees
    float LaunchAzimuth = 0.0f;          // degrees
    float FlightTime = 0.0f;             // seconds

    bool GravityEnabled = true;
    bool PropulsionEnabled = true;

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

#endif // ROCKET_H

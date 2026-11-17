#ifndef TORPEDO_H
#define TORPEDO_H

#include "core/Hierarchy/EntityProfiles/weapon.h"
class Torpedo: public Weapon
{
    Q_OBJECT
public:
    explicit Torpedo(Hierarchy* h);
    ~Torpedo();

    float LaunchDepth = 50.0f;           // meters
    float CurrentDepth = 50.0f;          // meters
    float Speed = 10.0f;                 // m/s
    float MaxSpeed = 25.0f;              // m/s
    float Acceleration = 2.0f;           // m/s²
    float TurnRate = 5.0f;               // degrees/s
    float MaxOperatingDepth = 500.0f;    // meters
    float OperatingLifetime = 600.0f;    // seconds
    float WaterCurrent = 0.0f;           // m/s

    bool GuidanceEnabled = true;
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

#endif // TORPEDO_H

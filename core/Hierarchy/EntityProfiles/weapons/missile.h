#ifndef MISSILE_H
#define MISSILE_H

#include "core/Hierarchy/EntityProfiles/weapon.h"

class Missile: public Weapon
{
    Q_OBJECT
public:
    explicit Missile(Hierarchy* h);
    ~Missile();

    float InitialSpeed = 200.0f;         // m/s
    float CurrentSpeed = 200.0f;         // m/s
    float MaxSpeed = 500.0f;             // m/s
    float Acceleration = 20.0f;          // m/s²
    float MaxAcceleration = 50.0f;       // m/s²
    float MaxTurnRate = 10.0f;           // degrees/s
    float MaxPitchRate = 10.0f;          // degrees/s
    float MaxFlightTime = 300.0f;       // seconds

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

#endif // MISSILE_H

#ifndef BOMB_H
#define BOMB_H

#include "core/Hierarchy/EntityProfiles/weapon.h"

class Bomb: public Weapon
{
    Q_OBJECT
public:
    explicit Bomb(Hierarchy* h);
    ~Bomb();
    // WeaponType weaponType = WeaponType::Sonobuoy;

    float BombMass = 100.0f;             // kg
    float ReleaseAltitude = 3000.0f;     // meters
    float ReleaseSpeed = 200.0f;         // m/s
    float DragCoefficient = 0.1f;        // dimensionless
    float FallTime = 0.0f;               // seconds
    float MaxFlightTime = 120.0f;        // seconds

    bool GravityEnabled = true;
    bool StabilizationEnabled = false;
    bool GuidanceEnabled = false;

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

#endif // BOMB_H

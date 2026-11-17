#ifndef FLARE_H
#define FLARE_H

#include "core/Hierarchy/EntityProfiles/weapon.h"
class Flare: public Weapon
{
    Q_OBJECT
public:
    explicit Flare(Hierarchy* h);
    ~Flare();

    float InitialIntensity = 0.5f;       // normalized (0-1)
    float PeakIntensity = 1.0f;          // normalized (0-1)
    float BurnDuration = 5.0f;           // seconds
    float IntensityDecayRate = 0.2f;     // 1/s
    float DeploymentVelocity = 10.0f;    // m/s
    float DragCoefficient = 0.1f;        // dimensionless
    float VisualBrightness = 1.0f;       // normalized (0-1)

    int ParticleCount = 100;

    bool GravityEnabled = true;
    bool SmokeEnabled = true;

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

#endif // FLARE_H

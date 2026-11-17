
// =============================================================================
// FILE:        weapon.h
// MODULE:      Tactical Simulation Ordnance Systems
// PROJECT:     Tactical Display/Simulation Framework (TDF)
// ORGANISATION: Oxygen 2 Innovation (O2I)
// STANDARD:    RTCA DO-178C / ED-12C, DAL B (Guidelines applied)
//
// DESCRIPTION:  Defines the Weapon abstract base class. This module serves as
//               the foundation for all ordnance types (Missiles, Bombs, etc.),
//               managing targeting logic, flight monitoring, and detonation
//               parameters within the tactical simulation.
//
// AUTHOR:       Waris
// REVIEWED BY:  [Reviewer Name], [Review Date]
//
// CHANGE HISTORY:
//   Rev 1  Sep 2025  Initial implementation for TDF project.
//   Rev 2  Dec 2025  Integrated WeaponTarget and flight check timers.
//   Rev 3  Mar 2026  Added support for dynamic target platform tracking.
//   Rev 4  Apr 2026  Aligned with DO-178C documentation standards for O2I.
//
// COPYRIGHT:    Oxygen 2 Innovation (O2I). All rights reserved.
// =============================================================================
#ifndef WEAPON_H
#define WEAPON_H

#include <core/Hierarchy/entity.h>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QElapsedTimer>
#include <string>

// ── Auto-integrated component headers ────────────────────────────────────────
#include <core/Hierarchy/Components/transform.h>
#include <core/Hierarchy/Components/rigidbody.h>
#include <core/Hierarchy/Components/collider.h>
#include <core/Hierarchy/Components/trajectory.h>
#include <core/Hierarchy/Components/meshrenderer2d.h>
#include <core/Hierarchy/Components/dynamicmodel.h>
#include <core/Hierarchy/Components/crosssection.h>

class Platform;
class CanvasWidget;

// ── WeaponTarget struct (same as Sensor's Target struct) ─────────────────────
struct WeaponTarget {
    Platform   *entity    = nullptr;
    std::string name;
    float distance    = 0.0f;
    float bearing     = 0.0f;
    float elevation   = 0.0f;
    float closureRate = 0.0f;
    float aspect      = 0.0f;
    bool  inGimbalLock = false;
};

class Weapon : public Entity
{
    Q_OBJECT
public:
    explicit Weapon(Hierarchy* h);
    ~Weapon() override;

    // ── WeaponType enum (same role as Sensor::SubType) ────────────────────────
    enum  WeaponType     { Missile, Sonobuoy, Bomb, Torpedo, Artillery, Rocket, Flare, Chaff };
    const std::string weaponNames[8] = {"Missile", "Sonobuoy", "Bomb", "Torpedo", "Artillery", "Rocket", "Flare", "Chaff"};

    enum  GuidanceType   { Unguided, SemiActive, FullyActive, PassiveInfrared,
                              CommandGuided, InertialGuidance };
    const std::string guidanceNames[6] = {"Unguided", "SemiActive", "FullyActive", "PassiveInfrared", "CommandGuided", "InertialGuidance"};

    enum  DetonationType { Impact, Proximity, Timed, Command };
    const std::string detonationNames[4] = {"Impact", "Proximity", "Timed", "Command"};

    enum  PropulsionType { SolidRocket, LiquidRocket, Turbofan, Ramjet, Turboprop, Gravity };
    const std::string propulsionNames[6] = {"SolidRocket", "LiquidRocket", "Turbofan", "Ramjet", "Turboprop", "Gravity"};


    // ── Type tag (overridden by each subclass, same role as Sensor::subType) ──
    WeaponType weaponType = WeaponType::Missile;
    GuidanceType guidanceType = GuidanceType::InertialGuidance;
    DetonationType detonationType = DetonationType::Proximity;
    PropulsionType propulsionType = PropulsionType::Gravity;

    QString weaponTypeName() const{return QString::fromStdString(weaponNames[static_cast<int>(weaponType)]);}
    QString guidanceTypeName() const{return QString::fromStdString(guidanceNames[static_cast<int>(guidanceType)]);}

    QString detonationTypeName() const{return QString::fromStdString(detonationNames[static_cast<int>(detonationType)]);}

    QString propulsionTypeName() const{return QString::fromStdString(propulsionNames[static_cast<int>(propulsionType)]);}

    // ── Ownership ─────────────────────────────────────────────────────────────
    Entity *parentEntity = nullptr;

    // =========================================================================
    // AUTO-INTEGRATED COMPONENTS  (created in constructor, same as Platform)
    // =========================================================================
    Transform      transform      ;
    Collider       collider       ;
    // Trajectory     trajectory  ;
    MeshRenderer2D meshRenderer2d ;
    // CrossSection   crossSection;

    QVector3D Velocity;
    float Speed;
    QVector3D TargetPosition;
    Platform* TargetEntity;
    QJsonObject getWeaponTypeObj() const;
    void setWeaponTypeObj(const QJsonObject &obj);

    QJsonObject getGuidenceTypeObj() const;
    void setGuidenceTypeObj(const QJsonObject &obj);

    QJsonObject getDetonationTypeObj() const;
    void setDetonationTypeObj(const QJsonObject &obj);

    QJsonObject getPropulsionTypeObj() const;
    void setPropulsionTypeObj(const QJsonObject &obj);
    virtual void Update() = 0;


private:
    bool statesaved = false;
    QJsonObject InitalState;


};

#endif // WEAPON_H

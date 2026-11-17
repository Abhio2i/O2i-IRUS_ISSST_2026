
#include "weapon.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include <core/Hierarchy/hierarchy.h>
#include <core/Debug/console.h>
#include <core/GlobalRegistry.h>
#include <cmath>
#include <QtMath>
#include <QVector3D>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =============================================================================
// CONSTRUCTOR  — creates entity + auto-integrates all 7 components
// =============================================================================
Weapon::Weapon(Hierarchy* h) : Entity(h)
{
    type  = Constants::EntityType::Weapon;

}


// =============================================================================
// DESTRUCTOR
// =============================================================================
Weapon::~Weapon()
{

}

QJsonObject Weapon::getWeaponTypeObj() const {
    QJsonObject weaponTypeObj;
    weaponTypeObj["type"] = "option";
    weaponTypeObj["refresh"] = true;
    weaponTypeObj["editable"] = false;
    QJsonArray weaponoptionsArray;
    for (const std::string& opt : weaponNames)
        weaponoptionsArray.append(QString::fromStdString(opt));
    weaponTypeObj["options"] = weaponoptionsArray;
    weaponTypeObj["value"] = QString::fromStdString(weaponNames[weaponType]);
    return weaponTypeObj;
}


void Weapon::setWeaponTypeObj(const QJsonObject &obj){
    QJsonObject WeaponTypeObj = obj;
    if (WeaponTypeObj.contains("value")){
        for (int i = 0; i < 8 ; i++) {
            if (weaponNames[i] == WeaponTypeObj["value"].toString().toStdString()) {
                weaponType = (WeaponType)i;
            }
        }
    }
}


QJsonObject Weapon::getGuidenceTypeObj() const {
    QJsonObject GuidenceTypeObj;
    GuidenceTypeObj["type"] = "option";
    GuidenceTypeObj["refresh"] = true;
    GuidenceTypeObj["editable"] = false;
    QJsonArray guidanceoptionsArray;
    for (const std::string& opt : guidanceNames)
    guidanceoptionsArray.append(QString::fromStdString(opt));
    GuidenceTypeObj["options"] = guidanceoptionsArray;
    GuidenceTypeObj["value"] = QString::fromStdString(guidanceNames[guidanceType]);
    return GuidenceTypeObj;
}


void Weapon::setGuidenceTypeObj(const QJsonObject &obj){
    QJsonObject GuidenceTypeObj = obj;
    if (GuidenceTypeObj.contains("value")){
        for (int i = 0; i < 6 ; i++) {
            if (guidanceNames[i] == GuidenceTypeObj["value"].toString().toStdString()) {
                guidanceType = (GuidanceType)i;
            }
        }
    }
}

QJsonObject Weapon::getDetonationTypeObj() const {
    QJsonObject DetonationTypeObj;
    DetonationTypeObj["type"] = "option";
    DetonationTypeObj["refresh"] = true;
    DetonationTypeObj["editable"] = false;
    QJsonArray DetonationoptionsArray;
    for (const std::string& opt : detonationNames)
        DetonationoptionsArray.append(QString::fromStdString(opt));
    DetonationTypeObj["options"] = DetonationoptionsArray;
    DetonationTypeObj["value"] = QString::fromStdString(detonationNames[detonationType]);
    return DetonationTypeObj;
}


void Weapon::setDetonationTypeObj(const QJsonObject &obj){
    QJsonObject DetonationTypeObj = obj;
    if (DetonationTypeObj.contains("value")){
        for (int i = 0; i < 4; i++) {
            if (detonationNames[i] == DetonationTypeObj["value"].toString().toStdString()) {
                detonationType = (DetonationType)i;
            }
        }
    }
}


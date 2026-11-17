/**
 * @file iff.cpp
 * @brief Implementation of the IFF (Identification Friend or Foe) entity.
 */

#include "iff.h"
#include <core/Hierarchy/hierarchy.h>
#include <core/Debug/console.h>
#include <core/GlobalRegistry.h>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <core/Hierarchy/Components/transform.h>
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/EntityProfiles/iff.h"
#include <unordered_set>
#include <QVector3D>
#include <QSet>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float RAD2DEG = 180.0f / M_PI;

// Dropdown labels and serialized values: keep order aligned with IFFRole.
static const std::string IFFRoleNames[3] = {
    "Interrogator", "Responder", "Both"
};
static const std::string IFFModeNames[7] = {
    "Mode1", "Mode2", "Mode3A", "ModeC", "Mode4", "Mode5", "ModeS"
};

static bool readOptionValue(const QJsonObject& object, const char* key, QString& value)
{
    if (!object.contains(key)) return false;
    const QJsonValue v = object.value(key);
    if (v.isString()) { value = v.toString(); return true; }
    if (v.isObject()) {
        const QJsonValue stored = v.toObject().value("value");
        if (stored.isString()) { value = stored.toString(); return true; }
    }
    return false;
}


/**
 * @brief Constructs an IFF entity.
 * @param h Pointer to the parent Hierarchy.
 */
IFF::IFF(Hierarchy* h) : Entity(h) {
    type = Constants::EntityType::IFF;
    std::shared_ptr<Parameter> par = std::make_shared<Parameter>();
    par->Name = "iff_param";
    par->type = Constants::ParameterType::FLOAT;
    par->value = 0.0f;
    parameters["iff_param"] = par;
}

/**
 * @brief Emits signals to notify the hierarchy that this IFF has been added.
 */
void IFF::spawn() {
    // Hierarchy* parent = GlobalRegistry::getParentHierarchy(this);
    emit root->entityAdded(QString::fromStdString(parentID), QString::fromStdString(ID), QString::fromStdString(Name));
}

/**
 * @brief Returns an empty list – IFF does not support child components.
 * @return Empty vector.
 */
std::vector<std::string> IFF::getSupportedComponents() {
    return std::vector<std::string>{};
}

void IFF::addSupportedcomponent(){

}

/**
 * @brief Adds a component – not supported for IFF.
 * @param name Component name (ignored).
 */
void IFF::addComponent(std::string name) {
    Console::error("IFF does not support components: " + name);
}

/**
 * @brief Removes a component – not supported for IFF.
 * @param name Component name (ignored).
 */
void IFF::removeComponent(std::string name) {
    Console::error("IFF does not support components: " + name);
}

/**
 * @brief Retrieves component JSON – not supported.
 * @param name Component name.
 * @return Empty JSON object.
 */
QJsonObject IFF::getComponent(std::string name) {
    Console::error("IFF does not support components: " + name);
    return QJsonObject();
}

/**
 * @brief Updates the IFF entity from JSON data (only basic fields and IFF-specific parameters).
 * @param name Component name (unused).
 * @param obj JSON object containing update data.
 */
void IFF::updateComponent(QString name, const QJsonObject& obj) {
    Console::error(name.toStdString() + ": IFF does not support components");

    // Update basic entity fields
    if (obj.contains("name"))
        Name = obj["name"].toString().toStdString();

    if (obj.contains("active"))
        Active = obj["active"].toBool();

    if (obj.contains("parent_id"))
        parentID = obj["parent_id"].toString().toStdString();


    // Update IFF-specific fields
    if (obj.contains("transponder")) {
        transponder = obj["transponder"].toBool();
        // qDebug() << "[IFF] Updated transponder to" << transponder
        //          << "for:" << QString::fromStdString(Name);
    } else {
        // qDebug() << "[IFF] No 'transponder' key in JSON for:"
        //          << QString::fromStdString(Name)
        //          << "| Keys are:" << obj.keys();
    }

    if (obj.contains("emittingRange"))
        emittingRange = static_cast<float>(obj["emittingRange"].toDouble());

    if (obj.contains("emittingFrequency"))
        emittingFrequency = static_cast<float>(obj["emittingFrequency"].toDouble());

    if (obj.contains("disType"))
        disType = obj["disType"].toString().toStdString();

    if (obj.contains("disName"))
        disName = obj["disName"].toString().toStdString();

    if (obj.contains("operationalMode"))
        operationalMode = stringToOperationalMode(obj["operationalMode"].toString());

    if (obj.contains("modeConfiguration") && obj["modeConfiguration"].isObject()) {
        QJsonObject modeConfigObj = obj["modeConfiguration"].toObject();
        if (modeConfigObj.contains("mode1"))
            modeConfiguration.mode1 = modeConfigObj["mode1"].toString().toStdString();
        if (modeConfigObj.contains("mode2"))
            modeConfiguration.mode2 = modeConfigObj["mode2"].toString().toStdString();
        if (modeConfigObj.contains("mode3A"))
            modeConfiguration.mode3A = modeConfigObj["mode3A"].toString().toStdString();
        if (modeConfigObj.contains("mode4"))
            modeConfiguration.mode4 = modeConfigObj["mode4"].toString().toStdString();
        if (modeConfigObj.contains("modeC"))
            modeConfiguration.modeC = modeConfigObj["modeC"].toString().toStdString();
    }

    if (obj.contains("codeSystem"))
        codeSystem = stringToCodeSystem(obj["codeSystem"].toString());

    if (obj.contains("encryptionType"))
        encryptionType = stringToEncryptionType(obj["encryptionType"].toString());

    if (obj.contains("spoofable"))
        spoofable = obj["spoofable"].toBool();

    if (obj.contains("responseDelay"))
        responseDelay = static_cast<float>(obj["responseDelay"].toDouble());

    if (obj.contains("lastInterrogationTime"))
        lastInterrogationTime = obj["lastInterrogationTime"].toString().toStdString();

    // Flat updates; toJson()/fromJson() use the established "default" section.
    if (obj.contains("interrogatorEnabled")) interrogatorEnabled = obj["interrogatorEnabled"].toBool();
    if (obj.contains("replyFrequency")) replyFrequency = float(obj["replyFrequency"].toDouble());
    if (obj.contains("activeIFFMode")) activeIFFMode = obj["activeIFFMode"].toString().toStdString();
    if (obj.contains("enableMode1")) enableMode1 = obj["enableMode1"].toBool();
    if (obj.contains("enableMode2")) enableMode2 = obj["enableMode2"].toBool();
    if (obj.contains("enableMode3A")) enableMode3A = obj["enableMode3A"].toBool();
    if (obj.contains("enableModeC")) enableModeC = obj["enableModeC"].toBool();
    if (obj.contains("enableMode4")) enableMode4 = obj["enableMode4"].toBool();
    if (obj.contains("enableMode5")) enableMode5 = obj["enableMode5"].toBool();
    if (obj.contains("enableModeS")) enableModeS = obj["enableModeS"].toBool();
    if (obj.contains("modeConfiguration") && obj["modeConfiguration"].isObject()) {
        const auto cfg = obj["modeConfiguration"].toObject();
        if (cfg.contains("mode5")) modeConfiguration.mode5 = cfg["mode5"].toString().toStdString();
        if (cfg.contains("modeS")) modeConfiguration.modeS = cfg["modeS"].toString().toStdString();
    }

    // Existing updateComponent() accepts flat edits. Also accept the nested default section.
    const QJsonObject roleSource = obj.value("default").isObject() ? obj.value("default").toObject() : obj;
    QString requestedRole;
    bool roleFound = readOptionValue(roleSource, "IFFRole", requestedRole);
    bool validRole = false;
    if (roleFound) {
        for (int i = 0; i < 3; ++i) {
            if (requestedRole.toStdString() == IFFRoleNames[i]) {
                role = static_cast<IFFRole>(i);
                validRole = true;
                break;
            }
        }
    }
    if (validRole) {
        interrogatorEnabled = role != IFFRole::Responder;
        transponder = role != IFFRole::Interrogator;
    } else if (roleSource.contains("interrogatorEnabled") || roleSource.contains("transponder")) {
        // Compatibility with older saved configs using two booleans.
        if (roleSource.contains("interrogatorEnabled")) interrogatorEnabled = roleSource.value("interrogatorEnabled").toBool();
        if (roleSource.contains("transponder")) transponder = roleSource.value("transponder").toBool();
        // Both false is treated as inactive (existing operationalMode / Active).
        role = interrogatorEnabled && transponder ? IFFRole::Both
               : interrogatorEnabled ? IFFRole::Interrogator : IFFRole::Responder;
    }

    QString chosenMode;
    if (readOptionValue(roleSource, "activeIFFMode", chosenMode)) {
        for (const auto& opt : IFFModeNames)
            if (chosenMode.toStdString() == opt) { activeIFFMode = opt; break; }
    }
}

/**
 * @brief Serializes the IFF entity to JSON.
 * @return QJsonObject containing IFF parameters.
 */

QJsonObject IFF::toJson() const {
    QJsonObject obj;
    obj["name"] = QString::fromStdString(Name);
    obj["branch"] = QString::fromStdString("Entity");
    obj["id"] = QString::fromStdString(ID);
    obj["parent_id"] = QString::fromStdString(parentID);
    obj["active"] = Active;

    QJsonObject defaultObj;
    defaultObj["type"] = "Section";
    defaultObj["transponder"] = (role != IFFRole::Interrogator) && transponder;
    defaultObj["emittingRange"] = toParm(emittingRange,"km",  0,  500);
    defaultObj["emittingFrequency"] = toParm(emittingFrequency,"Mhz", 0,  3000);
    defaultObj["code"] = toParm(code,"code");
    defaultObj["interrogatorEnabled"] = (role != IFFRole::Responder) && interrogatorEnabled;
    defaultObj["replyFrequency"] = toParm(replyFrequency, "MHz", 0, 3000);

    // Same option-schema as the existing Category dropdown in the project.
    QJsonObject roleObj;
    roleObj["type"] = "option";
    roleObj["refresh"] = true;
    roleObj["editable"] = false;
    QJsonArray roleOptionsArray;
    for (const std::string& opt : IFFRoleNames)
        roleOptionsArray.append(QString::fromStdString(opt));
    roleObj["options"] = roleOptionsArray;
    roleObj["value"] = QString::fromStdString(IFFRoleNames[static_cast<int>(role)]);
    defaultObj["IFFRole"] = roleObj;

    QJsonObject modeOptionObj;
    modeOptionObj["type"] = "option";
    modeOptionObj["refresh"] = true;
    modeOptionObj["editable"] = false;
    QJsonArray modeOptionsArray;
    for (const std::string& opt : IFFModeNames)
        modeOptionsArray.append(QString::fromStdString(opt));
    modeOptionObj["options"] = modeOptionsArray;
    modeOptionObj["value"] = QString::fromStdString(activeIFFMode);
    defaultObj["activeIFFMode"] = modeOptionObj;
    defaultObj["enableMode1"] = enableMode1;
    defaultObj["enableMode2"] = enableMode2;
    defaultObj["enableMode3A"] = enableMode3A;
    defaultObj["enableModeC"] = enableModeC;
    defaultObj["enableMode4"] = enableMode4;
    defaultObj["enableMode5"] = enableMode5;
    defaultObj["enableModeS"] = enableModeS;
    QJsonObject modeObj;
    modeObj["type"] = "Section";
    modeObj["mode1"] = QString::fromStdString(modeConfiguration.mode1);
    modeObj["mode2"] = QString::fromStdString(modeConfiguration.mode2);
    modeObj["mode3A"] = QString::fromStdString(modeConfiguration.mode3A);
    modeObj["modeC"] = QString::fromStdString(modeConfiguration.modeC);
    modeObj["mode4"] = QString::fromStdString(modeConfiguration.mode4);
    modeObj["mode5"] = QString::fromStdString(modeConfiguration.mode5);
    modeObj["modeS"] = QString::fromStdString(modeConfiguration.modeS);
    obj["modeConfiguration"] = modeObj;
    obj["default"] = defaultObj;

    QJsonObject AddParameters = AdditionalParameters;
    AddParameters["type"] = "Section";
    obj["AdditionalParameters"] = AddParameters;

    return obj;
}

/**
 * @brief Deserializes the IFF entity from JSON.
 * @param obj JSON object containing IFF data.
 */
void IFF::fromJson(const QJsonObject& obj) {
    if (obj.contains("name"))
        Name = obj["name"].toString().toStdString();

    if (obj.contains("id"))
        ID = obj["id"].toString().toStdString();

    if (obj.contains("parent_id"))
        parentID = obj["parent_id"].toString().toStdString();

    if (obj.contains("active"))
        Active = obj["active"].toBool();

    // Deserialize parameters
    if (obj.contains("parameters")) {
        QJsonObject parObj = obj["parameters"].toObject();
        if (parObj.contains("value")) {
            QJsonObject paramMap = parObj["value"].toObject();
            for (const QString& key : paramMap.keys()) {
                QJsonObject paramObj = paramMap[key].toObject();
                std::shared_ptr<Parameter> param = std::make_shared<Parameter>();
                param->fromJson(paramObj);
                parameters[key.toStdString()] = param;
            }
        }
    }

    if (obj.contains("default") && obj["default"].isObject()) {
        QJsonObject defaultObj = obj["default"].toObject();
        if (defaultObj.contains("transponder"))
            transponder = defaultObj["transponder"].toBool();

        if (defaultObj.contains("emittingRange"))
            emittingRange = valueFromParm(defaultObj["emittingRange"].toObject());

        if (defaultObj.contains("emittingFrequency"))
            emittingFrequency = valueFromParm(defaultObj["emittingFrequency"].toObject());

        if (defaultObj.contains("code"))
            code = valueFromParm(defaultObj["code"].toObject());
        // The booleans use the same direct-section convention as "transponder".
        if (defaultObj.contains("interrogatorEnabled")) interrogatorEnabled = defaultObj["interrogatorEnabled"].toBool();
        if (defaultObj.contains("replyFrequency")) replyFrequency = float(valueFromParm(defaultObj["replyFrequency"].toObject()));
        QString selectedMode;
        if (readOptionValue(defaultObj, "activeIFFMode", selectedMode)) {
            for (const std::string& opt : IFFModeNames)
                if (selectedMode.toStdString() == opt) { activeIFFMode = opt; break; }
        }
        if (defaultObj.contains("enableMode1")) enableMode1 = defaultObj["enableMode1"].toBool();
        if (defaultObj.contains("enableMode2")) enableMode2 = defaultObj["enableMode2"].toBool();
        if (defaultObj.contains("enableMode3A")) enableMode3A = defaultObj["enableMode3A"].toBool();
        if (defaultObj.contains("enableModeC")) enableModeC = defaultObj["enableModeC"].toBool();
        if (defaultObj.contains("enableMode4")) enableMode4 = defaultObj["enableMode4"].toBool();
        if (defaultObj.contains("enableMode5")) enableMode5 = defaultObj["enableMode5"].toBool();
        if (defaultObj.contains("enableModeS")) enableModeS = defaultObj["enableModeS"].toBool();

        QString selectedRole;
        bool validRole = false;
        if (readOptionValue(defaultObj, "IFFRole", selectedRole)) {
            for (int i = 0; i < 3; ++i) {
                if (selectedRole.toStdString() == IFFRoleNames[i]) {
                    role = static_cast<IFFRole>(i);
                    validRole = true;
                    break;
                }
            }
        }
        if (validRole) {
            // Role takes precedence over legacy booleans.
            interrogatorEnabled = role != IFFRole::Responder;
            transponder = role != IFFRole::Interrogator;
        } else {
            // Old JSON files without IFFRole keep working.
            role = interrogatorEnabled && transponder ? IFFRole::Both
                   : interrogatorEnabled ? IFFRole::Interrogator : IFFRole::Responder;
        }
    }

    if (obj.contains("modeConfiguration") && obj["modeConfiguration"].isObject()) {
        const auto cfg = obj["modeConfiguration"].toObject();
        if (cfg.contains("mode1")) modeConfiguration.mode1 = cfg["mode1"].toString().toStdString();
        if (cfg.contains("mode2")) modeConfiguration.mode2 = cfg["mode2"].toString().toStdString();
        if (cfg.contains("mode3A")) modeConfiguration.mode3A = cfg["mode3A"].toString().toStdString();
        if (cfg.contains("modeC")) modeConfiguration.modeC = cfg["modeC"].toString().toStdString();
        if (cfg.contains("mode4")) modeConfiguration.mode4 = cfg["mode4"].toString().toStdString();
        if (cfg.contains("mode5")) modeConfiguration.mode5 = cfg["mode5"].toString().toStdString();
        if (cfg.contains("modeS")) modeConfiguration.modeS = cfg["modeS"].toString().toStdString();
    }

    if(obj.contains("AdditionalParameters")){
        AdditionalParameters = obj["AdditionalParameters"].toObject();
    }
}

/**
 * @brief Performs IFF scan to detect other IFF transponders within range.
 */
void IFF::scan()
{
    // scan() is the existing simulation entry point; no external update calls needed.
    if (!root || !parentEntity || !Active || !interrogatorEnabled ||
        role == IFFRole::Responder ||
        (operationalMode != OperationalMode::Active &&
         operationalMode != OperationalMode::Simulation)) {
        targets.clear();
        iffTargets.clear();
        detects.clear();
        localIffSeen.clear();
        return;
    }

    auto sourceIt = root->Platforms.find(parentEntity->ID);
    if (sourceIt == root->Platforms.end() || !sourceIt->second) return;
    Transform& source = sourceIt->second->transform;

    QSet<Platform*> seenThisScan;
    QJsonArray contactUpdates;
    for (auto& [key, entity] : root->Iffs) {
        if (!entity || entity == this || !entity->Active || !entity->transponder ||
            entity->role == IFFRole::Interrogator ||
            !entity->parentEntity) continue;
        auto targetIt = root->Platforms.find(entity->parentEntity->ID);
        if (targetIt == root->Platforms.end() || !targetIt->second) continue;
        Platform* platform = targetIt->second;
        if (platform == sourceIt->second || seenThisScan.contains(platform)) continue;

        // Preserve the established GPS kilometre-distance convention.
        const float rangeKm = distanceBetween(source.getLatitude(), source.getLongitude(),
                                              platform->transform.getLatitude(), platform->transform.getLongitude()) / 1000.0f;
        if (rangeKm > emittingRange || rangeKm > entity->emittingRange) continue;
        // 1030 MHz interrogation / 1090 MHz reply are separate channels.
        if (std::abs(emittingFrequency - 1030.0f) > 0.01f ||
            std::abs(entity->replyFrequency - 1090.0f) > 0.01f) continue;

        const IFFResponse response = entity->respondToInterrogation(this, rangeKm * 1000.0f);
        if (!response.valid) continue;
        seenThisScan.insert(platform);

        const QVector3D localPos = source.inverseTransformPoint(platform->transform.matrix.translation());
        float angle = std::atan2(localPos.x(), localPos.z()) * RAD2DEG;
        if (angle < 0.0f) angle += 360.0f;

        int index = -1;
        for (int i = 0; i < targets.size(); ++i) {
            if (targets[i].entity == platform) { index = i; break; }
        }
        IFFTarget t = (index >= 0) ? targets[index] : IFFTarget{};
        t.entity = platform;
        t.distance = rangeKm;
        t.radius = rangeKm;
        t.angle = angle;
        t.responderId = response.responderId;
        t.responderName = response.responderName;
        t.mode = response.mode;
        t.code = response.code;
        t.authenticated = response.authenticated;
        t.ally = t.authenticated;
        t.status = t.ally ? 1 : 0; // 0 is unidentified, not proven hostile.
        if (index >= 0) targets[index] = t;
        else targets.append(t);

        // Convert only at the existing UI signal boundary, not during interrogation.
        QJsonObject contact;
        contact["responderId"] = QString::fromStdString(response.responderId);
        contact["responderName"] = QString::fromStdString(response.responderName);
        contact["mode"] = QString::fromStdString(response.mode);
        contact["code"] = QString::fromStdString(response.code);
        contact["authenticated"] = response.authenticated;
        contact["status"] = response.authenticated ? "Friend" : "Unknown";
        contact["distanceMeters"] = response.distanceMeters;
        contact["timestamp"] = QString::fromStdString(response.timestamp);
        contact["distanceKm"] = rangeKm;
        contact["angle"] = angle;
        contactUpdates.append(contact);
    }

    // Drop stale/out-of-range/unresponsive contacts.
    for (int i = targets.size() - 1; i >= 0; --i) {
        if (!seenThisScan.contains(targets[i].entity)) targets.removeAt(i);
    }
    detects.clear();
    localIffSeen.clear();
    for (const IFFTarget& t : targets) {
        detects.insert(t.entity);
        localIffSeen.insert(t.responderId);
    }
    iffTargets = targets; // Keep older consumers in sync.
    emit iffContactsUpdated(contactUpdates); // Empty array clears stale UI contacts.
}
/**
 * @brief Converts OperationalMode enum to string.
 * @param om Operational mode.
 * @return String representation.
 */
QString IFF::operationalModeToString(OperationalMode om) const {
    switch (om) {
    case OperationalMode::Active: return "Active";
    case OperationalMode::Passive: return "Passive";
    case OperationalMode::Off: return "Off";
    case OperationalMode::Simulation: return "Simulation";
    default: return "Off";
    }
}

/**
 * @brief Converts string to OperationalMode enum.
 * @param str String representation.
 * @return Corresponding OperationalMode.
 */
IFF::OperationalMode IFF::stringToOperationalMode(const QString& str) const {
    if (str == "Active") return OperationalMode::Active;
    if (str == "Passive") return OperationalMode::Passive;
    if (str == "Simulation") return OperationalMode::Simulation;
    return OperationalMode::Off;
}

/**
 * @brief Converts CodeSystem enum to string.
 * @param cs Code system.
 * @return String representation.
 */
QString IFF::codeSystemToString(CodeSystem cs) const {
    switch (cs) {
    case CodeSystem::NoPulse: return "NoPulse";
    case CodeSystem::FivePulses: return "FivePulses";
    case CodeSystem::SixPulses: return "SixPulses";
    case CodeSystem::TwelvePulses: return "TwelvePulses";
    default: return "NoPulse";
    }
}

/**
 * @brief Converts string to CodeSystem enum.
 * @param str String representation.
 * @return Corresponding CodeSystem.
 */
IFF::CodeSystem IFF::stringToCodeSystem(const QString& str) const {
    if (str == "FivePulses") return CodeSystem::FivePulses;
    if (str == "SixPulses") return CodeSystem::SixPulses;
    if (str == "TwelvePulses") return CodeSystem::TwelvePulses;
    return CodeSystem::NoPulse;
}

/**
 * @brief Converts EncryptionType enum to string.
 * @param et Encryption type.
 * @return String representation.
 */
QString IFF::encryptionTypeToString(EncryptionType et) const {
    switch (et) {
    case EncryptionType::None: return "None";
    case EncryptionType::NATO: return "NATO";
    case EncryptionType::SecureID: return "SecureID";
    default: return "None";
    }
}

/**
 * @brief Converts string to EncryptionType enum.
 * @param str String representation.
 * @return Corresponding EncryptionType.
 */
IFF::EncryptionType IFF::stringToEncryptionType(const QString& str) const {
    if (str == "NATO") return EncryptionType::NATO;
    if (str == "SecureID") return EncryptionType::SecureID;
    return EncryptionType::None;
}

/**
 * @brief Returns current UTC time as ISO string.
 * @return ISO timestamp.
 */
static std::string nowIsoString() {
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
}

/**
 * @brief Interrogates nearby platforms and processes IFF responses.
 * @param source Transform component of the interrogating platform.
 */
void IFF::interrogateTargets(Transform* source)
{
    Q_UNUSED(source);
    scan(); // Retain the existing API, but scan() owns all updates.
}

/**
 * @brief Produces an allocation-light typed response to a simulated IFF interrogation.
 * @param interrogator Source IFF entity.
 * @param distanceMeters Distance from interrogator to responder in metres.
 * @return A struct with valid=false if no reply is available.
 */
IFF::IFFResponse IFF::respondToInterrogation(IFF* interrogator, float distanceMeters)
{
    IFFResponse result{};
    if (!interrogator || !Active || !transponder ||
        role == IFFRole::Interrogator ||
        (operationalMode != OperationalMode::Active &&
         operationalMode != OperationalMode::Passive &&
         operationalMode != OperationalMode::Simulation)) return result;

    if (distanceMeters < 0.0f || distanceMeters > emittingRange * 1000.0f)
        return result;

    const std::string& mode = interrogator->activeIFFMode;
    const std::string* responseCode = nullptr;
    bool enabled = false;
    bool authenticated = false;

    if (mode == "Mode1") {
        enabled = enableMode1 && interrogator->enableMode1;
        responseCode = &modeConfiguration.mode1;
    } else if (mode == "Mode2") {
        enabled = enableMode2 && interrogator->enableMode2;
        responseCode = &modeConfiguration.mode2;
    } else if (mode == "Mode3A") {
        enabled = enableMode3A && interrogator->enableMode3A;
        responseCode = &modeConfiguration.mode3A;
    } else if (mode == "ModeC") {
        enabled = enableModeC && interrogator->enableModeC;
        responseCode = &modeConfiguration.modeC;
    } else if (mode == "ModeS") {
        enabled = enableModeS && interrogator->enableModeS;
        responseCode = &modeConfiguration.modeS;
    } else if (mode == "Mode4") {
        enabled = enableMode4 && interrogator->enableMode4;
        responseCode = &modeConfiguration.mode4;
        authenticated = enabled && !responseCode->empty() &&
                        *responseCode == interrogator->modeConfiguration.mode4;
    } else if (mode == "Mode5") {
        enabled = enableMode5 && interrogator->enableMode5;
        responseCode = &modeConfiguration.mode5;
        authenticated = enabled && !responseCode->empty() &&
                        *responseCode == interrogator->modeConfiguration.mode5;
    }

    if (!enabled || !responseCode) return result;

    result.valid = true;
    result.interrogatorId = interrogator->ID;
    result.interrogatorName = interrogator->Name;
    result.responderId = ID;
    result.responderName = Name;
    result.mode = mode;
    result.code = *responseCode;
    result.authenticated = authenticated;
    result.distanceMeters = distanceMeters;
    lastInterrogationTime = nowIsoString();
    result.timestamp = lastInterrogationTime;

    Message msg;
    msg.timeStamp = lastInterrogationTime;
    msg.source = interrogator->Name;
    msg.destination = Name;
    msg.content = "IFF response " + mode;
    messages.push_back(msg);
    return result;
}

/**
 * @brief Returns the number of current IFF targets.
 * @return Target count.
 */
int IFF::getIFFTargetCount() const
{
    return targets.size();
}

/**
 * @brief Retrieves IFF target data by index.
 * @param index Target index.
 * @param outResponderId Output responder ID.
 * @param outResponderName Output responder name.
 * @param outMode Output mode.
 * @param outCode Output code.
 * @param outDistance Output distance.
 * @param outAngle Output angle.
 * @param outStatus Output status (1 = friend, 0 = foe).
 * @return True if index valid, false otherwise.
 */
bool IFF::getIFFTarget(
    int index,
    std::string& outResponderId,
    std::string& outResponderName,
    std::string& outMode,
    std::string& outCode,
    float& outDistance,
    float& outAngle,
    int& outStatus
    ) const
{
    if (index < 0 || index >= targets.size())
        return false;

    const IFFTarget& t = targets[index];

    outResponderId   = t.responderId;
    outResponderName = t.responderName;
    outMode          = t.mode;
    outCode          = t.code;
    outDistance      = t.distance;
    outAngle         = t.angle;
    outStatus        = t.status;

    return true;
}

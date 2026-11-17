#include "chaff.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/hierarchy.h"

Chaff::Chaff(Hierarchy* h) : Weapon(h){
    weaponType     = WeaponType::Chaff;
    Active = false;
    if (/*meshRenderer2d && */meshRenderer2d.Sprite) {
        meshRenderer2d.Sprite->clear();
        meshRenderer2d.Sprite->append(":/sea/images/sea/sonobuoy.png");
    }


    if(root){
        auto& data = root->world.AddComponent<ECS::Entity_Data>(ecsID);
        data.active = false;
        data.destroy = false;
    }
}

Chaff::~Chaff(){
    if(root){
        auto* data = root->world.GetComponent<ECS::Entity_Data>(ecsID);
        data->active = false;
        data->destroy = true;
    }
}


void Chaff::SaveState(){
    /*if(transform)*/transform.SaveState();
    // /*if(trajectory)*/trajectory.SaveState();
    // /*if(dynamicModel)*/dynamicModel.SaveState();

    InitalState["active"] = Active;
}
void Chaff::RestoreState(){
    /*if(transform)*/transform.RestoreState();
    // /*if(trajectory)*/trajectory.RestoreState();
    // /*if(dynamicModel)*/dynamicModel.RestoreState();
    fromJson(InitalState);
}

void Chaff::spawn()
{
    emit root->entityAdded(QString::fromStdString(parentID), QString::fromStdString(ID), QString::fromStdString(Name));
    emit root->entityAddedPointer(QString::fromStdString(parentID),this);
}

std::vector<std::string> Chaff::getSupportedComponents()
{
    std::vector<std::string> supported;
    supported.push_back("transform");
    supported.push_back("collider");
    supported.push_back("bitmap");
    return supported;
}

void Chaff::addSupportedcomponent()
{
    for (const auto& item : getSupportedComponents()) {
        addComponent(item.c_str());
        qDebug() << item.c_str(); // Ya std::cout << item << std::endl;
    }
}

void Chaff::addComponent(std::string name)
{
    if (!root)
        return;

    // ============================================================
    // Transform
    // ============================================================

    if (name == "transform")
    {
        if (root->Components.find(transform.ID) !=
            root->Components.end())
        {
            return;
        }

        transform.init(this);

        root->Components.insert({
            transform.ID,
            &transform
        });

        emit root->componentAdded(
            QString::fromStdString(ID),
            QString::fromStdString(transform.ID),
            "transform"
            );
    }
    else if (name == "collider")
    {
        if (root->Components.find(collider.ID) !=
            root->Components.end())
        {
            return;
        }

        collider.init(this);
        collider.parentID = ID;

        root->Components.insert({
            collider.ID,
            &collider
        });

        emit root->componentAdded(
            QString::fromStdString(ID),
            QString::fromStdString(collider.ID),
            "collider"
            );
    }
    else if (name == "bitmap")
    {
        if (root->Components.find(meshRenderer2d.ID) !=
            root->Components.end())
        {
            return;
        }

        meshRenderer2d.init(this);

        root->Components.insert({
            meshRenderer2d.ID,
            &meshRenderer2d
        });

        emit root->componentAdded(
            QString::fromStdString(ID),
            QString::fromStdString(meshRenderer2d.ID),
            "bitmap"
            );

        emit root->entityMeshAdded(
            QString::fromStdString(parentID),
            this
            );
    }
}

void Chaff::removeComponent(std::string name)
{
    if(!root)return;
    if (name == "transform") {
        // if (!transform) return;
        root->Components.erase(transform.ID);
        // delete transform;
        // transform = nullptr;
        emit root->componentRemoved(QString::fromStdString(ID), "transform");
        emit root->entityMeshRemoved(QString::fromStdString(parentID));
        emit root->entityPhysicsRemoved(QString::fromStdString(parentID));
    }
    else if (name == "collider") {
        // if (!collider) return;
        root->Components.erase(collider.ID);
        // delete collider;
        // collider = nullptr;
        emit root->componentRemoved(QString::fromStdString(ID), "collider");
        emit root->entityMeshRemoved(QString::fromStdString(parentID));
        emit root->entityPhysicsRemoved(QString::fromStdString(parentID));
    }
    else if (name == "bitmap") {
        // if (!meshRenderer2d) return;
        root->Components.erase(meshRenderer2d.ID);
        // delete meshRenderer2d;
        // meshRenderer2d = nullptr;
        emit root->componentRemoved(QString::fromStdString(ID), "bitmap");
        emit root->entityMeshRemoved(QString::fromStdString(parentID));
    }
}

QJsonObject Chaff::getComponent(std::string name)
{
    Q_UNUSED(name);
    return QJsonObject();
}

void Chaff::updateComponent(QString name, const QJsonObject& obj)
{
    Q_UNUSED(name);
    Q_UNUSED(obj);
}

QJsonObject Chaff::toJson() const
{
    QJsonObject obj;
    obj["active"] = Active;
    // obj["drop"] = drop;
    obj["name"] = QString::fromStdString(Name);
    obj["id"] = QString::fromStdString(ID);
    obj["weaponType"] = getWeaponTypeObj();
    obj["guidanceType"] = getGuidenceTypeObj();
    obj["detonationType"] = getDetonationTypeObj();


    QJsonObject defaultObj;
    defaultObj["type"] = "Section";

    defaultObj["CloudRadius"] = toParm(CloudRadius, "m", 0, 1000);
    defaultObj["CloudDensity"] = toParm(CloudDensity, "", 0, 1);
    defaultObj["DeploymentTime"] = toParm(DeploymentTime, "s", 0, 3600);
    defaultObj["ExpansionRate"] = toParm(ExpansionRate, "m/s", 0, 100);
    defaultObj["EffectiveLifetime"] = toParm(EffectiveLifetime, "s", 0, 3600);
    defaultObj["RadarSignature"] = toParm(RadarSignature, "", 0, 1);
    defaultObj["DriftVelocity"] = toParm(DriftVelocity, "m/s", 0, 100);
    defaultObj["DispersionFactor"] = toParm(DispersionFactor, "", 0, 10);
    defaultObj["ParticleCount"] = toParm(ParticleCount, "", 0, 100000);

    obj["default"] = defaultObj;

    obj["transform"]    = transform.toJson();
    obj["collider"]     = collider.toJson();
    obj["bitmap"]       = meshRenderer2d.toJson();

    QJsonObject AddParameters = AdditionalParameters;
    AddParameters["type"] = "Section";
    obj["AdditionalParameters"] = AddParameters;

    return obj;

}

void Chaff::fromJson(const QJsonObject& obj)
{
    if (obj.contains("name"))
        Name = obj["name"].toString().toStdString();

    if (obj.contains("id"))
        ID = obj["id"].toString().toStdString();

    if (obj.contains("parent_id"))
        parentID = obj["parent_id"].toString().toStdString();

    if (obj.contains("active"))
        Active = obj["active"].toBool();

    if (obj.contains("weaponType") && obj["weaponType"].isObject()) {
        setWeaponTypeObj(obj["weaponType"].toObject());
    }

    if (obj.contains("guidanceType") && obj["guidanceType"].isObject()) {
        setGuidenceTypeObj(obj["guidanceType"].toObject());
    }

    if (obj.contains("detonationType") && obj["detonationType"].isObject()) {
        setDetonationTypeObj(obj["detonationType"].toObject());
    }

    if (obj.contains("default") && obj["default"].isObject()) {
        QJsonObject defaultObj = obj["default"].toObject();

        if (defaultObj.contains("CloudRadius"))
            CloudRadius = valueFromParm(defaultObj["CloudRadius"].toObject());

        if (defaultObj.contains("CloudDensity"))
            CloudDensity = valueFromParm(defaultObj["CloudDensity"].toObject());

        if (defaultObj.contains("DeploymentTime"))
            DeploymentTime = valueFromParm(defaultObj["DeploymentTime"].toObject());

        if (defaultObj.contains("ExpansionRate"))
            ExpansionRate = valueFromParm(defaultObj["ExpansionRate"].toObject());

        if (defaultObj.contains("EffectiveLifetime"))
            EffectiveLifetime = valueFromParm(defaultObj["EffectiveLifetime"].toObject());

        if (defaultObj.contains("RadarSignature"))
            RadarSignature = valueFromParm(defaultObj["RadarSignature"].toObject());

        if (defaultObj.contains("DriftVelocity"))
            DriftVelocity = valueFromParm(defaultObj["DriftVelocity"].toObject());

        if (defaultObj.contains("DispersionFactor"))
            DispersionFactor = valueFromParm(defaultObj["DispersionFactor"].toObject());

        if (defaultObj.contains("ParticleCount"))
            ParticleCount = static_cast<int>(
                valueFromParm(defaultObj["ParticleCount"].toObject()));
    }
    // 7 auto-components
    if (obj.contains("transform")    /*&& transform*/)     transform.fromJson(obj["transform"].toObject());
    if (obj.contains("collider")     /*&& collider*/)      collider.fromJson(obj["collider"].toObject());
    if (obj.contains("bitmap")       /*&& meshRenderer2d*/) meshRenderer2d.fromJson(obj["bitmap"].toObject());

    if(obj.contains("AdditionalParameters")){
        AdditionalParameters = obj["AdditionalParameters"].toObject();
    }

    // if(collider){
    collider.CollideRadius = 10;
    // }

    if(!statesaved){
        SaveState();
    }
}



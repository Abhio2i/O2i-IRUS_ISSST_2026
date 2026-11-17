#include "bomb.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/hierarchy.h"

Bomb::Bomb(Hierarchy* h) : Weapon(h){
    weaponType     = WeaponType::Bomb;
    Active = false;
    if (/*meshRenderer2d && */meshRenderer2d.Sprite) {
        meshRenderer2d.Sprite->clear();
        meshRenderer2d.Sprite->append(":/Bomb/images/Weapon/bomb.png");
    }


    if(root){
        auto& data = root->world.AddComponent<ECS::Entity_Data>(ecsID);
        data.active = false;
        data.destroy = false;
    }
}

Bomb::~Bomb(){
    if(root){
        auto* data = root->world.GetComponent<ECS::Entity_Data>(ecsID);
        data->active = false;
        data->destroy = true;
    }
}


void Bomb::SaveState(){
    /*if(transform)*/transform.SaveState();
    // /*if(trajectory)*/trajectory.SaveState();
    // /*if(dynamicModel)*/dynamicModel.SaveState();

    InitalState["active"] = Active;
}
void Bomb::RestoreState(){
    /*if(transform)*/transform.RestoreState();
    // /*if(trajectory)*/trajectory.RestoreState();
    // /*if(dynamicModel)*/dynamicModel.RestoreState();
    fromJson(InitalState);
}

void Bomb::spawn()
{
    emit root->entityAdded(QString::fromStdString(parentID), QString::fromStdString(ID), QString::fromStdString(Name));
    emit root->entityAddedPointer(QString::fromStdString(parentID),this);
}

std::vector<std::string> Bomb::getSupportedComponents()
{
    std::vector<std::string> supported;
    supported.push_back("transform");
    supported.push_back("collider");
    supported.push_back("bitmap");
    return supported;
}

void Bomb::addSupportedcomponent()
{
    for (const auto& item : getSupportedComponents()) {
        addComponent(item.c_str());
        qDebug() << item.c_str(); // Ya std::cout << item << std::endl;
    }
}

void Bomb::addComponent(std::string name)
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

void Bomb::removeComponent(std::string name)
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

QJsonObject Bomb::getComponent(std::string name)
{
    Q_UNUSED(name);
    return QJsonObject();
}

void Bomb::updateComponent(QString name, const QJsonObject& obj)
{
    Q_UNUSED(name);
    Q_UNUSED(obj);
}

QJsonObject Bomb::toJson() const
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

    defaultObj["BombMass"] = toParm(BombMass, "kg", 0, 5000);
    defaultObj["ReleaseAltitude"] = toParm(ReleaseAltitude, "m", 0, 30000);
    defaultObj["ReleaseSpeed"] = toParm(ReleaseSpeed, "m/s", 0, 1000);
    defaultObj["DragCoefficient"] = toParm(DragCoefficient, "", 0, 2);
    defaultObj["FallTime"] = toParm(FallTime, "s", 0, 3600);
    defaultObj["MaxFlightTime"] = toParm(MaxFlightTime, "s", 0, 3600);

    defaultObj["GravityEnabled"] = GravityEnabled;
    defaultObj["StabilizationEnabled"] = StabilizationEnabled;
    defaultObj["GuidanceEnabled"] = GuidanceEnabled;

    obj["default"] = defaultObj;

    obj["transform"]    = transform.toJson();
    obj["collider"]     = collider.toJson();
    obj["bitmap"]       = meshRenderer2d.toJson();

    QJsonObject AddParameters = AdditionalParameters;
    AddParameters["type"] = "Section";
    obj["AdditionalParameters"] = AddParameters;

    return obj;

}

void Bomb::fromJson(const QJsonObject& obj)
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

        if (defaultObj.contains("BombMass"))
            BombMass = valueFromParm(defaultObj["BombMass"].toObject());

        if (defaultObj.contains("ReleaseAltitude"))
            ReleaseAltitude = valueFromParm(defaultObj["ReleaseAltitude"].toObject());

        if (defaultObj.contains("ReleaseSpeed"))
            ReleaseSpeed = valueFromParm(defaultObj["ReleaseSpeed"].toObject());

        if (defaultObj.contains("DragCoefficient"))
            DragCoefficient = valueFromParm(defaultObj["DragCoefficient"].toObject());

        if (defaultObj.contains("FallTime"))
            FallTime = valueFromParm(defaultObj["FallTime"].toObject());

        if (defaultObj.contains("MaxFlightTime"))
            MaxFlightTime = valueFromParm(defaultObj["MaxFlightTime"].toObject());

        if (defaultObj.contains("GravityEnabled"))
            GravityEnabled = defaultObj["GravityEnabled"].toBool();

        if (defaultObj.contains("StabilizationEnabled"))
            StabilizationEnabled = defaultObj["StabilizationEnabled"].toBool();

        if (defaultObj.contains("GuidanceEnabled"))
            GuidanceEnabled = defaultObj["GuidanceEnabled"].toBool();
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



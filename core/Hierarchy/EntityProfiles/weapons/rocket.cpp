#include "rocket.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/hierarchy.h"

Rocket::Rocket(Hierarchy* h) : Weapon(h){
    weaponType     = WeaponType::Rocket;
    Active = false;
    if (/*meshRenderer2d && */meshRenderer2d.Sprite) {
        meshRenderer2d.Sprite->clear();
        meshRenderer2d.Sprite->append(":/rocket/images/Weapon/rocket.png");
    }


    if(root){
        auto& data = root->world.AddComponent<ECS::Entity_Data>(ecsID);
        data.active = false;
        data.destroy = false;
    }
}

Rocket::~Rocket(){
    if(root){
        auto* data = root->world.GetComponent<ECS::Entity_Data>(ecsID);
        data->active = false;
        data->destroy = true;
    }
}


void Rocket::SaveState(){
    /*if(transform)*/transform.SaveState();
    // /*if(trajectory)*/trajectory.SaveState();
    // /*if(dynamicModel)*/dynamicModel.SaveState();

    InitalState["active"] = Active;
}
void Rocket::RestoreState(){
    /*if(transform)*/transform.RestoreState();
    // /*if(trajectory)*/trajectory.RestoreState();
    // /*if(dynamicModel)*/dynamicModel.RestoreState();
    fromJson(InitalState);
}

void Rocket::spawn()
{
    emit root->entityAdded(QString::fromStdString(parentID), QString::fromStdString(ID), QString::fromStdString(Name));
    emit root->entityAddedPointer(QString::fromStdString(parentID),this);
}

std::vector<std::string> Rocket::getSupportedComponents()
{
    std::vector<std::string> supported;
    supported.push_back("transform");
    supported.push_back("collider");
    supported.push_back("bitmap");
    return supported;
}

void Rocket::addSupportedcomponent()
{
    for (const auto& item : getSupportedComponents()) {
        addComponent(item.c_str());
        qDebug() << item.c_str(); // Ya std::cout << item << std::endl;
    }
}

void Rocket::addComponent(std::string name)
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

void Rocket::removeComponent(std::string name)
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

QJsonObject Rocket::getComponent(std::string name)
{
    Q_UNUSED(name);
    return QJsonObject();
}

void Rocket::updateComponent(QString name, const QJsonObject& obj)
{
    Q_UNUSED(name);
    Q_UNUSED(obj);
}

QJsonObject Rocket::toJson() const
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

    defaultObj["LaunchSpeed"] = toParm(LaunchSpeed, "m/s", 0, 2000);
    defaultObj["CurrentSpeed"] = toParm(CurrentSpeed, "m/s", 0, 2000);
    defaultObj["Acceleration"] = toParm(Acceleration, "m/s²", 0, 500);
    defaultObj["BurnDuration"] = toParm(BurnDuration, "s", 0, 600);
    defaultObj["Mass"] = toParm(Mass, "kg", 0, 5000);
    defaultObj["DragCoefficient"] = toParm(DragCoefficient, "", 0, 2);
    defaultObj["LaunchElevation"] = toParm(LaunchElevation, "deg", -90, 90);
    defaultObj["LaunchAzimuth"] = toParm(LaunchAzimuth, "deg", 0, 360);
    defaultObj["FlightTime"] = toParm(FlightTime, "s", 0, 3600);

    defaultObj["GravityEnabled"] = GravityEnabled;
    defaultObj["PropulsionEnabled"] = PropulsionEnabled;

    obj["default"] = defaultObj;

    obj["transform"]    = transform.toJson();
    obj["collider"]     = collider.toJson();
    obj["bitmap"]       = meshRenderer2d.toJson();

    QJsonObject AddParameters = AdditionalParameters;
    AddParameters["type"] = "Section";
    obj["AdditionalParameters"] = AddParameters;

    return obj;

}

void Rocket::fromJson(const QJsonObject& obj)
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

        if (defaultObj.contains("LaunchSpeed"))
            LaunchSpeed = valueFromParm(defaultObj["LaunchSpeed"].toObject());

        if (defaultObj.contains("CurrentSpeed"))
            CurrentSpeed = valueFromParm(defaultObj["CurrentSpeed"].toObject());

        if (defaultObj.contains("Acceleration"))
            Acceleration = valueFromParm(defaultObj["Acceleration"].toObject());

        if (defaultObj.contains("BurnDuration"))
            BurnDuration = valueFromParm(defaultObj["BurnDuration"].toObject());

        if (defaultObj.contains("Mass"))
            Mass = valueFromParm(defaultObj["Mass"].toObject());

        if (defaultObj.contains("DragCoefficient"))
            DragCoefficient = valueFromParm(defaultObj["DragCoefficient"].toObject());

        if (defaultObj.contains("LaunchElevation"))
            LaunchElevation = valueFromParm(defaultObj["LaunchElevation"].toObject());

        if (defaultObj.contains("LaunchAzimuth"))
            LaunchAzimuth = valueFromParm(defaultObj["LaunchAzimuth"].toObject());

        if (defaultObj.contains("FlightTime"))
            FlightTime = valueFromParm(defaultObj["FlightTime"].toObject());

        if (defaultObj.contains("GravityEnabled"))
            GravityEnabled = defaultObj["GravityEnabled"].toBool();

        if (defaultObj.contains("PropulsionEnabled"))
            PropulsionEnabled = defaultObj["PropulsionEnabled"].toBool();
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


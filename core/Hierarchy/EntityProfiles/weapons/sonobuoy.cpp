#include "sonobuoy.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "core/Hierarchy/hierarchy.h"

const float RAD2DEG = 180.0f / M_PI;

Sonobuoy::Sonobuoy(Hierarchy* h) : Weapon(h){
    weaponType     = WeaponType::Sonobuoy;
    Active = false;
    if (/*meshRenderer2d && */meshRenderer2d.Sprite) {
        meshRenderer2d.Sprite->clear();
        meshRenderer2d.Sprite->append(":/sea/images/sea/sonobuoy.png");
    }

    // if (collider) {
        collider.CollideRadius = 10000;
    // }

    if(root){
        auto& data = root->world.AddComponent<ECS::Entity_Data>(ecsID);
        data.active = false;
        data.destroy = false;
    }
}

Sonobuoy::~Sonobuoy(){
    if(root){
        auto* data = root->world.GetComponent<ECS::Entity_Data>(ecsID);
        data->active = false;
        data->destroy = true;
    }
}

void Sonobuoy::SaveState(){
    /*if(transform)*/transform.SaveState();
    // /*if(trajectory)*/trajectory.SaveState();
    // /*if(dynamicModel)*/dynamicModel.SaveState();

    InitalState["active"] = Active;
    InitalState["drop"] = drop;
}
void Sonobuoy::RestoreState(){
    /*if(transform)*/transform.RestoreState();
    // /*if(trajectory)*/trajectory.RestoreState();
    // /*if(dynamicModel)*/dynamicModel.RestoreState();
    fromJson(InitalState);
}

void Sonobuoy::spawn()
{
    emit root->entityAdded(QString::fromStdString(parentID), QString::fromStdString(ID), QString::fromStdString(Name));
    emit root->entityAddedPointer(QString::fromStdString(parentID),this);
}

std::vector<std::string> Sonobuoy::getSupportedComponents()
{
    std::vector<std::string> supported;
    supported.push_back("transform");
    supported.push_back("collider");
    supported.push_back("bitmap");
    return supported;
}

void Sonobuoy::addSupportedcomponent()
{
    for (const auto& item : getSupportedComponents()) {
        addComponent(item.c_str());
        qDebug() << item.c_str(); // Ya std::cout << item << std::endl;
    }
}

void Sonobuoy::addComponent(std::string name)
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

void Sonobuoy::removeComponent(std::string name)
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

QJsonObject Sonobuoy::getComponent(std::string name)
{
    Q_UNUSED(name);
    return QJsonObject();
}

void Sonobuoy::updateComponent(QString name, const QJsonObject& obj)
{
    Q_UNUSED(name);
    Q_UNUSED(obj);
}

void Sonobuoy::dropped(){
    drop = true;
    Active = true;
    if(root){
        auto* data = root->world.GetComponent<ECS::Entity_Data>(ecsID);
        data->active = true;
    }
    // starttime = Simulation::simulationTime;
    if(parentEntity /*&& transform*/){
        Transform& source = root->Platforms[parentEntity->ID]->transform;
        transform.setTranslation(source.translation());
        transform.setAltitude(-Depth*3.281f);
    }

}

void Sonobuoy::Update(){
    if(!drop) return;
    if(!Active)return;
    // time =  -starttime + Simulation::simulationTime;
    if((time/3600.f) > Life ){
        detects.clear();
        detection.clear();
        collider.blinkRadius = 0;
        // Active = false;
        return;
    }
    // qDebug() << "[Sensor::ewscan] called for ID:" << QString::fromStdString(id)
    if(!parentEntity /*|| !transform || !collider*/) return;
    Transform* source = &transform;
    if(!source) return;
    if(/*collider &&*/ !detection.empty()){
        if(collider.blinkRadius>20){
            collider.blinkRadius = 20;
        }else{
            collider.blinkRadius = 40;
        }
    }else{
        collider.blinkRadius = 0;

    }
    detects.clear();
    detection.clear();
    // C# foreach (Transform tr in targets) -> C++ range-based for loop
    for (auto& [key, entity] : root->Platforms)
    {
        if(!entity || !entity->Active) continue;
        if (entity->category == Entity::Category::Marine) {
            Platform* platform = entity;
            // qDebug() << "[Sensor::ewscan] iterating entity:" << QString::fromStdString(key);
            if(platform->ID == parentEntity->ID || !platform /*|| !platform->transform*/) continue;
            QVector3D localPos = source->inverseTransformPoint(platform->transform.matrix.translation());
            //float distance = localPos.length();
            float metredis = distanceBetween(source->getLatitude(),source->getLongitude(),platform->transform.getLatitude(),platform->transform.getLongitude())/1000;

            // horizontal angle (Y axis) : x vs z
            float yAngle = std::atan2(localPos.x(), localPos.z()) * RAD2DEG;

            // qDebug()<<yAngle<<","<<detectCheck(localPos,metredis)<<","<<(fre1 < frequency && fre2> frequency);
            if (entity->Active && metredis<range)  // .position() is assumed
            {
                //qDebug()<< "detect";
                if (detects.count(platform) == 0)
                {
                    detects.insert(platform);
                    SonobuoyOutput target;
                    target.entity = platform;
                    target.bearing = yAngle;
                    target.radius = metredis;
                    detection.append(target);
                }else{
                    for (int i = 0; i < detection.size(); ++i) {
                        if (detection.at(i).entity == platform) {
                            detection[i].bearing = yAngle;
                            detection[i].radius = metredis;
                            break;
                        }
                    }
                }
            }
            else
            {
                if (detects.count(platform) > 0)
                {
                    for (int i = 0; i < detection.size(); ++i) {
                        if (detection.at(i).entity == platform) {
                            detection.removeAt(i);
                            break;
                        }
                    }
                    detects.erase(platform);
                }
            }
        }
    }


}

QJsonObject Sonobuoy::toJson() const
{
    QJsonObject obj;
    obj["active"] = Active;
    obj["drop"] = drop;
    obj["name"] = QString::fromStdString(Name);
    obj["id"] = QString::fromStdString(ID);

    obj["weaponType"] = getWeaponTypeObj();


    QJsonObject defaultObj;
    defaultObj["type"] = "Section";
    defaultObj["range"] = toParm(range,"km",0, 120);
    // defaultObj["TransmissionRange"] = toParm(TransmissionRange,"km", 0.1,  500);
    defaultObj["Depth"] = toParm(Depth,"m", 10, 2000);
    defaultObj["Life"] = toParm(Life,"h", 1, 100);
    obj["default"] = defaultObj;

    obj["transform"]    = transform.toJson();
    obj["collider"]     = collider.toJson();
    obj["bitmap"]       = meshRenderer2d.toJson();
    QJsonObject AddParameters = AdditionalParameters;
    AddParameters["type"] = "Section";
    obj["AdditionalParameters"] = AddParameters;

    return obj;

}

void Sonobuoy::fromJson(const QJsonObject& obj)
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


    if (obj.contains("drop"))
        drop = obj["drop"].toBool();

    if (obj.contains("default") && obj["default"].isObject()) {
        QJsonObject defaultObj = obj["default"].toObject();

        if (defaultObj.contains("range"))
            range = valueFromParm(defaultObj["range"].toObject());

        if (defaultObj.contains("TransmissionRange"))
            TransmissionRange = valueFromParm(defaultObj["TransmissionRange"].toObject());

        if (defaultObj.contains("Depth"))
            Depth = valueFromParm(defaultObj["Depth"].toObject());

        if (defaultObj.contains("Life"))
            Life = valueFromParm(defaultObj["Life"].toObject());

    }
    // 7 auto-components
    if (obj.contains("transform")    /*&& transform*/)     transform.fromJson(obj["transform"].toObject());
    // if (obj.contains("rigidbody")    /*&& rigidbody*/)     rigidbody.fromJson(obj["rigidbody"].toObject());
    if (obj.contains("collider")     /*&& collider*/)      collider.fromJson(obj["collider"].toObject());
    // if (obj.contains("trajectory")   /*&& trajectory*/)    trajectory.fromJson(obj["trajectory"].toObject());
    if (obj.contains("bitmap")       /*&& meshRenderer2d*/) meshRenderer2d.fromJson(obj["bitmap"].toObject());
    // if (obj.contains("dynamicModel") /*&& dynamicModel*/)  dynamicModel.fromJson(obj["dynamicModel"].toObject());
    // if (obj.contains("crossSection") /*&& crossSection*/)  crossSection.fromJson(obj["crossSection"].toObject());

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


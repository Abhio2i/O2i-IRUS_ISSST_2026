#include <QCoreApplication>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <sharedMemory.h> // Aapki library
#include <QDebug>
#include <QObject>
int main(int argc, char *argv[])
{
    /*
     * lat,rad
     * lon,rad
     * v_x,ft/s
     * v_y,ft/s
     * v_z,ft/s
     * pitch,rad
     * roll,rad
     * yaw,rad
     * true_heading,rad
     * true_airspeed,ft/s
     * driftangle,rad/s
     * roll_rate,rad/s
     * pitch_rate,rad/s
     * yaw_rate,rad/s
     * alt,m
     * vel_north,ft/s
     * vel_east,ft/s
     * vel_vert,ft/s
     * ground_speed,nauts
     * timestamp
     * */

    QCoreApplication a(argc, argv);

    //port 33336
    // 1. Shared Memory Setup
    SharedMemory shm;
    shm.create();
    EntityHandler* handler = shm.entityHandler;
    SharedStructs::Entity* entity = handler->getEntityByIndex(0);

    while(true) {
        qDebug() << (entity->transformData.geoCords.getLatitude());//deg
        qDebug() << (entity->transformData.geoCords.getLongitude());//deg
        qDebug() << (entity->dynamicsData.getEastVelocity());//m/s
        qDebug() << (entity->dynamicsData.getVerticalVelocity());//m/s
        qDebug() << (entity->dynamicsData.getNorthVelocity());//m/s

        qDebug() << (entity->dynamicsData.getPitch());//deg
        qDebug() << (entity->dynamicsData.getRoll());//deg
        qDebug() << (entity->dynamicsData.getYaw());//deg

        qDebug() << (entity->dynamicsData.getTrueHeading());//deg
        qDebug() << (entity->dynamicsData.getTrueAirSpeed());//km/h

        qDebug() << (entity->dynamicsData.getDriftAngle());//deg

        qDebug() << (entity->dynamicsData.getRollRate());//deg/s

        qDebug() << (entity->dynamicsData.getPitchRate());//deg/s

        qDebug() << (entity->dynamicsData.getYawRate());//deg/s

        qDebug() << (entity->dynamicsData.getMaxAltitude());//ft

        qDebug() << (entity->dynamicsData.getNorthVelocity());//m/s
        qDebug() << (entity->dynamicsData.getEastVelocity());//m/s
        qDebug() << (entity->dynamicsData.getVGroundVelocity());//km/h

        if(entity->aisCount>0){
            // qDebug() << entity->ais[0].aisID;
            // qDebug() << entity->ais[0].mmsi;
            // qDebug() << &entity->ais[0].nav_status;
            // qDebug() << entity->ais[0].rot;
            // qDebug() << entity->ais[0].sog;
            // qDebug() << entity->ais[0].positionAccuracy;
            // qDebug() << entity->ais[0].pos.latitude;
            // qDebug() << entity->ais[0].pos.latitude;
            // qDebug() << entity->ais[0].cog;
            // qDebug() << entity->ais[0].heading;
            // qDebug() << entity->ais[0].heading;
            // qDebug() << entity->ais[0].Maneuver_Indicator;
            // qDebug() << entity->ais[0].radio_status;
        }

        if(entity->adsbCount>0){
            // qDebug() << entity->adsb[0].lat;
            // qDebug() << entity->adsb[0].lon;
            // qDebug() << entity->adsb[0].altitude;
            // qDebug() << entity->adsb[0].call_sign;
            // qDebug() << entity->adsb[0].emitter_category;
            // qDebug() << entity->adsb[0].ground_speed;
            // qDebug() << entity->adsb[0].track;
            // qDebug() << entity->adsb[0].vx;
            // qDebug() << entity->adsb[0].vy;
            // qDebug() << entity->adsb[0].vz;
            // qDebug() << entity->adsb[0].ax;
            // qDebug() << entity->adsb[0].ay;
            // qDebug() << entity->adsb[0].az;

        }

        if(entity->aesaContactCount>0){
            // qDebug() << entity->aesaContact[0].lat_deg;
            // qDebug() << entity->aesaContact[0].lon_deg;
            // qDebug() << entity->aesaContact[0].altitude_m;
            // qDebug() << entity->aesaContact[0].call_sign;
            // qDebug() << entity->aesaContact[0].emitter_category;
            // qDebug() << entity->aesaContact[0].ground_speed;
            // qDebug() << entity->aesaContact[0].track;
            // qDebug() << entity->aesaContact[0].vx;
            // qDebug() << entity->aesaContact[0].vy;
            // qDebug() << entity->aesaContact[0].vz;
            // qDebug() << entity->aesaContact[0].ax;
            // qDebug() << entity->aesaContact[0].ay;
            // qDebug() << entity->aesaContact[0].az;

        }

    }


    QTcpServer * server = new QTcpServer();
    QObject::connect(server, &QTcpServer::newConnection, [server,entity](){

        QTcpSocket *clientSocket = server->nextPendingConnection();
        qDebug() << "new clinet is conncted from :: "<<clientSocket->peerAddress().toString();
        while(true) {
            QByteArray data;
            data.append(QByteArray::number(entity->transformData.geoCords.getLatitude()));//deg
            data.append(QByteArray::number(entity->transformData.geoCords.getLongitude()));//deg
            data.append(QByteArray::number(entity->dynamicsData.getEastVelocity()));//m/s
            data.append(QByteArray::number(entity->dynamicsData.getVerticalVelocity()));//m/s
            data.append(QByteArray::number(entity->dynamicsData.getNorthVelocity()));//m/s

            data.append(QByteArray::number(entity->dynamicsData.getPitch()));//deg
            data.append(QByteArray::number(entity->dynamicsData.getRoll()));//deg
            data.append(QByteArray::number(entity->dynamicsData.getYaw()));//deg

            data.append(QByteArray::number(entity->dynamicsData.getTrueHeading()));//deg
            data.append(QByteArray::number(entity->dynamicsData.getTrueAirSpeed()));//km/h

            data.append(QByteArray::number(entity->dynamicsData.getDriftAngle()));//deg

            data.append(QByteArray::number(entity->dynamicsData.getRollRate()));//deg/s

            data.append(QByteArray::number(entity->dynamicsData.getPitchRate()));//deg/s

            data.append(QByteArray::number(entity->dynamicsData.getYawRate()));//deg/s

            data.append(QByteArray::number(entity->dynamicsData.getMaxAltitude()));//ft

            data.append(QByteArray::number(entity->dynamicsData.getNorthVelocity()));//m/s
            data.append(QByteArray::number(entity->dynamicsData.getEastVelocity()));//m/s
            data.append(QByteArray::number(entity->dynamicsData.getVGroundVelocity()));//km/h

            clientSocket->write(data);
            clientSocket->flush();
        }



    });
    if(!server->listen(QHostAddress::Any,1234)){
        qDebug()<<" Server is not started: "<<server->errorString();

    }else{
        qDebug()<<"server started at 1234";
    }

    return a.exec();
}




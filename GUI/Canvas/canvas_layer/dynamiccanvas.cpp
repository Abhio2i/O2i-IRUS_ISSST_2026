#include "dynamiccanvas.h"
#include "core/Hierarchy/ECS/Struct/Meta_Data_Component.h"
#include "core/Hierarchy/ECS/Struct/collider_component.h"
#include "core/Hierarchy/ECS/Struct/radio_component.h"
#include "core/Hierarchy/ECS/Struct/trajectory_component.h"
#include "core/Hierarchy/ECS/Struct/dynamic_component.h"
#include "core/Hierarchy/ECS/Struct/transform_component.h"
#include "core/Hierarchy/ECS/Struct/sensor_component.h"
#include "core/Hierarchy/ECS/Struct/mesh_component.h"
#include "GUI/Canvas/context_menu/entity_image_size_store.h"
#include "core/Hierarchy/Utils/entityutils.h"
#include "map_widget.h"
#include <GIS_API/backend/api/map_api.h>
#include <cmath>

DynamicCanvas::DynamicCanvas(MapWidget *map, QWidget *parent):
    OverlayCanvas(map, parent)
{
    m_pixmap.load(":/texture/images/Texture/fighterjet.png");
    m_pixmap = m_pixmap.scaled(
        resoulation,
        resoulation,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
}

QPixmap appendPixmapsHorizontally(const QPixmap &oldPixmap, const QPixmap &newPixmap) {
    int oldWidth = oldPixmap.width();
    int newWidth = newPixmap.width();
    int height = std::max(oldPixmap.height(), newPixmap.height());
    QPixmap combinedCanvas(oldWidth + newWidth, height);
    combinedCanvas.fill(Qt::transparent);
    QPainter painter(&combinedCanvas);
    painter.drawPixmap(0, 0, oldPixmap);
    painter.drawPixmap(oldWidth, 0, newPixmap);
    painter.end();
    return combinedCanvas;
}

void DynamicCanvas::setEntitiesVisible(bool visible)
{
    if (m_showEntities == visible)
        return;
    m_showEntities = visible;
    update();
    if (MapWidget* map = getMap())
        map->update();
}

//fix by yogesh
void DynamicCanvas::drawContent(QPainter& painter)
{
    if (!getMap() || !getWorld())
        return;
    ECS::World* world = getWorld();
    MapWidget* map = getMap();
    const auto& en_datas = world->GetComponentArray<ECS::Entity_Data>();
    const auto& geos = world->GetComponentArray<ECS::GeocordComponent>();
    const QSet<ECS::Entity> selectedSet = m_editor ? m_editor->selectedEntities() : QSet<ECS::Entity>();
    if(m_showEntities && (m_showTrajectories || !selectedSet.isEmpty())){
        const auto& trajArray = world->GetComponentArray<ECS::trajectoryPathComponent>();
        const auto& activeEntities = trajArray->GetActiveEntities();
        const auto& trajectorys = trajArray->GetComponents();
        batchPath.clear();
        for (int i = 0; i < trajectorys.size(); i++) {
            ECS::Entity e = activeEntities[i];
            if (!m_showTrajectories && !selectedSet.contains(e)) continue;
            const auto& geodata = geos->GetData(e);
            const auto& traj = trajectorys[i];
            const auto& data = en_datas->GetData(e);
            if (traj.activeCount < 1 || !data->active || data->destroy|| !traj.active) continue;
            if(traj.currentWaypoint == 0){
                QPointF centerPoint = map->geoToCanvas(geodata->lat, geodata->lon);
                float centerX = centerPoint.x();
                float centerY = centerPoint.y();
                QPointF centerPoint2 = map->geoToCanvas(traj.path[0].lat, traj.path[0].lon);
                float centerX2 = centerPoint2.x();
                float centerY2 = centerPoint2.y();
                batchPath.moveTo(centerX, centerY);
                batchPath.lineTo(centerX2,centerY2);
                batchPath.closeSubpath();
            }
        }
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter.drawPath(batchPath);
    }
    if(m_showSensors)

    {//DRAW sensor
        batchPath.clear();
        const auto& sensorArray = world->GetComponentArray<ECS::sensorComponent>();
        const auto& activeEntities = sensorArray->GetActiveEntities();
        const auto& sensors = sensorArray->GetComponents();
        for (int i = 0; i < sensors.size(); i++) {

            ECS::Entity e = activeEntities[i];
            const auto& geodata = geos->GetData(e);
            const auto& data = en_datas->GetData(e);

            if (sensors[i].activeCount == 0 ||  sensors[i].activeCount > 9 || !data->active || data->destroy) continue;
            int max = sensors[i].activeCount;
            for(int sen = 0;sen<max;sen++){
                int activeIdx = sen;
                float range = sensors[i].sensor[activeIdx].range;
                if(range<2)continue;
                QPointF centerPoint = map->geoToCanvas(geodata->lat, geodata->lon);
                float centerX = centerPoint.x();
                float centerY = centerPoint.y();
                float platformHeading = geodata->heading;
                float minAz = sensors[i].sensor[activeIdx].minAzimuth;
                float maxAz = sensors[i].sensor[activeIdx].maxAzimuth;
                float worldLeftEdge  = platformHeading + minAz;
                float worldRightEdge = platformHeading + maxAz;
                float sectorSpan     = maxAz - minAz;
                auto [latAtRadius, lonAtRadius] = calculateNewLatLong(
                    geodata->lat, geodata->lon,
                    worldLeftEdge,
                    range
                    );
                QPointF radiusPoint = map->geoToCanvas(latAtRadius, lonAtRadius);
                float dx = radiusPoint.x() - centerX;
                float dy = radiusPoint.y() - centerY;
                float pixelRadius = std::sqrt(dx * dx + dy * dy);
                float startAngle = 90.0f - worldRightEdge;
                float sweepAngle = sectorSpan;
                QRectF boundingRect(
                    centerX - pixelRadius,
                    centerY - pixelRadius,
                    pixelRadius * 2,
                    pixelRadius * 2
                    );
                batchPath.moveTo(centerX, centerY);
                batchPath.arcTo(boundingRect, startAngle, sweepAngle);
                batchPath.closeSubpath();
            }
        }
        painter.setPen(QPen(Qt::black, 1));
        painter.drawPath(batchPath);
    }

    if(m_showRadios || !selectedSet.isEmpty())
    {
        batchPath.clear();
        const auto& radioArray = world->GetComponentArray<ECS::radioComponent>();
        const auto& activeEntities = radioArray->GetActiveEntities();
        const auto& radios = radioArray->GetComponents();
        for (size_t i = 0; i < radios.size(); ++i)
        {
            ECS::Entity e = activeEntities[i];
            if (!m_showRadios && !selectedSet.contains(e))
                continue;
            const auto& geodata = geos->GetData(e);
            const auto& data = en_datas->GetData(e);
            if (!data->active || data->destroy) {
                continue;
            }
            QPointF centerPoint = map->geoToCanvas(geodata->lat, geodata->lon);
            for (size_t r = 0; r < 90; ++r)
            {
                int targetId = radios[i].radios[r];
                if (targetId < 0)
                    continue;
                const auto& geodataT = geos->GetData(targetId);
                const auto& dataT = en_datas->GetData(targetId);
                if (dataT && dataT->active)
                {
                    QPointF lastPoint = map->geoToCanvas(geodataT->lat, geodataT->lon);
                    batchPath.moveTo(centerPoint);
                    batchPath.lineTo(lastPoint);
                }
            }
        }

        painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
        painter.drawPath(batchPath);
    }
        if(m_showCollider || !selectedSet.isEmpty())

    {//DRAW sensor
        batchPath.clear();
        const auto& colliderArray = world->GetComponentArray<ECS::ColliderWarningComponent>();
        const auto& activeEntities = colliderArray->GetActiveEntities();
        const auto& colliders = colliderArray->GetComponents();
        for (int i = 0; i < colliders.size(); i++) {
            ECS::Entity e = activeEntities[i];
            if (!m_showCollider && !selectedSet.contains(e)) continue;
            const auto& geodata = geos->GetData(e);
            const auto& data = en_datas->GetData(e);
            const auto& coll = colliders[i];
            if (coll.CollideRadius < 5 || !data->active || data->destroy) continue;
            QPointF centerPoint = map->geoToCanvas(geodata->lat, geodata->lon);
            float centerX = centerPoint.x();
            float centerY = centerPoint.y();
            auto [newLat, newLon] = calculateNewLatLong(geodata->lat, geodata->lon, 90,coll.CollideRadius/1000);
            QPointF points = map->geoToCanvas(newLat, newLon);
            float distance = QVector2D(centerPoint).distanceToPoint(QVector2D(points));
            batchPath.addEllipse(
                QPointF(centerX, centerY),
                distance,
                distance
                );
        }
        painter.setPen(QPen(QColor(0, 55, 0, 100), 2));
        painter.setBrush(QBrush(QColor(0, 255, 0, 40)));
        painter.drawPath(batchPath);
        painter.setBrush(Qt::NoBrush);
    }

    if(m_showEntities){
        const auto& spriteArray = world->GetComponentArray<ECS::SpritePath>();
        const auto& activeEntities = spriteArray->GetActiveEntities();
        const auto& sprites = spriteArray->GetComponents();
        int size = sprites.size();
        int pthSize = 0;
        int pth = 0;
        int frgs = 0;
        int i = 0;
        for (i = 0; i < size; i++) {
            ECS::Entity e = activeEntities[i];
            const auto& geodata = geos->GetData(e);
            const auto& data = en_datas->GetData(e);
            const auto& spr = sprites[i];

            // if (!data->active || data->destroy) continue;
            // if (!data->active || data->destroy) continue;
            QPointF screen = map->geoToCanvas(geodata->lat, geodata->lon);

            // added for entity zoom scale
            const float zoomF = currentZoomFactor();
            const float finalScale = spr.scale * zoomF;

            const char* targetPath = spr.path;
            pthSize = m_paths.size();
            for(pth = 0;pth<pthSize;pth++){
                if(strcmp(m_paths[pth].path, targetPath) == 0){
                    break;
                }
            }

            if (pth==pthSize) {
                m_paths.emplace_back(spr);
                QPixmap pxmap(spr.path);
                if(pxmap.isNull()){
                    pxmap.load(":/texture/images/Texture/fighterjet.png");
                }
                m_pixmap = appendPixmapsHorizontally(m_pixmap, pxmap.scaled(
                                                                   resoulation,
                                                                   resoulation,
                                                                   Qt::IgnoreAspectRatio,
                                                                   Qt::SmoothTransformation
                                                                   ));
                QPainter::PixmapFragment fragment = QPainter::PixmapFragment::create(
                    screen,                                      // Screen Pos (Center)
                    QRectF((m_paths.size()*resoulation), 0, resoulation, resoulation),// Texture Atlas Source Rect
                    finalScale, finalScale,                      // Scale X, Y  (was spr.scale)
                    geodata->heading,                            // Rotation
                    1.0                                          // Opacity
                    );
                m_fragments.push_back(fragment);
            }else{
                frgs = m_fragments.size();
                if(frgs==i){
                    QPainter::PixmapFragment fragment = QPainter::PixmapFragment::create(
                        screen,                                      // Screen Pos (Center)
                        QRectF((pth+1)*resoulation, 0, resoulation, resoulation),// Texture Atlas Source Rect
                        finalScale, finalScale,                      // Scale X, Y  (was spr.scale)
                        geodata->heading,                            // Rotation
                        1.0                                          // Opacity
                        );
                    m_fragments.push_back(fragment);
                }
            }
            m_fragments[i].x = screen.x();
            m_fragments[i].sourceLeft = (pth+1)*resoulation;
            m_fragments[i].y = screen.y();
            m_fragments[i].scaleX = finalScale;   // was spr.scale
            m_fragments[i].scaleY = finalScale;   // was spr.scale
            m_fragments[i].rotation = geodata->heading;
            m_fragments[i].opacity = data->active?1.0:0.0;
        }
        painter.drawPixmapFragments(m_fragments.data(),size,m_pixmap);
        // painter.drawPixmap(QPointF(0,0), m_pixmap);
    }

    // if(m_showEntities)
    // {

    //     const auto& entityArray = world->GetComponentArray<ECS::DynamicTarget>();
    //     const auto& activeEntities = entityArray->GetActiveEntities();
    //     const auto& enity = entityArray->GetComponents();
    //     for (int i = 0; i < enity.size(); i++) {
    //         ECS::Entity e = activeEntities[i];
    //         // const auto& geodata = geos->GetData(e);
    //         const auto& data = en_datas->GetData(e);
    //         const auto& coll = enity[i];
    //         if (!data->active || data->destroy) continue;
    //         GeoPos geo = flatXYZToGeo(coll.pos.x(), coll.pos.y()*KMtoFT, coll.pos.z());
    //         QPointF centerPoint = map->geoToCanvas(geo.lat, geo.lon);
    //         float centerX = centerPoint.x();
    //         float centerY = centerPoint.y();
    //         float distance = 5;
    //         batchPath.addEllipse(
    //             QPointF(centerX, centerY),
    //             distance,
    //             distance
    //             );
    //     }
    //     painter.setPen(QPen(QColor(0, 55, 0, 100), 2));
    //     painter.setBrush(QBrush(QColor(255, 255, 0, 255)));
    //     painter.drawPath(batchPath);
    //     painter.setBrush(Qt::NoBrush);
    // }
}

//fix by yogesh
void DynamicCanvas::resetAtlas()
{
    m_paths.clear();
    m_fragments.clear();
    m_pixmap.load(":/texture/images/Texture/fighterjet.png");
    m_pixmap = m_pixmap.scaled(
        resoulation, resoulation,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);
}

bool DynamicCanvas::handleMousePress(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton/* && m_isDrawingPoint*/) {
        // addPointAt(event->pos());
        // m_isDrawingPoint = false;
        setCursor(Qt::ArrowCursor);
        return true;
    }
    return false;
}

bool DynamicCanvas::handleMouseMove(QMouseEvent* event)
{
    Q_UNUSED(event)
    return false;
}

bool DynamicCanvas::handleMouseRelease(QMouseEvent* event)
{
    Q_UNUSED(event)
    return false;
}
void DynamicCanvas::setImageScale(int px)
{
    if (px < 8)   px = 8;
    if (px > 500) px = 500;

    EntityImageSizeStore::instance().clear();

    resoulation = px;

    m_paths.clear();
    m_fragments.clear();
    m_pixmap = QPixmap();
    m_pixmap.load(":/texture/images/Texture/fighterjet.png");
    m_pixmap = m_pixmap.scaled(
        resoulation,
        resoulation,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
    if (!m_pixmap.isNull()) {
        m_pixmap = m_pixmap.scaled(
            resoulation, resoulation,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
    }

    update();

    if (MapWidget* map = getMap()) {
        map->update();
        const auto children = map->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for (QWidget* child : children)
            child->update();
    }
}

void DynamicCanvas::setSensorsVisible(bool visible)
{
    if (m_showSensors == visible)
        return;
    m_showSensors = visible;
    update();
    if (MapWidget* map = getMap())
        map->update();
}

void DynamicCanvas::setRadiosVisible(bool visible)
{
    if (m_showRadios == visible)
        return;
    m_showRadios = visible;
    update();
    if (MapWidget* map = getMap())
        map->update();
}

void DynamicCanvas::setColliderVisible(bool visible)
{
    if (m_showCollider == visible)
        return;
    m_showCollider = visible;
    update();
    if (MapWidget* map = getMap())
        map->update();
}
void DynamicCanvas::setTrajectoriesVisible(bool visible)
{
    if (m_showTrajectories == visible)
        return;
    m_showTrajectories = visible;
    update();
    if (MapWidget* map = getMap())
        map->update();
}
void DynamicCanvas::setEntityZoomScaleEnabled(bool enabled)
{
    if (m_entityZoomScale == enabled)
        return;
    m_entityZoomScale = enabled;
    update();
    if (MapWidget* map = getMap())
        map->update();
}

float DynamicCanvas::currentZoomFactor() const
{
    if (!m_entityZoomScale)
        return 1.0f;

    const int z = MapApi::instance()->getZoom();
    const float refZoom = 10.0f;

    float factor = std::pow(2.0f, static_cast<float>(z) - refZoom);
    return qBound(0.25f, factor, 8.0f);
}

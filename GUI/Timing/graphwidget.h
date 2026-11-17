/* =============================================================================
 * FILE:         graphwidget.h
 * MODULE:       Timeline Graph Widget
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 * STANDARD:     RTCA DO-178C / ED-12C, DAL B
 * COVERAGE:     Branch / Decision Coverage required (100% true/false paths)
 *
 * DESCRIPTION:  Declares the GraphWidget class which provides a timeline graph
 *               widget for visualising entity activity over time. It displays
 *               horizontal bars representing each entity's active period,
 *               supports zooming via mouse wheel, and updates a status table
 *               with real‑time entity state information. Integrates with
 *               Hierarchy to access entity data.
 *
 * REQUIREMENTS: REQ-GRAPH-010  Timeline graph for entity activity
 *               REQ-GRAPH-011  Display horizontal bars per entity from start
 *                              to end time
 *               REQ-GRAPH-012  X‑axis with time ticks (HH:MM:SS format)
 *               REQ-GRAPH-013  Y‑axis with entity labels
 *               REQ-GRAPH-014  Zoom in/out with mouse wheel
 *               REQ-GRAPH-015  Real‑time refresh with current simulation time
 *               REQ-GRAPH-016  Status table showing current entity state
 *               REQ-GRAPH-017  Integration with Hierarchy for entity data
 *
 * AUTHOR:       Arti Rajpoot
 * REVIEWED BY:  [Reviewer Name], [Review Date] — SPR-GRAPH-001
 *
 *
 * COPYRIGHT:    Oxygen 2 Innovation (O2I). All rights reserved.
 *               Restricted circulation — defence simulation use only.
 * =============================================================================
 */

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "core/Hierarchy/hierarchy.h"
#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QString>


struct GraphData {
    int entityNum;
    double startTime;
    double endTime;
    QColor color;
};


struct TimelineEntry {
    QString name;
    double  startTime = 0.0;
    double  endTime   = -1.0;
    bool    deleted   = false;
    bool    hasStarted = false;
};


class GraphCanvas;

class GraphWidget : public QWidget
{
    Q_OBJECT
public:

    explicit GraphWidget(QWidget *parent = nullptr);


    int entity = 10;
    float t = 0;
    int time = 10;
    int zoom = 100;
    QVector<GraphData> graphDataList;


    bool simulationPaused = false;


    void setHierarchy(Hierarchy* hier);


    void drawGraph(QWidget* canvas);


    void handleWheel(QWheelEvent *event);


    static QString formatTime(double seconds);

public slots:

    void refresh(float delta);

private:

    Hierarchy* h = nullptr;
    QTableWidget* tableWidget;
    QWidget* graphCanvas;


    QMap<QString, TimelineEntry> history;


    void updateTable();


    void updateHistory();


    void drawXTick(QPainter &p, int startX, int endX, int y, int canvasWidth);


    void drawYTick(QPainter &p, int startY, int endY, int x, int canvasHeight,
                   QVector<QString> entityNames);


    void drawData(QPainter &p, int startX, int bottomY, int canvasWidth, int canvasHeight);

    QVector<Platform*> getSortedPlatforms();


    QVector<TimelineEntry> getSortedHistory() const;
};

#endif // GRAPHWIDGET_H

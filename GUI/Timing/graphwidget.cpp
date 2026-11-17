/* =============================================================================
 * FILE:         graphwidget.cpp
 * MODULE:       Timeline Graph Widget
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 * STANDARD:     RTCA DO-178C / ED-12C, DAL B
 * COVERAGE:     Branch / Decision Coverage required (100% true/false paths)
 *
 * DESCRIPTION:  Implements the GraphWidget class which provides a timeline graph
 *               widget for visualising entity activity over time. It displays
 *               horizontal bars representing each entity's active period,
 *               supports zooming via mouse wheel, and updates a status table
 *               with real‑time entity state information. Integrates with
 *               Hierarchy to access entity data.
 *
 * REQUIREMENTS: Implements REQ-GRAPH-010 through REQ-GRAPH-017
 *
 * AUTHOR:       Arti Rajpoot
 * REVIEWED BY:  [Reviewer Name], [Review Date] — SPR-GRAPH-001
 *
 * COPYRIGHT:    Oxygen 2 Innovation (O2I). All rights reserved.
 *               Restricted circulation — defence simulation use only.
 * =============================================================================
 */

#include "graphwidget.h"
#include "qpainter.h"
#include <QtMath>
#include <QWheelEvent>
#include <QHeaderView>
#include <QTime>
#include <algorithm>
#include <cctype>
#include <core/Simulation/simulation.h>


// Helper Functions
/* Convert seconds to HH:MM:SS format */
QString GraphWidget::formatTime(double seconds) {
    if (seconds < 0)
        return QStringLiteral("-");
    int totalSeconds = static_cast<int>(seconds);
    int hours   = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs    = totalSeconds % 60;
    return QString("%1:%2:%3")
        .arg(hours,   2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs,    2, 10, QChar('0'));
}

// Natural Sort Helpers

static int extractTrailingNumber(const std::string& s) {
    int i = static_cast<int>(s.size()) - 1;
    while (i >= 0 && std::isdigit(static_cast<unsigned char>(s[i]))) i--;
    std::string numStr = s.substr(i + 1);
    if (numStr.empty()) return -1;
    return std::stoi(numStr);
}

static bool naturalLessName(const QString& a, const QString& b) {
    const std::string na = a.toStdString();
    const std::string nb = b.toStdString();

    size_t i = 0;
    while (i < na.size() && i < nb.size() && na[i] == nb[i]) i++;

    if (i < na.size() && i < nb.size() &&
        std::isdigit(static_cast<unsigned char>(na[i])) &&
        std::isdigit(static_cast<unsigned char>(nb[i]))) {
        int numA = extractTrailingNumber(na);
        int numB = extractTrailingNumber(nb);
        if (numA != numB) return numA < numB;
    }
    return na < nb;
}

static bool naturalLess(Platform* a, Platform* b) {
    return naturalLessName(QString::fromStdString(a->Name),
                           QString::fromStdString(b->Name));
}

// Sorted Platforms Helper

QVector<Platform*> GraphWidget::getSortedPlatforms() {
    QVector<Platform*> sorted;
    if (!h) return sorted;
    for (auto& [key, platform] : h->Platforms) {
        if (platform) sorted.append(platform);
    }
    std::sort(sorted.begin(), sorted.end(), naturalLess);
    return sorted;
}

QVector<TimelineEntry> GraphWidget::getSortedHistory() const {
    QVector<TimelineEntry> list;
    for (const auto& e : history)
        list.append(e);
    std::sort(list.begin(), list.end(),
              [](const TimelineEntry& a, const TimelineEntry& b) {
                  return naturalLessName(a.name, b.name);
              });
    return list;
}

//  Custom Canvas Class

class GraphCanvas : public QWidget {
public:
    GraphWidget* parent;
    GraphCanvas(GraphWidget* p) : QWidget(p), parent(p) {
        setStyleSheet("background-color: black;");
    }

protected:

    void paintEvent(QPaintEvent *) override {
        parent->drawGraph(this);
    }

    void wheelEvent(QWheelEvent *event) override {
        parent->handleWheel(event);
    }
};

// Constructor
/* Initialize graph widget with default settings */
GraphWidget::GraphWidget(QWidget *parent): QWidget(parent) {
    setWindowFlags(Qt::Window
                   | Qt::WindowTitleHint
                   | Qt::WindowSystemMenuHint
                   | Qt::WindowMinMaxButtonsHint
                   | Qt::WindowCloseButtonHint
                   | Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: black;");

    time   = 0;
    zoom   = 100;
    entity = 0;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"Entity Name", "Start Time", "End Time",
                                            "Total Duration", "Status"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableWidget->setStyleSheet(
        "QTableWidget {"
        "   background-color: #1a1a1a;"
        "   color: white;"
        "   gridline-color: #333333;"
        "   border: 1px solid #444444;"
        "}"
        "QHeaderView::section {"
        "   background-color: #2a2a2a;"
        "   color: white;"
        "   padding: 5px;"
        "   border: 1px solid #444444;"
        "   font-weight: bold;"
        "}"
        "QTableWidget::item {"
        "   padding: 5px;"
        "}"
        );

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    // No maximum height — table and graph share space ~50/50
    tableWidget->setMinimumHeight(120);

    graphCanvas = new GraphCanvas(this);
    mainLayout->addWidget(tableWidget, 1);   // stretch 1
    mainLayout->addWidget(graphCanvas, 1);   // stretch 1 → half / half
    setLayout(mainLayout);

    graphDataList.append({0, 1.5, 4.0, Qt::blue});
    graphDataList.append({1, 0.5, 3.2, Qt::green});
    graphDataList.append({2, 5.0, 8.5, Qt::red});
    update();
}



void GraphWidget::setHierarchy(Hierarchy* hier){
    h = hier;
}

void GraphWidget::updateHistory()
{
    if (!h) return;

    // Collect names currently present
    QSet<QString> currentNames;
    for (auto& [key, platform] : h->Platforms) {
        if (!platform) continue;
        QString name = QString::fromStdString(platform->Name);
        currentNames.insert(name);

        TimelineEntry& e = history[name];
        e.name      = name;
        e.startTime = platform->dynamicModel.startTime;
        e.deleted   = false;

        // If the model already has a concrete endTime, respect it
        double modelEnd = platform->dynamicModel.endTime;
        if (modelEnd >= 0.0) {
            e.endTime = modelEnd;
        }
        // else leave e.endTime as-is (may still be -1 while running)

        if (t >= e.startTime)
            e.hasStarted = true;
    }

    // Any history entry no longer present → mark Deleted and freeze end time
    for (auto it = history.begin(); it != history.end(); ++it) {
        if (!currentNames.contains(it.key())) {
            TimelineEntry& e = it.value();
            if (!e.deleted) {
                e.deleted = true;
                if (e.endTime < 0.0)
                    e.endTime = t;          // freeze at deletion moment
            }
        }
    }
}



void GraphWidget::refresh(float delta)
{
    if (!h) return;

    // Prefer simulation clock; fallback accumulate
    if (Simulation::simulationTime > 0.0f)
        t = static_cast<double>(Simulation::simulationTime);
    else
        t += static_cast<double>(delta);

    updateHistory();

    // Determine axis range from history (includes deleted entities)
    double maxT = t;
    for (const auto& e : history) {
        if (e.startTime > maxT) maxT = e.startTime;
        double et = (e.endTime < 0.0) ? t : e.endTime;
        if (et > maxT) maxT = et;
    }
    time = static_cast<int>(std::ceil(maxT));
    if (time < 1) time = 1;

    entity = history.size();

    updateTable();
    if (graphCanvas) graphCanvas->update();
}

void GraphWidget::updateTable(){
    if (!h) return;
    tableWidget->setRowCount(0);

    QVector<TimelineEntry> sorted = getSortedHistory();
    for (const TimelineEntry& e : sorted) {
        double startTime = e.startTime;
        double endTime   = e.endTime;          // may still be -1
        bool   isPending = (t < startTime);
        bool   isRunning = (!e.deleted && !isPending && endTime < 0.0);
        bool   isFinished= (endTime >= 0.0);

        // Display values
        QString endStr;
        QString durStr;
        QString status;

        if (e.deleted) {
            status = "Deleted";
            endStr = formatTime(endTime);
            durStr = formatTime(qMax(0.0, endTime - startTime));
        }
        else if (isPending) {
            status = "Pending";
            endStr = QStringLiteral("-");
            durStr = QStringLiteral("-");
        }
        else if (isRunning) {
            // Still active → show live end = current time
            if (simulationPaused)
                status = "Stop";
            else
                status = "Running";
            endStr = formatTime(t);
            durStr = formatTime(qMax(0.0, t - startTime));
        }
        else { // finished but still present in hierarchy
            status = "Completed";
            endStr = formatTime(endTime);
            durStr = formatTime(qMax(0.0, endTime - startTime));
        }

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        tableWidget->setItem(row, 0, new QTableWidgetItem(e.name));
        tableWidget->setItem(row, 1, new QTableWidgetItem(formatTime(startTime)));
        tableWidget->setItem(row, 2, new QTableWidgetItem(endStr));
        tableWidget->setItem(row, 3, new QTableWidgetItem(durStr));

        QTableWidgetItem* statusItem = new QTableWidgetItem(status);
        if (status == "Running") {
            statusItem->setForeground(Qt::green);
        } else if (status == "Completed") {
            statusItem->setForeground(Qt::cyan);
        } else if (status == "Deleted") {
            statusItem->setForeground(QColor(255, 100, 100));   // soft red
        } else if (status == "Stop") {
            statusItem->setForeground(QColor(255, 165, 0));     // orange
        } else { // Pending
            statusItem->setForeground(Qt::yellow);
        }
        tableWidget->setItem(row, 4, statusItem);
    }
}


void GraphWidget::handleWheel(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        zoom += 10;
    } else {
        if (zoom > 20) zoom -= 10;
    }
    if (!h) return;
    graphCanvas->update();
}


void GraphWidget::drawGraph(QWidget* canvas) {
    if (!h) return;

    QPainter p(canvas);
    p.setRenderHint(QPainter::Antialiasing);

    int w = canvas->width();
    int hgt = canvas->height();

    QVector<TimelineEntry> sorted = getSortedHistory();
    entity = sorted.size();

    QFontMetrics fm(p.font());
    int maxNameWidth = 50;
    for (const auto& e : sorted) {
        int nameWidth = fm.horizontalAdvance(e.name) + 15;
        if (nameWidth > maxNameWidth) maxNameWidth = nameWidth;
    }

    int leftPadding  = maxNameWidth;
    int rightPadding = 50;
    int topPadding   = 30;
    int botPadding   = 40;

    p.setPen(QPen(Qt::white, 2));

    int startX  = leftPadding;
    int endX    = w - rightPadding;
    int bottomY = hgt - botPadding;
    int topY    = topPadding;

    // Draw X-axis
    p.drawLine(startX, bottomY, endX, bottomY);
    drawXTick(p, startX, endX, bottomY, w);

    QVector<QString> entityNames;
    for (const auto& e : sorted)
        entityNames.append(e.name);

    // Draw Y-axis
    p.drawLine(startX, bottomY, startX, topY);
    drawYTick(p, bottomY, topY, startX, hgt, entityNames);

    // Set clipping area
    QRect chartArea(startX, topY, endX - startX, bottomY - topY);
    p.setClipRect(chartArea);

    drawData(p, startX, bottomY, w, hgt);

    p.setClipping(false);
}

/* Draw entity timeline data — naturally sorted order, includes deleted */
void GraphWidget::drawData(QPainter &p, int startX, int bottomY,
                           int canvasWidth, int canvasHeight)
{
    if (history.isEmpty()) return;

    const int axisTime = (time > 0) ? time : 1;

    int availableWidth  = canvasWidth  - 100;
    int availableHeight = canvasHeight - 100;
    if (availableWidth <= 0 || availableHeight <= 0) return;

    double xSpacing = (availableWidth  / (double)axisTime) * (zoom / 100.0);
    double ySpacing = (entity > 0)
                          ? (availableHeight / (double)entity) * (zoom / 100.0)
                          : 0.0;

    QVector<TimelineEntry> sorted = getSortedHistory();
    int entityNum = 0;

    for (const TimelineEntry& e : sorted) {
        double startTime = e.startTime;
        double endTime   = e.endTime;
        if (endTime < 0.0) endTime = t;          // still open → current time
        if (endTime < startTime) endTime = startTime;

        int x1 = startX + static_cast<int>(startTime * xSpacing);
        int x2 = startX + static_cast<int>(endTime   * xSpacing);
        int y  = bottomY - static_cast<int>(entityNum * ySpacing);

        int barHeight = 8;
        int dotRadius = 4;
        int barWidth  = x2 - x1;
        if (barWidth < 2) barWidth = 2;

        bool pending   = (t < startTime);
        bool completed = (e.endTime >= 0.0) || e.deleted;

        if (pending) {
            // Only a start marker
            p.setBrush(QColor(120, 120, 0));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPoint(x1, y), dotRadius, dotRadius);
        } else {
            // Bar colour: deleted → muted red-ish, completed → teal, running → cyan
            if (e.deleted)
                p.setBrush(QColor(180, 80, 80));
            else if (completed)
                p.setBrush(QColor(0, 180, 180));
            else
                p.setBrush(Qt::cyan);

            p.setPen(Qt::NoPen);
            p.drawRect(x1, y - barHeight / 2, barWidth, barHeight);

            p.setBrush(Qt::yellow);
            p.setPen(QPen(Qt::white, 1));
            p.drawEllipse(QPoint(x1, y), dotRadius, dotRadius);

            // End marker only when the entity has a fixed end
            if (completed) {
                p.setBrush(Qt::red);
                p.drawEllipse(QPoint(x2, y), dotRadius, dotRadius);
            }
        }

        entityNum++;
    }
}

/* Draw X-axis ticks and time labels */
void GraphWidget::drawXTick(QPainter &p, int startX, int endX, int y, int canvasWidth)
{
    Q_UNUSED(canvasWidth);

    int availableWidth = endX - startX;
    if (availableWidth <= 0 || time <= 0)
        return;

    double tickSpacing = (availableWidth / static_cast<double>(time)) * (zoom / 100.0);

    QFontMetrics fm(p.font());
    QString sampleTime = formatTime(time);
    int textWidth = fm.horizontalAdvance(sampleTime) + 12;

    int step = 1;
    if (tickSpacing < 1e-6) {
        step = qMax(1, time);
    } else {
        int minStep = static_cast<int>(qCeil(textWidth / tickSpacing));
        step = qMax(1, minStep);
    }

    static const int kNice[] = {1, 5, 10, 15, 30, 60, 120, 300, 600, 900, 1800, 3600};
    for (int nice : kNice) {
        if (nice >= step) {
            step = nice;
            break;
        }
    }
    if (step > time)
        step = qMax(1, time);

    p.setPen(QPen(Qt::white, 1));

    for (int i = 0; i <= time; i += step) {
        int x = startX + static_cast<int>(i * tickSpacing);
        if (x > endX)
            break;

        p.drawLine(x, y - 5, x, y + 5);

        QString timeLabel = formatTime(i);
        QRect textRect(x - 40, y + 8, 80, 20);
        p.drawText(textRect, Qt::AlignCenter, timeLabel);
    }

    // Always show 0 and max on axis ends
    p.drawText(QRect(startX - 20, y + 8, 40, 20), Qt::AlignCenter, formatTime(0));
    p.drawText(QRect(endX - 40, y + 8, 80, 20), Qt::AlignCenter, formatTime(time));
}

/* Draw Y-axis ticks and entity labels — ALWAYS show every entity name */
void GraphWidget::drawYTick(QPainter &p, int startY, int endY, int x,
                            int canvasHeight, QVector<QString> entityNames) {
    Q_UNUSED(canvasHeight);
    int availableHeight = startY - endY;
    int numEntities     = entity;
    if (numEntities == 0) return;

    double spacing = (availableHeight / (double)numEntities) * (zoom / 100.0);

    // Always draw every label (user request). If they overlap the user can zoom.
    for (int i = 0; i < numEntities; ++i) {
        int y = startY - static_cast<int>(i * spacing);
        if (y < endY) break;

        p.drawLine(x - 5, y, x + 5, y);

        QString label = (i < entityNames.size())
                            ? entityNames[i]
                            : "Entity" + QString::number(i);

        QRect textRect(0, y - 10, x - 8, 20);
        p.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
}

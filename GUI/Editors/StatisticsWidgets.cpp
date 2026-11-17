#include "StatisticsWidgets.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QFrame>
#include <QtMath>
#include <algorithm>

namespace {
QTableWidget *makeTable(const QStringList &names) {
    auto *t = new QTableWidget(names.size(), 2);
    t->setHorizontalHeaderLabels({"Parameter", "Count"});
    t->verticalHeader()->hide();
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionMode(QAbstractItemView::NoSelection);
    t->setFocusPolicy(Qt::NoFocus);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    for (int i = 0; i < names.size(); ++i) {
        t->setItem(i, 0, new QTableWidgetItem(names[i]));
        auto *v = new QTableWidgetItem("0");
        v->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        t->setItem(i, 1, v);
    }
    return t;
}
void setValues(QTableWidget *table, const QVector<int> &values) {
    for (int i = 0; i < values.size() && i < table->rowCount(); ++i)
        table->item(i, 1)->setText(QString::number(values[i]));
}
class DonutChart : public QWidget {
public:
    explicit DonutChart(const EntityDistribution *data, QWidget *parent = nullptr)
        : QWidget(parent), data_(data) { setMinimumSize(185, 170); }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QVector<int> values = {data_->air, data_->ground, data_->sea,
                                     data_->staticEntities, data_->other};
        const QVector<QColor> colors = {QColor("#3b82f6"), QColor("#22c55e"),
            QColor("#f59e0b"), QColor("#a855f7"), QColor("#94a3b8")};
        int total = 0;
        for (int v : values) total += qMax(0, v);
        const int side = qMin(width(), height()) - 20;
        QRectF r((width()-side)/2.0, (height()-side)/2.0, side, side);
        p.setPen(Qt::NoPen);
        if (total == 0) {
            p.setBrush(QColor("#e5e7eb")); p.drawEllipse(r);
        } else {
            int start = 90 * 16;
            int used = 0;
            for (int i = 0; i < values.size(); ++i) {
                if (values[i] <= 0) continue;
                int span = (i == values.size()-1) ? 360*16-used
                    : qRound(360.0*16*values[i]/total);
                // Skip a zero-value final slice without affecting preceding slices.
                p.setBrush(colors[i]); p.drawPie(r, start, -span);
                start -= span; used += span;
            }
        }
        QRectF hole = r.adjusted(side*.27, side*.27, -side*.27, -side*.27);
        p.setBrush(palette().color(QPalette::Base)); p.drawEllipse(hole);
        p.setPen(palette().color(QPalette::Text));
        QFont f = p.font(); f.setBold(true); f.setPointSize(13); p.setFont(f);
        p.drawText(hole, Qt::AlignCenter, QString::number(total));
    }
private:
    const EntityDistribution *data_;
};
}

EntityDistributionWidget::EntityDistributionWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    table_ = makeTable({"Air", "Ground", "Sea", "Static", "Other"});
    chart_ = new DonutChart(&data_, this);
    layout->addWidget(table_, 3);
    layout->addWidget(chart_, 2);
}
void EntityDistributionWidget::setData(const EntityDistribution &d) {
    data_ = d;
    setValues(table_, {d.air, d.ground, d.sea, d.staticEntities, d.other});
    chart_->update();
}
OperationalStatusWidget::OperationalStatusWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    table_ = makeTable({"Moving", "Stationary", "Airborne", "Submerged", "Damaged", "Healthy"});
    layout->addWidget(table_);
}
void OperationalStatusWidget::setData(const OperationalStatus &d) {
    setValues(table_, {d.moving, d.stationary, d.airborne, d.submerged, d.damaged, d.healthy});
}
SensorStatisticsWidget::SensorStatisticsWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    table_ = makeTable({"Total Sensors", "Active Sensors", "Radar", "Sonar", "Active Tracks", "Total Detections"});
    layout->addWidget(table_);
}
void SensorStatisticsWidget::setData(const SensorStatistics &d) {
    setValues(table_, {d.total, d.active, d.radar, d.sonar, d.activeTracks, d.totalDetections});
}
StatisticsChartWidget::StatisticsChartWidget(const QString &title, const QString &yAxis, QWidget *parent)
    : QWidget(parent), title_(title), yAxis_(yAxis) {
    setMinimumHeight(210);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void StatisticsChartWidget::setSeries(const QVector<ChartSeries> &series) {
    series_ = series;
    update();
}
void StatisticsChartWidget::clear() { series_.clear(); update(); }
void StatisticsChartWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), palette().color(QPalette::Base));
    const QRectF plot(58, 38, qMax(10, width()-76), qMax(10, height()-78));
    p.setPen(QColor("#344054"));
    QFont bold = p.font(); bold.setBold(true); p.setFont(bold);
    p.drawText(QRectF(12, 5, width()-24, 25), Qt::AlignLeft | Qt::AlignVCenter, title_);
    p.setFont(QFont());
    double minX=0, maxX=1, maxY=1;
    for (const auto &s : series_)
        for (const auto &pt : s.points) {
            maxX = qMax(maxX, pt.x);
            maxY = qMax(maxY, pt.y);
        }
    // The time axis is elapsed simulation time (seconds).
    p.setPen(QColor("#e2e8f0"));
    for (int i=0; i<=4; ++i) {
        double y=plot.bottom()-plot.height()*i/4.0;
        p.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
        p.setPen(QColor("#475467"));
        p.drawText(QRectF(0,y-9,50,18), Qt::AlignRight | Qt::AlignVCenter,
                   QString::number(maxY*i/4.0, 'f', maxY < 10 ? 1 : 0));
        p.setPen(QColor("#e2e8f0"));
    }
    for (int i=0; i<=4; ++i) {
        double x=plot.left()+plot.width()*i/4.0;
        p.drawLine(QPointF(x,plot.top()),QPointF(x,plot.bottom()));
        p.setPen(QColor("#475467"));
        p.drawText(QRectF(x-28,plot.bottom()+3,56,17),Qt::AlignCenter,
                   QString::number(maxX*i/4.0,'f',0));
        p.setPen(QColor("#e2e8f0"));
    }
    p.setPen(QColor("#475467"));
    p.drawText(QRectF(plot.left(),height()-23,plot.width(),20),Qt::AlignCenter,"Simulation time (s)");
    p.save();
    p.translate(13, plot.center().y()); p.rotate(-90);
    p.drawText(QRectF(-plot.height()/2,-11,plot.height(),20),Qt::AlignCenter,yAxis_);
    p.restore();
    p.save(); p.setClipRect(plot);
    for (const auto &s : series_) {
        QPainterPath path;
        bool first=true;
        for (const auto &pt : s.points) {
            QPointF pos(plot.left()+plot.width()*pt.x/maxX,
                        plot.bottom()-plot.height()*pt.y/maxY);
            if (first) { path.moveTo(pos); first=false; } else path.lineTo(pos);
        }
        p.setPen(QPen(s.color, 2)); p.drawPath(path);
    }
    p.restore();
    // Legend in upper right; series labels are always visible.
    int x = width()-12;
    for (int i=series_.size()-1; i>=0; --i) {
        const auto &s=series_[i];
        int w=p.fontMetrics().horizontalAdvance(s.name)+24;
        x-=w;
        p.setPen(QPen(s.color,3)); p.drawLine(x+2,20,x+14,20);
        p.setPen(QColor("#344054")); p.drawText(x+18,24,s.name);
        x-=8;
    }
}

#pragma once
#include <QWidget>
#include <QVector>
#include <QColor>
#include "RuntimeStatisticsData.h"

class QTableWidget;
class QLabel;

class EntityDistributionWidget : public QWidget {
    Q_OBJECT
public:
    explicit EntityDistributionWidget(QWidget *parent = nullptr);
    void setData(const EntityDistribution &data);
private:
    QTableWidget *table_;
    QWidget *chart_;
    EntityDistribution data_;
};

class OperationalStatusWidget : public QWidget {
    Q_OBJECT
public:
    explicit OperationalStatusWidget(QWidget *parent = nullptr);
    void setData(const OperationalStatus &data);
private:
    QTableWidget *table_;
};

class SensorStatisticsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SensorStatisticsWidget(QWidget *parent = nullptr);
    void setData(const SensorStatistics &data);
private:
    QTableWidget *table_;
};

struct ChartPoint { double x = 0.0, y = 0.0; };
struct ChartSeries {
    QString name;
    QColor color;
    QVector<ChartPoint> points;
};

class StatisticsChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsChartWidget(const QString &title, const QString &yAxis, QWidget *parent = nullptr);
    void setSeries(const QVector<ChartSeries> &series);
    void clear();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QString title_, yAxis_;
    QVector<ChartSeries> series_;
};

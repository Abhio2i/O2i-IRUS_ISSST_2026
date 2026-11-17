#pragma once
#include <QDialog>
#include <QVector>
#include "RuntimeStatisticsData.h"
#include "StatisticsWidgets.h"

class QLabel;
class QPushButton;

class RuntimeStatisticsDialog : public QDialog {
    Q_OBJECT
public:
    explicit RuntimeStatisticsDialog(QWidget *parent = nullptr);
    const RuntimeStatisticsData &currentData() const { return data_; }
public slots:
    void updateStatistics(const RuntimeStatisticsData &data);
    void resetHistory();
    void exportCsv();
private:
    void refreshCharts();
    QLabel *scenario_, *status_, *time_, *rate_;
    QLabel *total_, *active_, *destroyed_, *inactive_;
    EntityDistributionWidget *distribution_;
    OperationalStatusWidget *operational_;
    SensorStatisticsWidget *sensors_;
    StatisticsChartWidget *countChart_, *tickChart_;
    QPushButton *pauseButton_;
    RuntimeStatisticsData data_;
    QVector<ChartPoint> totalHistory_, activeHistory_, destroyedHistory_, tickHistory_;
    bool monitoringPaused_ = false;
    static constexpr int MaxHistoryPoints = 600;
};

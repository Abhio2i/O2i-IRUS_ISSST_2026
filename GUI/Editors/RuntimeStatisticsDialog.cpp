#include "RuntimeStatisticsDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMetaType>
#include <QFrame>
#include <QtMath>

namespace {
QLabel *valueLabel(const QString &initial = "0") {
    auto *label = new QLabel(initial);
    QFont font=label->font(); font.setPointSize(16); font.setBold(true);
    label->setFont(font);
    return label;
}
QWidget *summaryCard(const QString &title, QLabel *value) {
    auto *frame = new QFrame;
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setObjectName("summaryCard");
    auto *layout=new QVBoxLayout(frame);
    auto *caption=new QLabel(title);
    layout->addWidget(caption);
    layout->addWidget(value);
    return frame;
}
QGroupBox *group(const QString &title, QWidget *content) {
    auto *box=new QGroupBox(title);
    auto *layout=new QVBoxLayout(box);
    layout->setContentsMargins(6,10,6,6);
    layout->addWidget(content);
    return box;
}
void appendLimited(QVector<ChartPoint> &history, ChartPoint point, int maxPoints) {
    history.append(point);
    if (history.size()>maxPoints) history.remove(0,history.size()-maxPoints);
}
QString csvQuote(const QString &value) {
    QString escaped=value;
    escaped.replace('"', "\"\"");
    return '"'+escaped+'"';
}
}
RuntimeStatisticsDialog::RuntimeStatisticsDialog(QWidget *parent) : QDialog(parent) {
    qRegisterMetaType<RuntimeStatisticsData>("RuntimeStatisticsData");
    setWindowTitle("Runtime Statistics");
    resize(1150,760);
    setMinimumSize(840,600);
    auto *root=new QVBoxLayout(this);
    auto *scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    root->addWidget(scroll,1);
    auto *body=new QWidget;
    scroll->setWidget(body);
    auto *main=new QVBoxLayout(body);
    main->setSpacing(10);

    auto *meta=new QHBoxLayout;
    scenario_=new QLabel("-"); status_=new QLabel("Stopped");
    time_=new QLabel("00:00:00.000"); rate_=new QLabel("1.0x");
    meta->addWidget(new QLabel("Scenario:")); meta->addWidget(scenario_,2);
    meta->addWidget(new QLabel("Status:")); meta->addWidget(status_,1);
    meta->addWidget(new QLabel("Simulation Time:")); meta->addWidget(time_,1);
    meta->addWidget(new QLabel("Sim Rate:")); meta->addWidget(rate_);
    main->addLayout(meta);

    auto *cards=new QHBoxLayout;
    total_=valueLabel(); active_=valueLabel(); destroyed_=valueLabel(); inactive_=valueLabel();
    cards->addWidget(summaryCard("Total Entities",total_));
    cards->addWidget(summaryCard("Active Entities",active_));
    cards->addWidget(summaryCard("Destroyed Entities",destroyed_));
    cards->addWidget(summaryCard("Inactive Entities",inactive_));
    main->addLayout(cards);

    auto *panels=new QHBoxLayout;
    distribution_=new EntityDistributionWidget;
    operational_=new OperationalStatusWidget;
    sensors_=new SensorStatisticsWidget;
    panels->addWidget(group("Entity Distribution",distribution_),5);
    panels->addWidget(group("Operational Status",operational_),3);
    panels->addWidget(group("Sensor Statistics",sensors_),3);
    main->addLayout(panels,2);

    auto *charts=new QHBoxLayout;
    countChart_=new StatisticsChartWidget("Entity Count Timeline","Entities");
    tickChart_=new StatisticsChartWidget("Simulation Tick Time","Tick time (ms)");
    charts->addWidget(countChart_,1);
    charts->addWidget(tickChart_,1);
    main->addLayout(charts,2);

    auto *buttons=new QHBoxLayout;
    pauseButton_=new QPushButton("Pause Monitoring");
    auto *reset=new QPushButton("Reset History");
    auto *exportButton=new QPushButton("Export CSV");
    auto *closeButton=new QPushButton("Close");
    buttons->addWidget(pauseButton_);
    buttons->addWidget(reset);
    buttons->addWidget(exportButton);
    buttons->addStretch(); buttons->addWidget(closeButton);
    root->addLayout(buttons);
    connect(pauseButton_, &QPushButton::clicked, this, [this] {
        monitoringPaused_=!monitoringPaused_;
        pauseButton_->setText(monitoringPaused_ ? "Resume Monitoring" : "Pause Monitoring");
    });
    connect(reset,&QPushButton::clicked,this,&RuntimeStatisticsDialog::resetHistory);
    connect(exportButton,&QPushButton::clicked,this,&RuntimeStatisticsDialog::exportCsv);
    connect(closeButton,&QPushButton::clicked,this,&QDialog::close);
    // setStyleSheet("QDialog { background: #f5f6f8; }"
    //               "QGroupBox { font-weight: 600; border: 1px solid #cbd0d8;"
    //               " margin-top: 8px; background: white; }"
    //               "QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; }"
    //               "QFrame#summaryCard { background: white; border: 1px solid #cbd0d8;"
    //               " border-radius: 4px; }"
    //               "QPushButton { padding: 6px 12px; }"
    //               "QTableWidget { background: white; alternate-background-color: #f7f8fa; border: 0; }"
    //               "QHeaderView::section { background: #eceef1; padding: 4px; border: 0; }");
}
void RuntimeStatisticsDialog::updateStatistics(const RuntimeStatisticsData &data) {
    if (monitoringPaused_) return; // Pauses display sampling; never pauses simulation.
    // A new simulation run resets chart history automatically.
    if (data.sampleTimeSeconds < data_.sampleTimeSeconds) resetHistory();
    data_=data;
    scenario_->setText(data.scenario);
    status_->setText(data.status);
    status_->setStyleSheet(data.status.compare("Running",Qt::CaseInsensitive)==0
        ? "color:#13803d;font-weight:bold" : "color:#475467;font-weight:bold");
    time_->setText(data.simulationTime);
    rate_->setText(QString::number(data.simulationRate,'f',1)+"x");
    total_->setText(QString::number(data.totalEntities));
    active_->setText(QString::number(data.activeEntities));
    destroyed_->setText(QString::number(data.destroyedEntities));
    inactive_->setText(QString::number(data.inactiveEntities));
    distribution_->setData(data.distribution);
    operational_->setData(data.operational);
    sensors_->setData(data.sensors);
    const double t=qMax(0.0,data.sampleTimeSeconds);
    appendLimited(totalHistory_,{t,double(data.totalEntities)},MaxHistoryPoints);
    appendLimited(activeHistory_,{t,double(data.activeEntities)},MaxHistoryPoints);
    appendLimited(destroyedHistory_,{t,double(data.destroyedEntities)},MaxHistoryPoints);
    if (data.tickDurationMs>=0.0)
        appendLimited(tickHistory_,{t,data.tickDurationMs},MaxHistoryPoints);
    refreshCharts();
}
void RuntimeStatisticsDialog::refreshCharts() {
    countChart_->setSeries({{"Total",QColor("#2563eb"),totalHistory_},
                            {"Active",QColor("#16a34a"),activeHistory_},
                            {"Destroyed",QColor("#dc2626"),destroyedHistory_}});
    tickChart_->setSeries({{"Tick (ms)",QColor("#2563eb"),tickHistory_}});
}
void RuntimeStatisticsDialog::resetHistory() {
    totalHistory_.clear(); activeHistory_.clear(); destroyedHistory_.clear();
    tickHistory_.clear(); refreshCharts();
}
void RuntimeStatisticsDialog::exportCsv() {
    const QString path=QFileDialog::getSaveFileName(this,"Export Statistics","runtime_statistics.csv","CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this,"Export failed",file.errorString()); return;
    }
    QTextStream out(&file);
    out << "Scenario," << csvQuote(data_.scenario) << "\n";
    out << "Status," << csvQuote(data_.status) << "\n";
    out << "Simulation Time," << csvQuote(data_.simulationTime) << "\n";
    out << "Total Entities," << data_.totalEntities << "\n";
    out << "Active Entities," << data_.activeEntities << "\n";
    out << "Destroyed Entities," << data_.destroyedEntities << "\n";
    out << "Inactive Entities," << data_.inactiveEntities << "\n";
    out << "Air," << data_.distribution.air << "\n";
    out << "Ground," << data_.distribution.ground << "\n";
    out << "Sea," << data_.distribution.sea << "\n";
    out << "Static," << data_.distribution.staticEntities << "\n";
    out << "Other," << data_.distribution.other << "\n";
    out << "Moving," << data_.operational.moving << "\n";
    out << "Stationary," << data_.operational.stationary << "\n";
    out << "Airborne," << data_.operational.airborne << "\n";
    out << "Submerged," << data_.operational.submerged << "\n";
    out << "Damaged," << data_.operational.damaged << "\n";
    out << "Healthy," << data_.operational.healthy << "\n";
    out << "Total Sensors," << data_.sensors.total << "\n";
    out << "Active Sensors," << data_.sensors.active << "\n";
    out << "Radar," << data_.sensors.radar << "\n";
    out << "Sonar," << data_.sensors.sonar << "\n";
    out << "Active Tracks," << data_.sensors.activeTracks << "\n";
    out << "Total Detections," << data_.sensors.totalDetections << "\n\n";
    out << "Simulation Time (s),Total,Active,Destroyed,Tick (ms)\n";
    for (int i=0; i<totalHistory_.size(); ++i) {
        out << totalHistory_[i].x << ',' << totalHistory_[i].y << ','
            << activeHistory_[i].y << ',' << destroyedHistory_[i].y << ',';
        // Tick samples can be absent; match by time rather than index.
        QString tick;
        for (const auto &pt : tickHistory_)
            if (qAbs(pt.x-totalHistory_[i].x)<0.000001) { tick=QString::number(pt.y); break; }
        out << tick << '\n';
    }
}

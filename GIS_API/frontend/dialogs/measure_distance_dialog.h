// #ifndef API_MEASURE_DISTANCE_DIALOG_H
// #define API_MEASURE_DISTANCE_DIALOG_H

// #include <QDialog>
// #include <QTableWidget>
// #include <QComboBox>
// #include <QLabel>
// #include <QPushButton>
// #include <QPointF>
// #include <QVector>

// class ApiMeasureDistanceDialog : public QDialog {
//     Q_OBJECT
// public:
//     explicit ApiMeasureDistanceDialog(QWidget* parent = nullptr);
//     ~ApiMeasureDistanceDialog() override;
//     void setPoints(const QVector<QPointF>& geoPoints, const QVector<double>& segmentMeters);
//     void clearMeasurement();

// signals:
//     void requestNew();
//     void closedByUser();

// protected:
//     void closeEvent(QCloseEvent* event) override;

// private slots:
//     void onUnitChanged(int index);
//     void onNew();
//     void onCopy();
//     void onClose();

// private:
//     void setupUi();
//     void rebuildTable();
//     double convertFromMeters(double meters) const;
//     QString unitSuffix() const;

//     QTableWidget* m_table = nullptr;
//     QComboBox* m_unitCombo = nullptr;
//     QLabel* m_totalLabel = nullptr;
//     QPushButton* m_newBtn = nullptr;
//     QPushButton* m_copyBtn = nullptr;
//     QPushButton* m_closeBtn = nullptr;

//     QVector<QPointF> m_points;
//     QVector<double> m_segmentMeters;
//     int m_unitIndex = 0;
// };

// #endif
#ifndef API_MEASURE_DISTANCE_DIALOG_H
#define API_MEASURE_DISTANCE_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPointF>
#include <QVector>

class ApiMeasureDistanceDialog : public QDialog {
    Q_OBJECT
public:
    enum class Method { Cartesian, Ellipsoidal };

    explicit ApiMeasureDistanceDialog(QWidget* parent = nullptr);
    ~ApiMeasureDistanceDialog() override;

    // geoPoints: x = longitude, y = latitude (degrees, WGS84).
    // segmentMeters: bahar se diye gaye to wahi use hote hain (map labels ke saath
    // same rehte hain). Khali diye to dialog khud selected method se calculate karta hai.
    void setPoints(const QVector<QPointF>& geoPoints,
                   const QVector<double>& segmentMeters = QVector<double>());
    void clearMeasurement();

    Method method() const { return m_method; }

    // Static helpers (kahin aur bhi use kar sakte hain)
    static double cartesianMeters(const QPointF& a, const QPointF& b);
    static double ellipsoidalMeters(const QPointF& a, const QPointF& b);

signals:
    void requestNew();
    void closedByUser();
    void methodChanged(ApiMeasureDistanceDialog::Method method);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onUnitChanged(int index);
    void onMethodChanged();
    void onNew();
    void onCopy();
    void onClose();

private:
    void setupUi();
    void recalculateSegments();
    void rebuildTable();
    double convertFromMeters(double meters) const;
    QString unitSuffix() const;

    QTableWidget* m_table = nullptr;
    QComboBox* m_unitCombo = nullptr;
    QLabel* m_totalLabel = nullptr;
    QPushButton* m_newBtn = nullptr;
    QPushButton* m_copyBtn = nullptr;
    QPushButton* m_closeBtn = nullptr;
    QRadioButton* m_cartesianRadio = nullptr;
    QRadioButton* m_ellipsoidalRadio = nullptr;
    QButtonGroup* m_methodGroup = nullptr;

    QVector<QPointF> m_points;
    QVector<double> m_segmentMeters;
    int m_unitIndex = 0;
    Method m_method = Method::Cartesian;
    bool m_externalSegments = false;
};

#endif

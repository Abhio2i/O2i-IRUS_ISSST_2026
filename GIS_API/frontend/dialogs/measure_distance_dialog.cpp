
#include "measure_distance_dialog.h"
#include "backend/api/measure_api.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QClipboard>
#include <QApplication>
#include <QCloseEvent>
#include <QAbstractItemView>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------------
// Distance calculation helpers
// ---------------------------------------------------------------------------

// Haversine fallback (Vincenty converge na kare to use hota hai)
static double haversineMeters(const QPointF& a, const QPointF& b)
{
    const double R = 6371008.8;
    const double lat1 = a.y() * M_PI / 180.0;
    const double lat2 = b.y() * M_PI / 180.0;
    const double dLat = lat2 - lat1;
    const double dLon = (b.x() - a.x()) * M_PI / 180.0;
    const double h = std::sin(dLat / 2) * std::sin(dLat / 2) +
                     std::cos(lat1) * std::cos(lat2) *
                         std::sin(dLon / 2) * std::sin(dLon / 2);
    return 2.0 * R * std::asin(std::min(1.0, std::sqrt(h)));
}

// Cartesian (planar): points ko Web Mercator (EPSG:3857) meters mein convert
// karke seedhi straight-line distance (Pythagoras).
double ApiMeasureDistanceDialog::cartesianMeters(const QPointF& a, const QPointF& b)
{
    const double R = 6378137.0;
    const double maxLat = 85.05112878;
    auto toMercator = [&](const QPointF& p) {
        double lat = std::max(-maxLat, std::min(maxLat, p.y()));
        double x = R * p.x() * M_PI / 180.0;
        double y = R * std::log(std::tan(M_PI / 4.0 + lat * M_PI / 360.0));
        return QPointF(x, y);
    };
    const QPointF pa = toMercator(a);
    const QPointF pb = toMercator(b);
    return std::hypot(pb.x() - pa.x(), pb.y() - pa.y());
}

// Ellipsoidal (geodesic): WGS84 ellipsoid par Vincenty inverse formula.
double ApiMeasureDistanceDialog::ellipsoidalMeters(const QPointF& p1, const QPointF& p2)
{
    if (p1 == p2)
        return 0.0;

    const double a = 6378137.0;
    const double f = 1.0 / 298.257223563;
    const double b = (1.0 - f) * a;

    const double L = (p2.x() - p1.x()) * M_PI / 180.0;
    const double U1 = std::atan((1.0 - f) * std::tan(p1.y() * M_PI / 180.0));
    const double U2 = std::atan((1.0 - f) * std::tan(p2.y() * M_PI / 180.0));
    const double sinU1 = std::sin(U1), cosU1 = std::cos(U1);
    const double sinU2 = std::sin(U2), cosU2 = std::cos(U2);

    double lambda = L, lambdaP = 0.0;
    double sinSigma = 0, cosSigma = 0, sigma = 0;
    double cos2Alpha = 0, cos2SigmaM = 0;
    int iter = 0;
    bool converged = false;

    do {
        const double sinL = std::sin(lambda), cosL = std::cos(lambda);
        sinSigma = std::sqrt(std::pow(cosU2 * sinL, 2) +
                             std::pow(cosU1 * sinU2 - sinU1 * cosU2 * cosL, 2));
        if (sinSigma == 0.0)
            return 0.0; // coincident points
        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosL;
        sigma = std::atan2(sinSigma, cosSigma);
        const double sinAlpha = cosU1 * cosU2 * sinL / sinSigma;
        cos2Alpha = 1.0 - sinAlpha * sinAlpha;
        cos2SigmaM = (cos2Alpha != 0.0) ? cosSigma - 2.0 * sinU1 * sinU2 / cos2Alpha : 0.0;
        const double C = f / 16.0 * cos2Alpha * (4.0 + f * (4.0 - 3.0 * cos2Alpha));
        lambdaP = lambda;
        lambda = L + (1.0 - C) * f * sinAlpha *
                         (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma *
                                                                   (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM)));
        if (std::fabs(lambda - lambdaP) < 1e-12) {
            converged = true;
            break;
        }
    } while (++iter < 200);

    if (!converged)
        return haversineMeters(p1, p2); // nearly antipodal points

    const double uSq = cos2Alpha * (a * a - b * b) / (b * b);
    const double A = 1.0 + uSq / 16384.0 * (4096.0 + uSq * (-768.0 + uSq * (320.0 - 175.0 * uSq)));
    const double B = uSq / 1024.0 * (256.0 + uSq * (-128.0 + uSq * (74.0 - 47.0 * uSq)));
    const double dSigma = B * sinSigma * (cos2SigmaM + B / 4.0 *
                                                           (cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM) -
                                                            B / 6.0 * cos2SigmaM * (-3.0 + 4.0 * sinSigma * sinSigma) *
                                                                (-3.0 + 4.0 * cos2SigmaM * cos2SigmaM)));
    return b * A * (sigma - dSigma);
}

// ---------------------------------------------------------------------------
// Dialog
// ---------------------------------------------------------------------------

ApiMeasureDistanceDialog::ApiMeasureDistanceDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Measure Distance"));
    setMinimumSize(420, 360);
    setWindowFlags(Qt::Window |
                   Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint |
                   Qt::WindowMinMaxButtonsHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowStaysOnTopHint);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setupUi();
}

ApiMeasureDistanceDialog::~ApiMeasureDistanceDialog() {}

void ApiMeasureDistanceDialog::setupUi()
{
    setStyleSheet(
        "QDialog { background-color: #1a1a2e; color: #e0e0e0; }"
        "QTableWidget {"
        "  background-color: #16213e; color: #e0e0e0;"
        "  gridline-color: #0f3460; border: 1px solid #0f3460;"
        "  selection-background-color: #0078d7;"
        "}"
        "QHeaderView::section {"
        "  background-color: #0f3460; color: white; padding: 6px;"
        "  border: none; font-weight: bold;"
        "}"
        "QComboBox {"
        "  background-color: #16213e; color: white; padding: 6px 28px 6px 10px;"
        "  border: 1px solid #0f3460; border-radius: 4px; min-width: 110px;"
        "}"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width: 24px;"
        "  border-left: 1px solid #0f3460;"
        "  background-color: #0f3460;"
        "  border-top-right-radius: 4px;"
        "  border-bottom-right-radius: 4px;"
        "}"
        "QComboBox::down-arrow {"
        "  image: url(:/icons/images/down.png);"
        "  width: 12px; height: 12px;"
        "  margin-right: 8px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #16213e; color: white; selection-background-color: #0078d7;"
        "  border: 1px solid #0f3460;"
        "  outline: none;"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView {"
        "  background-color: #16213e; color: white; selection-background-color: #0078d7;"
        "}"
        "QLabel { color: #e0e0e0; font-size: 13px; }"
        "QRadioButton { color: #e0e0e0; font-size: 13px; spacing: 8px; }"
        "QRadioButton::indicator {"
        "  width: 12px; height: 12px;"
        "  border: 2px solid white; border-radius: 8px;"
        "  background-color: transparent;"
        "}"
        "QRadioButton::indicator:hover { border-color: #5aa9ff; }"
        "QRadioButton::indicator:checked {"
        "  border: 2px solid white;"
        "  background-color: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,"
        "    stop:0 #0078d7, stop:0.55 #0078d7, stop:0.6 transparent, stop:1 transparent);"
        "}"
        "QPushButton {"
        "  background-color: #0f3460; color: white; padding: 8px 18px;"
        "  border: none; border-radius: 4px; font-size: 13px; min-width: 80px;"
        "}"
        "QPushButton:hover { background-color: #1a4a7a; }"
        "QPushButton:pressed { background-color: #0078d7; }"
        );

    QVBoxLayout* main = new QVBoxLayout(this);
    main->setContentsMargins(12, 12, 12, 12);
    main->setSpacing(10);

    // --- Table ---
    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({tr("x"), tr("y"), tr("Distance")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(false);
    m_table->setShowGrid(true);
    main->addWidget(m_table, 1);

    // --- Method row: Cartesian / Ellipsoidal ---
    QHBoxLayout* methodRow = new QHBoxLayout();
    methodRow->setSpacing(16);
    m_cartesianRadio = new QRadioButton(tr("Cartesian"), this);
    m_ellipsoidalRadio = new QRadioButton(tr("Ellipsoidal"), this);

    // Initial state MeasureApi se lo, taaki radio aur actual method hamesha sync rahein
    m_method = (MeasureApi::instance()->distanceMethod() == MeasureApi::DistanceMethod::Ellipsoidal)
                   ? Method::Ellipsoidal : Method::Cartesian;
    m_cartesianRadio->setChecked(m_method == Method::Cartesian);
    m_ellipsoidalRadio->setChecked(m_method == Method::Ellipsoidal);

    m_methodGroup = new QButtonGroup(this);
    m_methodGroup->addButton(m_cartesianRadio);
    m_methodGroup->addButton(m_ellipsoidalRadio);

    // Group mein ek select hone par doosra unselect hota hai, isliye
    // sirf ek radio ka toggled connect karna kaafi hai (signal ek hi baar aata hai).
    connect(m_cartesianRadio, &QRadioButton::toggled,
            this, &ApiMeasureDistanceDialog::onMethodChanged);

    methodRow->addWidget(m_cartesianRadio);
    methodRow->addWidget(m_ellipsoidalRadio);
    methodRow->addStretch();
    main->addLayout(methodRow);

    // --- Unit + Total row ---
    QHBoxLayout* bottom = new QHBoxLayout();
    bottom->setSpacing(10);
    m_unitCombo = new QComboBox(this);
    m_unitCombo->addItems({tr("meters"), tr("kilometers"), tr("feet"),
                           tr("miles"), tr("nautical miles"), tr("degrees")});
    connect(m_unitCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ApiMeasureDistanceDialog::onUnitChanged);
    bottom->addWidget(m_unitCombo);
    bottom->addStretch();
    m_totalLabel = new QLabel(tr("Total: 0 m"), this);
    m_totalLabel->setStyleSheet(
        "QLabel { background-color: #16213e; border: 1px solid #0f3460;"
        " border-radius: 4px; padding: 8px 14px; font-weight: bold; }");
    bottom->addWidget(m_totalLabel);
    main->addLayout(bottom);

    // --- Buttons ---
    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->setSpacing(10);
    buttons->addStretch();
    m_newBtn = new QPushButton(tr("New"), this);
    m_copyBtn = new QPushButton(tr("Copy"), this);
    m_closeBtn = new QPushButton(tr("Close"), this);
    connect(m_newBtn, &QPushButton::clicked, this, &ApiMeasureDistanceDialog::onNew);
    connect(m_copyBtn, &QPushButton::clicked, this, &ApiMeasureDistanceDialog::onCopy);
    connect(m_closeBtn, &QPushButton::clicked, this, &ApiMeasureDistanceDialog::onClose);
    buttons->addWidget(m_newBtn);
    buttons->addWidget(m_copyBtn);
    buttons->addWidget(m_closeBtn);
    main->addLayout(buttons);
}

void ApiMeasureDistanceDialog::setPoints(const QVector<QPointF>& geoPoints,
                                         const QVector<double>& segmentMeters)
{
    m_points = geoPoints;
    if (segmentMeters.size() == geoPoints.size() && !segmentMeters.isEmpty()) {
        // Bahar (map / MeasureApi) se aayi values use karo, taaki map aur table same rahein
        m_segmentMeters = segmentMeters;
        m_externalSegments = true;
    } else {
        // Koi segments nahi diye gaye: dialog khud calculate karega
        m_externalSegments = false;
        recalculateSegments();
    }
    rebuildTable();
}

void ApiMeasureDistanceDialog::clearMeasurement()
{
    m_points.clear();
    m_segmentMeters.clear();
    rebuildTable();
}

void ApiMeasureDistanceDialog::recalculateSegments()
{
    m_segmentMeters.clear();
    m_segmentMeters.reserve(m_points.size());
    for (int i = 0; i < m_points.size(); ++i) {
        if (i == 0) {
            m_segmentMeters << 0.0;
            continue;
        }
        const double d = (m_method == Method::Ellipsoidal)
                             ? ellipsoidalMeters(m_points[i - 1], m_points[i])
                             : cartesianMeters(m_points[i - 1], m_points[i]);
        m_segmentMeters << d;
    }
}

void ApiMeasureDistanceDialog::rebuildTable()
{
    m_table->setRowCount(0);
    double totalMeters = 0.0;
    for (double d : m_segmentMeters)
        totalMeters += d;

    for (int i = 0; i < m_points.size(); ++i) {
        m_table->insertRow(i);
        const QPointF& p = m_points[i];
        QTableWidgetItem* xItem = new QTableWidgetItem(QString::number(p.x(), 'f', 6));
        QTableWidgetItem* yItem = new QTableWidgetItem(QString::number(p.y(), 'f', 6));
        const double segM = (i < m_segmentMeters.size()) ? m_segmentMeters[i] : 0.0;
        const QString distText = QString::number(convertFromMeters(i == 0 ? 0.0 : segM), 'f', 3);
        QTableWidgetItem* dItem = new QTableWidgetItem(distText);
        xItem->setTextAlignment(Qt::AlignCenter);
        yItem->setTextAlignment(Qt::AlignCenter);
        dItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, xItem);
        m_table->setItem(i, 1, yItem);
        m_table->setItem(i, 2, dItem);
    }
    m_totalLabel->setText(
        tr("Total: %1 %2")
            .arg(convertFromMeters(totalMeters), 0, 'f', 3)
            .arg(unitSuffix()));
}

double ApiMeasureDistanceDialog::convertFromMeters(double meters) const
{
    switch (m_unitIndex) {
    case 1: return meters / 1000.0;
    case 2: return meters * 3.280839895;
    case 3: return meters / 1609.344;
    case 4: return meters / 1852.0;
    case 5: {
        const double R = 6371000.0;
        return (meters / R) * (180.0 / M_PI);
    }
    default: return meters;
    }
}

QString ApiMeasureDistanceDialog::unitSuffix() const
{
    switch (m_unitIndex) {
    case 1: return QStringLiteral("km");
    case 2: return QStringLiteral("ft");
    case 3: return QStringLiteral("mi");
    case 4: return QStringLiteral("nmi");
    case 5: return QStringLiteral("°");
    default: return QStringLiteral("m");
    }
}

void ApiMeasureDistanceDialog::onUnitChanged(int index)
{
    m_unitIndex = index;
    static const char* kUnits[] = {
        "meters", "kilometers", "feet", "miles", "nautical_miles", "degrees"
    };
    if (index >= 0 && index < 6)
        MeasureApi::instance()->setDisplayUnit(QString::fromLatin1(kUnits[index]));
    rebuildTable();
}

void ApiMeasureDistanceDialog::onMethodChanged()
{
    m_method = m_ellipsoidalRadio->isChecked() ? Method::Ellipsoidal
                                               : Method::Cartesian;
    // MeasureApi ko batao: ab segments (aur map labels) isi method se banenge
    MeasureApi::instance()->setDistanceMethod(
        m_method == Method::Ellipsoidal ? MeasureApi::DistanceMethod::Ellipsoidal
                                        : MeasureApi::DistanceMethod::Cartesian);

    if (m_externalSegments) {
        // MeasureApi se naye segments lekar table refresh karo
        const QVector<double> segs = MeasureApi::instance()->getSegmentMeters();
        if (segs.size() == m_points.size())
            m_segmentMeters = segs;
    } else {
        recalculateSegments();
    }
    rebuildTable();
    emit methodChanged(m_method);
}

void ApiMeasureDistanceDialog::onNew()
{
    clearMeasurement();
    emit requestNew();
}

void ApiMeasureDistanceDialog::onCopy()
{
    QString text;
    text += QStringLiteral("Method\t%1\n")
                .arg(m_method == Method::Ellipsoidal ? "Ellipsoidal" : "Cartesian");
    text += "x\ty\tDistance\n";
    for (int r = 0; r < m_table->rowCount(); ++r) {
        QStringList cols;
        for (int c = 0; c < 3; ++c) {
            QTableWidgetItem* it = m_table->item(r, c);
            cols << (it ? it->text() : QString());
        }
        text += cols.join('\t') + "\n";
    }
    // m_totalLabel mein pehle se "Total: X unit" likha hai, isliye suffix dobara nahi jodna
    text += QString("\t\t%1\n").arg(m_totalLabel->text());
    QApplication::clipboard()->setText(text);
}

void ApiMeasureDistanceDialog::onClose()
{
    emit closedByUser();
    hide();
}

void ApiMeasureDistanceDialog::closeEvent(QCloseEvent* event)
{
    emit closedByUser();
    QDialog::closeEvent(event);
}

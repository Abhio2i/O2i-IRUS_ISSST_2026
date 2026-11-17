
// #include "measure_api.h"
// #include <cmath>

// MeasureApi* MeasureApi::m_instance = nullptr;

// MeasureApi* MeasureApi::instance()
// {
//     if (!m_instance)
//         m_instance = new MeasureApi();
//     return m_instance;
// }

// MeasureApi::MeasureApi(QObject* parent)
//     : QObject(parent)
// {
// }

// MeasureApi::~MeasureApi() {}

// void MeasureApi::startMeasuring()
// {
//     if (m_active)
//         return;
//     m_active = true;
//     m_points.clear();
//     emit measuringChanged(true);
//     emit pointsChanged();
// }

// void MeasureApi::stopMeasuring()
// {
//     if (!m_active)
//         return;
//     m_active = false;
//     emit measuringChanged(false);
// }

// void MeasureApi::addPoint(double lon, double lat)
// {
//     if (!m_active)
//         return;
//     m_points.append(QPointF(lon, lat));
//     emit pointsChanged();
// }

// void MeasureApi::addPoint(const QPointF& geoPos)
// {
//     addPoint(geoPos.x(), geoPos.y());
// }

// void MeasureApi::clearPoints()
// {
//     m_points.clear();
//     emit cleared();
//     emit pointsChanged();
// }

// QVector<double> MeasureApi::getSegmentMeters() const
// {
//     QVector<double> segs;
//     segs.reserve(m_points.size());
//     for (int i = 0; i < m_points.size(); ++i) {
//         if (i == 0) {
//             segs.append(0.0);
//         } else {
//             const QPointF& a = m_points[i - 1];
//             const QPointF& b = m_points[i];
//             segs.append(distanceMeters(a.y(), a.x(), b.y(), b.x()));
//         }
//     }
//     return segs;
// }

// double MeasureApi::getTotalMeters() const
// {
//     double total = 0.0;
//     const QVector<double> segs = getSegmentMeters();
//     for (double d : segs)
//         total += d;
//     return total;
// }

// double MeasureApi::segmentMetersAt(int index) const
// {
//     if (index < 0 || index >= m_points.size())
//         return 0.0;
//     if (index == 0)
//         return 0.0;
//     const QPointF& a = m_points[index - 1];
//     const QPointF& b = m_points[index];
//     return distanceMeters(a.y(), a.x(), b.y(), b.x());
// }

// double MeasureApi::convertFromMeters(double meters, const QString& unit) const
// {
//     const QString u = unit.toLower().trimmed();
//     if (u == "kilometers" || u == "km")
//         return meters / 1000.0;
//     if (u == "feet" || u == "ft")
//         return meters * 3.280839895;
//     if (u == "miles" || u == "mi")
//         return meters / 1609.344;
//     if (u == "nautical_miles" || u == "nautical miles" || u == "nmi" || u == "nm")
//         return meters / 1852.0;
//     if (u == "degrees" || u == "deg" || u == "°") {
//         const double R = 6371000.0;
//         return (meters / R) * (180.0 / M_PI);
//     }
//     return meters; // meters
// }

// QString MeasureApi::unitSuffix(const QString& unit) const
// {
//     const QString u = unit.toLower().trimmed();
//     if (u == "kilometers" || u == "km") return QStringLiteral("km");
//     if (u == "feet" || u == "ft") return QStringLiteral("ft");
//     if (u == "miles" || u == "mi") return QStringLiteral("mi");
//     if (u == "nautical_miles" || u == "nautical miles" || u == "nmi" || u == "nm") return QStringLiteral("nmi");
//     if (u == "degrees" || u == "deg" || u == "°") return QStringLiteral("°");
//     return QStringLiteral("m");
// }

// void MeasureApi::setDisplayUnit(const QString& unit)
// {
//     QString u = unit.toLower().trimmed();
//     if (u.isEmpty())
//         u = QStringLiteral("meters");
//     if (u == "km") u = QStringLiteral("kilometers");
//     if (u == "ft") u = QStringLiteral("feet");
//     if (u == "mi") u = QStringLiteral("miles");
//     if (u == "nmi" || u == "nm" || u == "nautical miles") u = QStringLiteral("nautical_miles");
//     if (u == "deg" || u == "°") u = QStringLiteral("degrees");
//     if (u == m_displayUnit)
//         return;
//     m_displayUnit = u;
//     emit displayUnitChanged(m_displayUnit);
// }

// double MeasureApi::convertFromMeters(double meters) const
// {
//     return convertFromMeters(meters, m_displayUnit);
// }

// QString MeasureApi::currentUnitSuffix() const
// {
//     return unitSuffix(m_displayUnit);
// }

// double MeasureApi::distanceMeters(double lat1, double lon1, double lat2, double lon2)
// {
//     const double R = 6371000.0;
//     const double toRad = M_PI / 180.0;
//     double dLat = (lat2 - lat1) * toRad;
//     double dLon = (lon2 - lon1) * toRad;
//     double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0)
//                + std::cos(lat1 * toRad) * std::cos(lat2 * toRad)
//                      * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
//     double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
//     return R * c;
// }

#include "measure_api.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MeasureApi* MeasureApi::m_instance = nullptr;
MeasureApi::DistanceMethod MeasureApi::s_method = MeasureApi::DistanceMethod::Ellipsoidal;

MeasureApi* MeasureApi::instance()
{
    if (!m_instance)
        m_instance = new MeasureApi();
    return m_instance;
}

MeasureApi::MeasureApi(QObject* parent)
    : QObject(parent)
{
}

MeasureApi::~MeasureApi() {}

void MeasureApi::startMeasuring()
{
    if (m_active)
        return;
    m_active = true;
    m_points.clear();
    emit measuringChanged(true);
    emit pointsChanged();
}

void MeasureApi::stopMeasuring()
{
    if (!m_active)
        return;
    m_active = false;
    emit measuringChanged(false);
}

void MeasureApi::addPoint(double lon, double lat)
{
    if (!m_active)
        return;
    m_points.append(QPointF(lon, lat));
    emit pointsChanged();
}

void MeasureApi::addPoint(const QPointF& geoPos)
{
    addPoint(geoPos.x(), geoPos.y());
}

void MeasureApi::clearPoints()
{
    m_points.clear();
    emit cleared();
    emit pointsChanged();
}

QVector<double> MeasureApi::getSegmentMeters() const
{
    QVector<double> segs;
    segs.reserve(m_points.size());
    for (int i = 0; i < m_points.size(); ++i) {
        if (i == 0) {
            segs.append(0.0);
        } else {
            const QPointF& a = m_points[i - 1];
            const QPointF& b = m_points[i];
            segs.append(distanceMeters(a.y(), a.x(), b.y(), b.x()));
        }
    }
    return segs;
}

double MeasureApi::getTotalMeters() const
{
    double total = 0.0;
    const QVector<double> segs = getSegmentMeters();
    for (double d : segs)
        total += d;
    return total;
}

double MeasureApi::segmentMetersAt(int index) const
{
    if (index < 0 || index >= m_points.size())
        return 0.0;
    if (index == 0)
        return 0.0;
    const QPointF& a = m_points[index - 1];
    const QPointF& b = m_points[index];
    return distanceMeters(a.y(), a.x(), b.y(), b.x());
}

double MeasureApi::convertFromMeters(double meters, const QString& unit) const
{
    const QString u = unit.toLower().trimmed();
    if (u == "kilometers" || u == "km")
        return meters / 1000.0;
    if (u == "feet" || u == "ft")
        return meters * 3.280839895;
    if (u == "miles" || u == "mi")
        return meters / 1609.344;
    if (u == "nautical_miles" || u == "nautical miles" || u == "nmi" || u == "nm")
        return meters / 1852.0;
    if (u == "degrees" || u == "deg" || u == "°") {
        const double R = 6371000.0;
        return (meters / R) * (180.0 / M_PI);
    }
    return meters; // meters
}

QString MeasureApi::unitSuffix(const QString& unit) const
{
    const QString u = unit.toLower().trimmed();
    if (u == "kilometers" || u == "km") return QStringLiteral("km");
    if (u == "feet" || u == "ft") return QStringLiteral("ft");
    if (u == "miles" || u == "mi") return QStringLiteral("mi");
    if (u == "nautical_miles" || u == "nautical miles" || u == "nmi" || u == "nm") return QStringLiteral("nmi");
    if (u == "degrees" || u == "deg" || u == "°") return QStringLiteral("°");
    return QStringLiteral("m");
}

void MeasureApi::setDisplayUnit(const QString& unit)
{
    QString u = unit.toLower().trimmed();
    if (u.isEmpty())
        u = QStringLiteral("meters");
    if (u == "km") u = QStringLiteral("kilometers");
    if (u == "ft") u = QStringLiteral("feet");
    if (u == "mi") u = QStringLiteral("miles");
    if (u == "nmi" || u == "nm" || u == "nautical miles") u = QStringLiteral("nautical_miles");
    if (u == "deg" || u == "°") u = QStringLiteral("degrees");
    if (u == m_displayUnit)
        return;
    m_displayUnit = u;
    emit displayUnitChanged(m_displayUnit);
}

double MeasureApi::convertFromMeters(double meters) const
{
    return convertFromMeters(meters, m_displayUnit);
}

QString MeasureApi::currentUnitSuffix() const
{
    return unitSuffix(m_displayUnit);
}

// ---------------------------------------------------------------------------
// Distance method (Cartesian / Ellipsoidal)
// ---------------------------------------------------------------------------

void MeasureApi::setDistanceMethod(DistanceMethod method)
{
    if (s_method == method)
        return;
    s_method = method;
    emit distanceMethodChanged(method);
    // Segments / map labels dobara banane ke liye (pointsChanged par jo refresh
    // pehle se hota hai, wahi chalega)
    emit pointsChanged();
}

MeasureApi::DistanceMethod MeasureApi::distanceMethod() const
{
    return s_method;
}

// Haversine: sirf fallback ke liye (Vincenty converge na kare)
static double haversineMeters(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6371008.8;
    const double toRad = M_PI / 180.0;
    const double dLat = (lat2 - lat1) * toRad;
    const double dLon = (lon2 - lon1) * toRad;
    const double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0)
                     + std::cos(lat1 * toRad) * std::cos(lat2 * toRad)
                           * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
    const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return R * c;
}

// Cartesian (planar): Web Mercator (EPSG:3857) meters mein straight line (Pythagoras)
double MeasureApi::cartesianMeters(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6378137.0;
    const double maxLat = 85.05112878;
    auto toMercator = [&](double lat, double lon, double& x, double& y) {
        lat = std::max(-maxLat, std::min(maxLat, lat));
        x = R * lon * M_PI / 180.0;
        y = R * std::log(std::tan(M_PI / 4.0 + lat * M_PI / 360.0));
    };
    double x1, y1, x2, y2;
    toMercator(lat1, lon1, x1, y1);
    toMercator(lat2, lon2, x2, y2);
    return std::hypot(x2 - x1, y2 - y1);
}

// Ellipsoidal (geodesic): WGS84 ellipsoid par Vincenty inverse formula
double MeasureApi::ellipsoidalMeters(double lat1, double lon1, double lat2, double lon2)
{
    if (lat1 == lat2 && lon1 == lon2)
        return 0.0;

    const double a = 6378137.0;
    const double f = 1.0 / 298.257223563;
    const double b = (1.0 - f) * a;

    const double L = (lon2 - lon1) * M_PI / 180.0;
    const double U1 = std::atan((1.0 - f) * std::tan(lat1 * M_PI / 180.0));
    const double U2 = std::atan((1.0 - f) * std::tan(lat2 * M_PI / 180.0));
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
            return 0.0;
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
        return haversineMeters(lat1, lon1, lat2, lon2);

    const double uSq = cos2Alpha * (a * a - b * b) / (b * b);
    const double A = 1.0 + uSq / 16384.0 * (4096.0 + uSq * (-768.0 + uSq * (320.0 - 175.0 * uSq)));
    const double B = uSq / 1024.0 * (256.0 + uSq * (-128.0 + uSq * (74.0 - 47.0 * uSq)));
    const double dSigma = B * sinSigma * (cos2SigmaM + B / 4.0 *
                                                           (cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM) -
                                                            B / 6.0 * cos2SigmaM * (-3.0 + 4.0 * sinSigma * sinSigma) *
                                                                (-3.0 + 4.0 * cos2SigmaM * cos2SigmaM)));
    return b * A * (sigma - dSigma);
}

// Segment distance: selected method ke hisaab se
double MeasureApi::distanceMeters(double lat1, double lon1, double lat2, double lon2)
{
    return (s_method == DistanceMethod::Ellipsoidal)
               ? ellipsoidalMeters(lat1, lon1, lat2, lon2)
               : cartesianMeters(lat1, lon1, lat2, lon2);
}

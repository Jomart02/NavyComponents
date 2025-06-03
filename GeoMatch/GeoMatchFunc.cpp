// DirectCalculation.cpp
#include "GeoMatchFunc.h"
#include <cmath>
#include <iostream>

constexpr double EPSILON = 1e-12;

double NormalizeLat(double lat) {
    if (std::isnan(lat)) return lat;

    // Приводим к [-π; π]
    lat = fmod(lat + M_PI, M_2PI) - M_PI;

    // Если выходит за пределы допустимого диапазона широты
    if (lat > M_PI_2) lat = M_PI_2;
    else if (lat < -M_PI_2) lat = -M_PI_2;

    return lat;
}
double NormalizeLon(double lon) {
    if (std::isnan(lon)) return lon;

    // Приводим к (-π, π]
    lon = fmod(lon + M_PI, M_2PI) - M_PI;

    return lon;
}


DirectLocError PositionByDistanceBearing(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    DirectCalculationMethod method,
    GEO_POINT& endPnt
) {
    // Проверка входных данных
    if (std::isnan(startPnt.lat) || std::abs(startPnt.lat) > M_PI_2 + 1e-9) {
        return DirectLocError::DIRECLOC_INVALID_LAT;
    }
    if (std::isnan(startPnt.lon)) {
        return DirectLocError::DIRECLOC_INVALID_LON;
    }
    if (distanceMeters < 0.01) {
        return DirectLocError::DIRECLOC_DISTANCE_TOO_SMALL;
    }

    switch (method) {
        case DirectCalculationMethod::GEODESIC:
            return PositionByDistanceBearing_Geodesic(startPnt, distanceMeters, bearingRad, ellipsoidType, endPnt);

        case DirectCalculationMethod::LOXODROME:
            return PositionByDistanceBearing_Loxodrome(startPnt, distanceMeters, bearingRad, ellipsoidType, endPnt);

        case DirectCalculationMethod::GREAT_CIRCLE:
            return PositionByDistanceBearing_Ortho(startPnt, distanceMeters, bearingRad, ellipsoidType, endPnt);

        default:
            return DirectLocError::DIRECLOC_INVALID_METHOD;
    }
}

DirectLocError PositionByDistanceBearing_Geodesic(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
) {
    if (std::isnan(startPnt.lat) || std::abs(startPnt.lat) > M_PI_2 + 1e-9)
        return DirectLocError::DIRECLOC_INVALID_LAT;
    if (std::isnan(startPnt.lon))
        return DirectLocError::DIRECLOC_INVALID_LON;
    if (distanceMeters < 0.01)
        return DirectLocError::DIRECLOC_DISTANCE_TOO_SMALL;

    Ellipsoid ell = Ellipsoid::Get(ellipsoidType);
    const double a = ell.a;
    const double b = ell.b;
    const double f = (a - b) / a;

    const double beta1 = atan((1 - f) * tan(startPnt.lat));
    const double sinBeta1 = sin(beta1);
    const double cosBeta1 = cos(beta1);

    const double sinAlpha = sin(bearingRad);
    const double cosAlpha = cos(bearingRad);
    const double cos2Alpha = cosAlpha * cosAlpha;
    const double u2 = cos2Alpha * ((a * a - b * b) / (b * b));

    const double A = 1 + u2 / 16384 * (4096 + u2 * (-768 + u2 * (320 - 175 * u2)));
    const double B = u2 / 1024 * (256 + u2 * (-128 + u2 * (74 - 47 * u2)));

    double sigma = distanceMeters / (b * A);
    double sigmaOld = 0;
    double sinSigma, cosSigma; // Убираем const
    double deltaSigma, cos2SigmaM;

    constexpr int MAX_ITERATIONS = 20;

    for (int i = 0; i < MAX_ITERATIONS && std::abs(sigma - sigmaOld) > 1e-12; ++i) {
        sigmaOld = sigma;
        sinSigma = sin(sigma);  // Теперь это просто double
        cosSigma = cos(sigma);
        cos2SigmaM = cos(2 * beta1 + sigma);
        deltaSigma = B * sinSigma * (
            cos2SigmaM + B / 4 * (
                cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) -
                B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) *
                (-3 + 4 * cos2SigmaM * cos2SigmaM)
            )
        );
        sigma = distanceMeters / (b * A) + deltaSigma;
    }

    const double sinSigmaFinal = sin(sigma);
    const double cosSigmaFinal = cos(sigma);

    const double sinBeta2 = sinBeta1 * cosSigmaFinal + cosBeta1 * sinSigmaFinal * cosAlpha;
    endPnt.lat = atan2(sinBeta2, sqrt(
        pow(cosBeta1 * sinSigmaFinal * cosAlpha - sinBeta1 * cosSigmaFinal, 2) +
        pow(cosBeta1 * sinSigmaFinal * sinAlpha, 2)
    ) / (1 - f));

    const double lambda = atan2(
        sinSigmaFinal * sinAlpha,
        cosBeta1 * cosSigmaFinal - sinBeta1 * sinSigmaFinal * cosAlpha
    );

    const double C = f / 16 * cos2Alpha * (4 + f * (4 - 3 * cos2Alpha));
    const double L = lambda - (1 - C) * f * sinAlpha * (
        sigma + C * sinSigmaFinal * (cos2SigmaM + C * cosSigmaFinal * (-1 + 2 * cos2SigmaM * cos2SigmaM)));

    endPnt.lon = startPnt.lon + L;

    // Нормализуем координаты
    endPnt.lat = NormalizeLat(endPnt.lat);
    endPnt.lon = NormalizeLon(endPnt.lon);

    return DirectLocError::DIRECLOC_NOERROR;
}

DirectLocError PositionByDistanceBearing_Ortho(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
) {
    if (std::isnan(startPnt.lat) || std::abs(startPnt.lat) > M_PI_2 + 1e-9)
        return DirectLocError::DIRECLOC_INVALID_LAT;
    if (std::isnan(startPnt.lon))
        return DirectLocError::DIRECLOC_INVALID_LON;
    if (distanceMeters < 0.01)
        return DirectLocError::DIRECLOC_DISTANCE_TOO_SMALL;

    Ellipsoid ell = Ellipsoid::Get(ellipsoidType);
    const double R = ell.a;
    const double angularDist = distanceMeters / R;

    const double sinLat1 = sin(startPnt.lat);
    const double cosLat1 = cos(startPnt.lat);

    const double sinLat2 = sinLat1 * cos(angularDist) +
                           cosLat1 * sin(angularDist) * cos(bearingRad);
    endPnt.lat = asin(sinLat2);

    const double y = sin(bearingRad) * sin(angularDist) * cosLat1;
    const double x = cos(angularDist) - sinLat1 * sinLat2;
    endPnt.lon = startPnt.lon + atan2(y, x);

    // Нормализуем
    endPnt.lat = NormalizeLat(endPnt.lat);
    endPnt.lon = NormalizeLon(endPnt.lon);

    return DirectLocError::DIRECLOC_NOERROR;
}

DirectLocError PositionByDistanceBearing_Loxodrome(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
) {
    if (std::isnan(startPnt.lat) || std::abs(startPnt.lat) > M_PI_2 + 1e-9)
        return DirectLocError::DIRECLOC_INVALID_LAT;
    if (std::isnan(startPnt.lon))
        return DirectLocError::DIRECLOC_INVALID_LON;
    if (distanceMeters < 0.01)
        return DirectLocError::DIRECLOC_DISTANCE_TOO_SMALL;

    Ellipsoid ell = Ellipsoid::Get(ellipsoidType);
    const double R = ell.a;

    // Вычисляем изменения
    double dPhi = distanceMeters * cos(bearingRad) / R;
    endPnt.lat = startPnt.lat + dPhi;

    // Средняя широта
    double phiMid = (startPnt.lat + endPnt.lat) / 2.0;

    double dLambda = distanceMeters * sin(bearingRad) / (R * cos(phiMid));
    endPnt.lon = startPnt.lon + dLambda;

    // Нормализуем
    endPnt.lat = NormalizeLat(endPnt.lat);
    endPnt.lon = NormalizeLon(endPnt.lon);

    return DirectLocError::DIRECLOC_NOERROR;
}
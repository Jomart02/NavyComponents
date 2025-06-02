// DirectCalculation.cpp
#include "GeoMatchFunc.h"
#include <cmath>
#include <iostream>

constexpr double EPSILON = 1e-12;

DirectLocError PositionByDistanceBearing(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    DirectCalculationMethod method,
    GEO_POINT& endPnt
) {
    if (std::isnan(startPnt.lat) || std::abs(startPnt.lat) > M_PI_2 + EPSILON)
        return DirectLocError::DIRECLOC_INVALID_LAT;
    if (std::isnan(startPnt.lon))
        return DirectLocError::DIRECLOC_INVALID_LON;
    if (distanceMeters < 0.01)
        return DirectLocError::DIRECLOC_DISTANCE_TOO_SMALL;

    Ellipsoid ell = Ellipsoid::Get(ellipsoidType);

    switch (method) {
        case DirectCalculationMethod::GEODESIC: {
            // Используем упрощённый алгоритм Винсента
            constexpr int MAX_ITERATIONS = 20;
            const double a = ell.a;
            const double f = 1.0 / ell.f_inv;
            const double b = ell.b;

            const double beta1 = atan((1 - f) * tan(startPnt.lat));
            const double sinBeta1 = sin(beta1);
            const double cosBeta1 = cos(beta1);

            const double tanAz = cos(bearingRad);
            const double sinAlpha = sin(bearingRad);
            const double cos2Alpha = 1 - sinAlpha * sinAlpha;
            const double u2 = cos2Alpha * ((a * a - b * b) / (b * b));
            const double A = 1 + u2 / 16384 * (4096 + u2 * (-768 + u2 * (320 - 175 * u2)));
            const double B = u2 / 1024 * (256 + u2 * (-128 + u2 * (74 - 47 * u2)));

            double sigma = distanceMeters / (b * A);
            double sigmaOld = 0;
            double sinSigma = 0, cosSigma = 0, deltaSigma = 0, cos2SigmaM = 0;

            for (int i = 0; i < MAX_ITERATIONS && std::abs(sigma - sigmaOld) > 1e-12; ++i) {
                sigmaOld = sigma;
                sinSigma = sin(sigma);
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

            const double beta2 = atan2(sinBeta1 * cosSigma + cosBeta1 * sinSigma * cos(bearingRad),
                                       (1 - f) * sqrt(sinAlpha * sinAlpha + (sinBeta1 * sinSigma - cosBeta1 * cosSigma * cos(bearingRad)) *
                                                     (sinBeta1 * sinSigma - cosBeta1 * cosSigma * cos(bearingRad))));
            const double lambda = atan2(sinSigma * sinAlpha,
                                        cosBeta1 * cosSigma - sinBeta1 * sinSigma * cos(bearingRad));

            const double C = f / 16 * cos2Alpha * (4 + f * (4 - 3 * cos2Alpha));
            const double L = lambda - (1 - C) * f * sinAlpha * (
                sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));

            endPnt.lat = beta2;
            endPnt.lon = startPnt.lon + L;
            break;
        }

        case DirectCalculationMethod::LOXODROME: {
            // Локсодромический расчёт
            const double R = ell.a;
            const double dPhi = distanceMeters * cos(bearingRad) / R;
            const double phiMid = (startPnt.lat + endPnt.lat) / 2.0;
            const double dLambda = distanceMeters * sin(bearingRad) / (R * cos(phiMid));
            endPnt.lat = startPnt.lat + dPhi;
            endPnt.lon = startPnt.lon + dLambda;
            break;
        }

        case DirectCalculationMethod::GREAT_CIRCLE: {
            // Расчёт по сфере (ортодромия)
            const double R = ell.a;
            const double sinLat1 = sin(startPnt.lat);
            const double cosLat1 = cos(startPnt.lat);
            const double angularDist = distanceMeters / R;

            const double sinLat2 = sin(startPnt.lat) * cos(angularDist) +
                                   cosLat1 * sin(angularDist) * cos(bearingRad);

            endPnt.lat = asin(sinLat2);

            const double y = sin(bearingRad) * sin(angularDist) * cosLat1;
            const double x = cos(angularDist) - sinLat1 * sinLat2;
            endPnt.lon = startPnt.lon + atan2(y, x);
            break;
        }

        default:
            return DirectLocError::DIRECLOC_INVALID_LAT;
    }

    return DirectLocError::DIRECLOC_NOERROR;
}
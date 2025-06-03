
#pragma once
#include "defines.h"

enum class DirectLocError {
    DIRECLOC_NOERROR = 0,
    DIRECLOC_INVALID_LAT,
    DIRECLOC_INVALID_LON,
    DIRECLOC_DISTANCE_TOO_SMALL,
    DIRECLOC_INVALID_METHOD
};

DirectLocError PositionByDistanceBearing(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoid,
    DirectCalculationMethod method,
    GEO_POINT& endPnt
);
DirectLocError PositionByDistanceBearing_Geodesic(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
);
DirectLocError PositionByDistanceBearing_Loxodrome(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
) ;
DirectLocError PositionByDistanceBearing_Ortho(
    const GEO_POINT& startPnt,
    double distanceMeters,
    double bearingRad,
    EllipsoidType ellipsoidType,
    GEO_POINT& endPnt
);

inline double DegToRad(double deg) { return deg * M_PI / 180.0; }
inline double RadToDeg(double rad) { return rad * 180.0 / M_PI; }
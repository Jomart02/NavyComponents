#include "NaviStrings.h"
#include <cmath>

#define PRESISSION 0.005

#if defined(WIN32)
    #define DEGREE_SYMBOL "\xB0"
#else if defined(unix)
    #define DEGREE_SYMBOL "°"
#endif

QString geoLat(double lat)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(lat, dgr, minutes, 3);
    auto size = snprintf(buffer, sizeof(buffer), "%02d%s %06.3f'%s", static_cast<int>(dgr), DEGREE_SYMBOL, minutes, (lat < 0 ? "S" : "N"));
    if(size < 0) {
        return QString("--\xB0 --.---'-");
    }
    return QString::fromLocal8Bit(buffer, size);
}

QString geoLon(double lon)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(lon, dgr, minutes, 3);
    auto size = snprintf(buffer, sizeof(buffer), "%03d%s %06.3f'%s", static_cast<int>(dgr), DEGREE_SYMBOL, minutes, (lon < 0 ? "W" : "E"));
    return QString::fromLocal8Bit(buffer, size);
}

QString timer(uint t)
{
    char buffer[32];
    uint h, m, s;
    decomposeTime(t, h, m, s);
    if(h > 99) {
        return ("99:99:99");
    }
    auto size = snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", h, m, s);
    if(size < 0) {
        return ("99:99:99");
    }

    return QString::fromLocal8Bit(buffer, size);

}

void decomposeCoordinate(double angle, double& degrees, double& minutes, unsigned int min_decimals, double* seconds)
{
    degrees = fabs(angle);
    minutes = modf(degrees, & degrees) * 60.;
    if(min_decimals > 0) {
        double t = pow(10, min_decimals);
        minutes = round(t * minutes) / t;
        if(minutes >= 60.) {
            minutes -= 60.;
            degrees++;
        }
    }

    if(seconds != nullptr) {
        * seconds = modf(minutes, & minutes) * 60.;
    }

}

void decomposeTime(uint time, uint& h, uint& m, uint& s)
{
    s = time % 60;
    time -= s;
    m = time % 3600;
    h = (time - m) / 3600;
    m /= 60;
}

QString geoDeltaLat(double lat)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(lat, dgr, minutes, 3);
    auto size = snprintf(buffer, sizeof(buffer), "%s%02d%s %06.3f'", (lat < 0 ? "-" : "+"), static_cast<int>(dgr), DEGREE_SYMBOL, minutes);
    if(size < 0) {
        return QString("+--\xB0 --.---'");
    }

    return QString::fromLocal8Bit(buffer, size);
}

QString geoDeltaLon(double lon)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(lon, dgr, minutes, 3);
    auto size = snprintf(buffer, sizeof(buffer), "%s%03d%s %06.3f'", (lon < 0 ? "-" : "+"), static_cast<int>(dgr), DEGREE_SYMBOL, minutes);
    if(size < 0) {
        return QString("+---\xB0 --.---'");
    }

    return QString::fromLocal8Bit(buffer, size);
}

QString geoAltitude(double alt)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(alt, dgr, minutes, 1);
    auto size = snprintf(buffer, sizeof(buffer), "%s%02d%s %04.1f'", (alt < 0 ? "-" : "+"), static_cast<int>(dgr), DEGREE_SYMBOL, minutes);
    if(size < 0) {
        return QString("+--\xB0 --.-'");
    }

    return QString::fromLocal8Bit(buffer, size);
}

QString geoAzimuth(double az)
{
    char buffer[32];
    double dgr, minutes;
    decomposeCoordinate(az, dgr, minutes, 3);
    auto size = snprintf(buffer, sizeof(buffer), "%03d%s %06.3f'", static_cast<int>(dgr), DEGREE_SYMBOL, minutes);
    if(size < 0) {
        return QString("---\xB0 --.---'");
    }

    return QString::fromLocal8Bit(buffer, size);
}
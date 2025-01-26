#pragma once
#include <QString>

QString geoLat(double lat);
QString geoLon(double lon);
QString geoDeltaLat(double lat);
QString geoDeltaLon(double lon);
QString geoAltitude(double alt);
QString geoAzimuth(double az);
QString timer(uint t);

void decomposeCoordinate(double angle, double& degrees, double& minutes, unsigned int min_decimals = 0, double* seconds = nullptr);
void decomposeTime(uint time, uint& h, uint& m, uint& s);

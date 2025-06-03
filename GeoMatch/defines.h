#pragma once

#include <cmath>
#define M_2PI 6.283185307179586 // 2 * M_PI

// Перечисление доступных эллипсоидов
enum class EllipsoidType {
    WGS84,
    KRASSOVSKY,
    GRS80,
    DEFAULT = WGS84
};

// Перечисление методов решения прямой задачи счисления
enum class DirectCalculationMethod {
    GEODESIC,   // Точное решение (геодезическая линия)
    LOXODROME,  // По локсодроме (приближённо)
    GREAT_CIRCLE // По ортодроме на сфере
};

// Структура для хранения точки на поверхности Земли
struct GEO_POINT {
    double lat; // Широта в радианах
    double lon; // Долгота в радианах

    GEO_POINT() : lat(0.0), lon(0.0) {}
    GEO_POINT(double latitude, double longitude) : lat(latitude), lon(longitude) {}
};

struct Ellipsoid {
     double a;     // Большая полуось
     double b;     // Малая полуось
     double f_inv; // Обратное сжатие (1/f)

     static Ellipsoid Get(EllipsoidType type){
          Ellipsoid e;
          switch (type) {
               case EllipsoidType::WGS84:
                    e.a = 6378137.0;
                    e.f_inv = 298.257223563;
                    break;
               case EllipsoidType::KRASSOVSKY:
                    e.a = 6378245.0;
                    e.f_inv = 298.3;
                    break;
               case EllipsoidType::GRS80:
                    e.a = 6378137.0;
                    e.f_inv = 298.257222101;
                    break;
               default:
                    e.a = 6378137.0;
                    e.f_inv = 298.257223563;
                    break;
          }
          e.b = e.a * (1.0 - 1.0 / e.f_inv);
          return e;
     }
};
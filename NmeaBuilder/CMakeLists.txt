cmake_minimum_required(VERSION 3.5)

set(TARGET nmeabuilder)

message("Building " ${TARGET})

set(SOURCE_LIB nmea.cpp nmea.h)
add_library(${TARGET} STATIC ${SOURCE_LIB})
target_include_directories(${TARGET} PUBLIC ./)
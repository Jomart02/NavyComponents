cmake_minimum_required(VERSION 3.5)

set(TARGET GeoMatch)

message("Building " ${TARGET})

set(SOURCE_LIB defines.h GeoMatchFunc.cpp GeoMatchFunc.h)
add_library(${TARGET} STATIC ${SOURCE_LIB})
target_include_directories(${TARGET} PUBLIC ./)
cmake_minimum_required(VERSION 3.5)

set(TARGET CoordinateEdit)

set(CMAKE_AUTOMOC ON)  
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCE_LIB CoordinateEdit.cpp CoordinateEdit.h NaviStrings.cpp NaviStrings.h )

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

message("Building " ${TARGET})

add_library(${TARGET} STATIC ${SOURCE_LIB})
target_link_libraries(${TARGET} PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(${TARGET} PUBLIC ./)

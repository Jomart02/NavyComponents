cmake_minimum_required(VERSION 3.5)

set(PLUGIN CircularValueWidgets)
project(${PLUGIN})
# project(CircularValueWidgets)

set(CMAKE_AUTOMOC ON)  
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HDRS
    CircularValueWidget.h
    SpeedometerWidget.h
    SpeedometerWidgetPlugin.h
)

set(SRCS
    CircularValueWidget.cpp
    SpeedometerWidget.cpp
    SpeedometerWidgetPlugin.cpp
)

message(STATUS "QT6_INSTALL_PREFIX: ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}")

find_package(Qt6 COMPONENTS Widgets Core Gui Designer REQUIRED)

set(QRCS
    ./res/res.qrc
)

add_library(${PLUGIN} SHARED ${HDRS} ${SRCS} ${QRCS})

target_link_libraries(${PLUGIN} PRIVATE Qt6::Widgets Qt6::Designer Qt6::Core Qt6::Gui)


# add_executable(${PROJECT_NAME}  ${SRCS} ${QRCS})
# target_link_libraries(${PROJECT_NAME}  Qt6::Core Qt6::Widgets)

install(TARGETS ${PLUGIN}
    DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/designer
)

target_include_directories(${PLUGIN} PUBLIC ./)
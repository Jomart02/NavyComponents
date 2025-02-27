cmake_minimum_required(VERSION 3.5)


project(CircularValueWidgets)

set(CMAKE_AUTOMOC ON)  
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRCS CircularValueWidget.cpp CircularValueWidget.h SpeedometerWidget.cpp SpeedometerWidget.h main.cpp)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

set(QRCS
    ./res/res.qrc
)

add_executable(${PROJECT_NAME}  ${SRCS} ${QRCS})
target_link_libraries(${PROJECT_NAME}  Qt6::Core Qt6::Widgets)


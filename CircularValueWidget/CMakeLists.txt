cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME CircularValueWidgets)
project(${PROJECT_NAME} LANGUAGES CXX)

# Настройки Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("Building " ${PROJECT_NAME})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/BaseCircularWidget/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/SpeedometerWidget/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/CompassWidget/)
# Опция для включения тестов
option(BUILD_TESTS "Build test executable" OFF)

# Исходные файлы для виджетов (без плагинов)
set(WIDGET_HDRS
    ./include/BaseCircularWidget/CircularValueWidget.h
    ./include/SpeedometerWidget/SpeedometerWidget.h
    ./include/CompassWidget/CompassWidget.h
)

set(WIDGET_SRCS
    ./src/BaseCircularWidget/CircularValueWidget.cpp
    ./src/SpeedometerWidget/SpeedometerWidget.cpp
    ./src/CompassWidget/CompassWidget.cpp
)

# Исходные файлы для плагинов
set(PLUGIN_COMPASS_HDRS
    ./include/CompassWidget/CompassWidgetPlugin.h
)
set(PLUGIN_SPEED_HDRS
    ./include/SpeedometerWidget/SpeedometerWidgetPlugin.h
)

set(PLUGIN_COMPASS_SRCS
    ./src/CompassWidget/CompassWidgetPlugin.cpp
)
set(PLUGIN_SPEED_SRCS
    ./src/SpeedometerWidget/SpeedometerWidgetPlugin.cpp
)

# Ресурсы
set(QRCS
    ./res/resCircularWidgets.qrc
)

# Поиск Qt6
find_package(Qt6 COMPONENTS Widgets Core Gui Designer REQUIRED)

# Создание библиотеки для виджетов (статическая, чтобы использовать в плагинах и тестах)
add_library(${PROJECT_NAME} STATIC ${WIDGET_HDRS} ${WIDGET_SRCS} ${QRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
target_include_directories(${PROJECT_NAME} PUBLIC ./)

# Создание плагинов как отдельных библиотек
add_library(SpeedometerWidgetPlugin SHARED ${WIDGET_HDRS} ${PLUGIN_SPEED_HDRS} ${PLUGIN_SPEED_SRCS})
target_link_libraries(SpeedometerWidgetPlugin PRIVATE ${PROJECT_NAME}  Qt6::Widgets Qt6::Designer Qt6::Core Qt6::Gui)
target_include_directories(SpeedometerWidgetPlugin PUBLIC ./)

add_library(CompassWidgetPlugin SHARED ${WIDGET_HDRS} ${PLUGIN_COMPASS_HDRS} ${PLUGIN_COMPASS_SRCS})
target_link_libraries(CompassWidgetPlugin PRIVATE ${PROJECT_NAME} Qt6::Widgets Qt6::Designer Qt6::Core Qt6::Gui) 
target_include_directories(CompassWidgetPlugin PUBLIC ./)


set(PLUGINS
    SpeedometerWidgetPlugin
    CompassWidgetPlugin
)


install(TARGETS ${PLUGINS}
    DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/designer
)

#Сборка тестов (если включена)

if(BUILD_TESTS)
    add_executable(${PROJECT_NAME}_Test ${WIDGET_HDRS} ${WIDGET_SRCS} ${QRCS} main.cpp)
    target_link_libraries(${PROJECT_NAME}_Test PRIVATE ${PROJECT_NAME} Qt6::Widgets Qt6::Core Qt6::Gui)
    target_include_directories(${PROJECT_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BaseCircularWidget
        ${CMAKE_CURRENT_SOURCE_DIR}/include/SpeedometerWidget
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CompassWidget
    )
endif()
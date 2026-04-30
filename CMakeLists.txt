cmake_minimum_required(VERSION 3.16)
project(XForge VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)
# Fallback to Qt5 if Qt6 not found
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Charts)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    MainWindow.cpp
    XFoil_Interface.cpp
    DigestPolar.cpp
)

set(HEADERS
    MainWindow.h
    XFoil_Interface.h
    DigestPolar.h
)

add_executable(XForge WIN32 ${SOURCES} ${HEADERS})

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(XForge PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts)
else()
    target_link_libraries(XForge PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Charts)
endif()
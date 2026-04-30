QT       += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET   = XForge
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    XFoil_Interface.cpp \
    DigestPolar.cpp

HEADERS += \
    MainWindow.h \
    XFoil_Interface.h \
    DigestPolar.h

# Suppress Qt deprecation warnings
DEFINES += QT_DEPRECATED_WARNINGS

# Windows: ensure console not shown
win32: CONFIG += windows

DISTFILES += \
    CMakeLists.txt \
    xfoil.exe
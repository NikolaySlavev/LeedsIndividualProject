QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = Streets
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11


QT += widgets opengl gui
LIBS += -lGLU32 -lOpenGL32

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainwidget.cpp \
    Image.cpp \
    database.cpp \
    structures.cpp \
    drawstreet.cpp \
    layout.cpp \
    buildingblocks.cpp \
    junction.cpp

HEADERS += \
    mainwindow.h \
    mainwidget.h \
    Image.h \
    database.h \
    drawstreet.h \
    layout.h \
    structures.h \
    buildingblocks.h \
    junction.h

FORMS += \
        mainwindow.ui


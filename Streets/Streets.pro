QT       += core gui
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
    mainwindow.cpp \
    mainwidget.cpp \
    Image.cpp

HEADERS += \
        mainwindow.h \
    mainwidget.h \
    Image.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

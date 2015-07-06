#-------------------------------------------------
#
# Project created by QtCreator 2015-06-26T12:47:30
#
#-------------------------------------------------

TARGET = GridStatDecorator
TEMPLATE = lib
QT       += widgets
INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

DEFINES += GRIDSTATDECORATOR_LIBRARY

SOURCES += gridstatdecorator.cpp

HEADERS += gridstatdecorator.h\
        gridstatdecorator_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    gridstatdecorator.json

FORMS += \
    control.ui

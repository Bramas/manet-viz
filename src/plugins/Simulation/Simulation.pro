#-------------------------------------------------
#
# Project created by QtCreator 2015-07-16T11:54:50
#
#-------------------------------------------------

QT       += widgets concurrent

TARGET = Simulation
TEMPLATE = lib

INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

DEFINES += SIMULATION_LIBRARY

SOURCES += simulation.cpp

HEADERS += simulation.h\
        simulation_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    simulation.json

FORMS += \
    control.ui

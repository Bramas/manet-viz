#-------------------------------------------------
#
# Project created by QtCreator 2015-08-10T15:14:54
#
#-------------------------------------------------

QT       += widgets concurrent

INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

TARGET = SenseArea
TEMPLATE = lib

DEFINES += SENSEAREA_LIBRARY

SOURCES += sensearea.cpp

HEADERS += sensearea.h\
        sensearea_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    sensearea.json

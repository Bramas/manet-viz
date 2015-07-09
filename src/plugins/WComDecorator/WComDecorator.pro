#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T12:03:13
#
#-------------------------------------------------

DESTDIR         = ../../manetviz/plugins
QT       += widgets concurrent

TARGET = WComDecorator
TEMPLATE = lib

INCLUDEPATH    += ../../manetviz

DEFINES += WCOMDECORATOR_LIBRARY

SOURCES += wcomdecorator.cpp

HEADERS += wcomdecorator.h\
        wcomdecorator_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    control.ui

DISTFILES += \
    wcomdecorator.json

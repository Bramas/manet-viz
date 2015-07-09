#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T17:09:45
#
#-------------------------------------------------

DESTDIR         = ../../manetviz/plugins
QT             += widgets concurrent

TARGET = WirelessCommunicationDecorator
TEMPLATE = lib
INCLUDEPATH    += ../../manetviz

DEFINES += WIRELESSCOMMUNICATIONDECORATOR_LIBRARY

SOURCES += wirelesscommunicationdecorator.cpp

HEADERS += wirelesscommunicationdecorator.h\
        wirelesscommunicationdecorator_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    wirelesscommunicationdecorator.json

FORMS += control.ui

#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T16:35:01
#
#-------------------------------------------------

CONFIG += c++11
QT       += widgets
DESTDIR         = ../../manetviz/plugins


TARGET = InterpolationDecorator
TEMPLATE = lib
INCLUDEPATH    += ../../manetviz

DEFINES += INTERPOLATIONDECORATOR_LIBRARY

SOURCES += interpolationdecorator.cpp

HEADERS += interpolationdecorator.h\
        interpolationdecorator_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    interpolationdecorator.json

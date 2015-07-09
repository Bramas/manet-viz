#-------------------------------------------------
#
# Project created by QtCreator 2015-06-22T12:31:26
#
#-------------------------------------------------

DESTDIR         = ../../manetviz/plugins

QT +=  concurrent

TARGET = LatLngCoordinatesLayout
TEMPLATE = lib
INCLUDEPATH    += ../../manetviz

DEFINES += LATLNGCOORDINATESLAYOUT_LIBRARY

SOURCES += latlngcoordinateslayout.cpp

HEADERS += latlngcoordinateslayout.h\
        latlngcoordinateslayout_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    latlngcoordinateslayout.json


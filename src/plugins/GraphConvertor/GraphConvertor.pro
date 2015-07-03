#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T16:42:18
#
#-------------------------------------------------

QT       += widgets

TARGET = GraphConvertor
TEMPLATE = lib
INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

DEFINES += GRAPHCONVERTOR_LIBRARY

SOURCES += graphconvertor.cpp

HEADERS += graphconvertor.h\
        graphconvertor_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    graphconvertor.json

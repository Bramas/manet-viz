#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T16:02:12
#
#-------------------------------------------------

QT       += widgets

TARGET = StaticNodes
TEMPLATE = lib
DESTDIR         = ../../manetviz/plugins
INCLUDEPATH    += ../../manetviz

DEFINES += STATICNODES_LIBRARY

SOURCES += staticnodes.cpp

HEADERS += staticnodes.h\
        staticnodes_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    staticnodes.json

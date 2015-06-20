TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../manetviz
TARGET          = $$qtLibraryTarget(graphlayer)
DESTDIR         = ../../manetviz/plugins

HEADERS += \
    graphlayer.h

SOURCES += \
    graphlayer.cpp

DISTFILES += \
    graphlayer.json


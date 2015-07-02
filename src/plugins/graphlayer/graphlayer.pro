TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins
TARGET          = Graphlayer

HEADERS += \
    graphlayer.h

SOURCES += \
    graphlayer.cpp

DISTFILES += \
    graphlayer.json

FORMS += \
    graphlayercontrol.ui


#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T11:25:07
#
#-------------------------------------------------

QT       += widgets concurrent

TARGET = Player
TEMPLATE = lib

INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

DEFINES += PLAYER_LIBRARY

SOURCES += player.cpp

HEADERS += player.h\
        player_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    player.json

FORMS += \
    control.ui

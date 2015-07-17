#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T11:25:07
#
#-------------------------------------------------

QT       += widgets

TARGET = Player
TEMPLATE = lib

DEFINES += PLAYER_LIBRARY

SOURCES += player.cpp

HEADERS += player.h\
        player_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

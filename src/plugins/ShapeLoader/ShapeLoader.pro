#-------------------------------------------------
#
# Project created by QtCreator 2015-07-03T10:40:35
#
#-------------------------------------------------

QT       += widgets

TARGET = ShapeLoader
TEMPLATE = lib

DEFINES += SHAPELOADER_LIBRARY

SOURCES += shapeloader.cpp

HEADERS += shapeloader.h\
        shapeloader_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

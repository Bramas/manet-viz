#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T15:07:07
#
#-------------------------------------------------

QT       += widgets

TARGET = ContactExporter
TEMPLATE = lib

DEFINES += CONTACTEXPORTER_LIBRARY

SOURCES += contactexporter.cpp

HEADERS += contactexporter.h\
        contactexporter_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T15:07:07
#
#-------------------------------------------------

QT       += widgets concurrent

INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

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

DISTFILES += \
    contactexporter.json

unix|win32: LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/lib/ -lgdal.1

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/include

unix|win32: LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/lib/ -lproj.9

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/include


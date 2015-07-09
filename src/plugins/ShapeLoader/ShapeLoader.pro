#-------------------------------------------------
#
# Project created by QtCreator 2015-07-03T10:40:35
#
#-------------------------------------------------

QT       += widgets concurrent

TARGET = ShapeLoader
TEMPLATE = lib

INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins

DEFINES += SHAPELOADER_LIBRARY

SOURCES += shapeloader.cpp

HEADERS += shapeloader.h\
        shapeloader_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    shapeloader.json

unix|win32: LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/lib/ -lproj.9

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Cellar/proj/4.9.1/include


unix|win32: LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/geos/3.4.2/lib/ -lgeos-3.4.2

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Cellar/geos/3.4.2/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Cellar/geos/3.4.2/include

FORMS += \
    control.ui

unix|win32: LIBS += -L$$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/lib/ -lgdal.1

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Cellar/gdal/1.11.2_1/include

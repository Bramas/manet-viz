#-------------------------------------------------
#
# Project created by QtCreator 2015-06-11T21:54:04
#
#-------------------------------------------------

QT       += core gui concurrent opengl

#QMAKE_CXXFLAGS += -I/usr/local/Cellar/geos/3.4.2/include -I/Library/Frameworks/PROJ.framework/Versions/4/unix/include
#QMAKE_LFLAGS += -L/usr/local/Cellar/geos/3.4.2/lib -lgeos -L/Library/Frameworks/PROJ.framework/Versions/4/unix/lib -lproj


LIBS += -L/usr/local/lib -lproj -lgeos
INCLUDEPATH += /usr/local/include

CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = manet-viz
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    evolvinggraph.cpp \
    abstractevolvinggraph.cpp \
    graph.cpp \
    node.cpp \
    controlwidget.cpp \
    comboboxdelegate.cpp \
    viewer.cpp \
    importdialog.cpp \
    csvparser.cpp \
    graphloader.cpp \
    graphicsview.cpp \
    pluginmanager.cpp \
    gtfsloader.cpp \
    gtfsdialog.cpp \
    project.cpp

HEADERS  += mainwindow.h \
    graphicsview.h \
    evolvinggraph.h \
    abstractevolvinggraph.h \
    graph.h \
    types.h \
    node.h \
    controlwidget.h \
    comboboxdelegate.h \
    viewer.h \
    importdialog.h \
    csvparser.h \
    graphloader.h \
    igraphlayout.h \
    pluginmanager.h \
    igraphlayout.h \
    gtfsloader.h \
    graphicsitems.h \
    iloader.h \
    gtfsdialog.h \
    project.h \
    iplugin.h

FORMS    += mainwindow.ui \
    controlwidget.ui \
    importdialog.ui \
    gtfsdialog.ui

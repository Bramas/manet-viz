#-------------------------------------------------
#
# Project created by QtCreator 2015-06-11T21:54:04
#
#-------------------------------------------------

QT       += core gui concurrent opengl
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
    gtfsloader.cpp

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
    iviewerlayer.h \
    igraphlayout.h \
    pluginmanager.h \
    igraphdecorator.h \
    igraphlayout.h \
    gtfsloader.h

FORMS    += mainwindow.ui \
    controlwidget.ui \
    importdialog.ui

LIBS += -L/usr/local/lib -lproj

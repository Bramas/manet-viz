#-------------------------------------------------
#
# Project created by QtCreator 2015-06-11T21:54:04
#
#-------------------------------------------------

QT       += core gui concurrent
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = manet-viz
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    evolvinggraph.cpp \
    geometricgraph.cpp \
    abstractevolvinggraph.cpp \
    graph.cpp \
    node.cpp \
    controlwidget.cpp \
    abstractlayer.cpp \
    viewer.cpp \
    graphlayer.cpp

HEADERS  += mainwindow.h \
    evolvinggraph.h \
    geometricgraph.h \
    abstractevolvinggraph.h \
    graph.h \
    types.h \
    node.h \
    controlwidget.h \
    abstractlayer.h \
    viewer.h \
    graphlayer.h

FORMS    += mainwindow.ui \
    controlwidget.ui

#-------------------------------------------------
#
# Project created by QtCreator 2015-06-11T21:54:04
#
#-------------------------------------------------

QT       += core gui

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
    graphviewer.cpp \
    controlwidget.cpp

HEADERS  += mainwindow.h \
    evolvinggraph.h \
    geometricgraph.h \
    abstractevolvinggraph.h \
    graph.h \
    types.h \
    node.h \
    graphviewer.h \
    controlwidget.h

FORMS    += mainwindow.ui \
    controlwidget.ui

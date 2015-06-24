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
<<<<<<< HEAD
    gtfsloader.cpp
=======
    pluginmanager.cpp
>>>>>>> e2a8cb4f6b18d90cba7fe9de22c77e44fba5579d

HEADERS  += mainwindow.h \
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
    graphicsview.h \
    igraphlayout.h \
<<<<<<< HEAD
    gtfsloader.h
=======
    pluginmanager.h \
    igraphdecorator.h
>>>>>>> e2a8cb4f6b18d90cba7fe9de22c77e44fba5579d

FORMS    += mainwindow.ui \
    controlwidget.ui \
    importdialog.ui

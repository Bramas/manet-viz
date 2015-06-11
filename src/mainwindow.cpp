#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>

#include "graphviewer.h"
#include "geometricgraph.h"
#include "controlwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GeometricGraph * gg = new GeometricGraph();
    if(gg->load("E:/Projects/build/taxi_february.txt"))
    {
        ControlWidget * controlWidget = new ControlWidget();

        GraphViewer * gw = new GraphViewer(gg);
        connect(controlWidget, SIGNAL(timeChanged(mvtime)), gw, SLOT(setTime(mvtime)));
        setCentralWidget(gw);
        this->addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
    }
    else
    {
        delete gg;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

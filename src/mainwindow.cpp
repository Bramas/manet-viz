#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QCloseEvent>

#include "graphviewer.h"
#include "geometricgraph.h"
#include "controlwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _evolvingGraph = new GeometricGraph();
    if(_evolvingGraph->load("/Users/quentinbramas/Projects/data-aggregation-strategies/datasets/RomeTaxi/taxi_february.txt"))
    {
        ControlWidget * controlWidget = new ControlWidget();

        GraphViewer * gw = new GraphViewer(_evolvingGraph);
        connect(controlWidget, SIGNAL(timeChanged(mvtime)), gw, SLOT(setTime(mvtime)));
        setCentralWidget(gw);
        this->addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
    }
    else
    {
        delete _evolvingGraph;
        _evolvingGraph = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent * e)
{
    if(_evolvingGraph)
    {
        _evolvingGraph->cancelLoadAndWait();
        _evolvingGraph->deleteLater();
    }
    e->accept();
}

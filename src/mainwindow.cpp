#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QCloseEvent>
#include <QFileDialog>

#include "viewer.h"
#include "geometricgraph.h"
#include "graphlayer.h"
#include "controlwidget.h"
#include "importdialog.h"
#include "graphloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
}

void MainWindow::open()
{

    QSettings settings;
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open a trace",
                                                    settings.value("defaultTracePath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    tr("Trace file (*.csv *.txt);;All files (*.*)"));
    ImportDialog importDialog(filename);
    int ret = importDialog.exec(); // synchronous
    if (ret == QDialog::Rejected) {
        return;
    }
    // Save the filename path in the app settings
    settings.setValue("defaultTracePath", QFileInfo(filename).absolutePath());

    _graphLoader = new GraphLoader(importDialog.createGraphLoader());
    _graphLoader->load();

    ControlWidget * controlWidget = new ControlWidget();

    Viewer * gw = new Viewer(this);
    const AbstractEvolvingGraph * evg = _graphLoader->constEvolvingGraph();
    IViewerLayer * gl = new GraphLayer(gw, evg);
    gw->addLayer(gl);

    connect(_graphLoader, &GraphLoader::onLoadProgressChanged, controlWidget, &ControlWidget::setLoadProgress);
    connect(controlWidget, SIGNAL(timeChanged(mvtime)), gw, SLOT(setTime(mvtime)));
    setCentralWidget(gw);
    connect(controlWidget, &ControlWidget::communicationRangeChanged, dynamic_cast<const GeometricGraph*>(evg), &GeometricGraph::setCommunicationRange);
    connect(controlWidget, &ControlWidget::contactDisplayChanged, dynamic_cast<GraphLayer*>(gl), &GraphLayer::setDisplayContact);
    connect(controlWidget, &ControlWidget::rangeDisplayChanged, dynamic_cast<GraphLayer*>(gl), &GraphLayer::setDisplayRange);
    this->addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent * e)
{
    if(_graphLoader)
    {
        _graphLoader->cancelLoadAndWait();
        _graphLoader->deleteLater();
    }
    e->accept();
}

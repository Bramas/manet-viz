#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGLWidget>

#include "graphicsview.h"
#include "viewer.h"
#include "controlwidget.h"
#include "importdialog.h"
#include "graphloader.h"
#include "gtfsloader.h"
#include "gtfsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionOpen_GTFS, SIGNAL(triggered()), this, SLOT(openGTFS()));
    _graphLoader = 0;
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

    const AbstractEvolvingGraph * evg = _graphLoader->constEvolvingGraph();

    Viewer * viewer = new Viewer(evg);
    GraphicsView * v = new GraphicsView(viewer, this);

    controlWidget->setViewer(viewer);

    //to enable OpenGL rendering
    //v->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    v->scale(10,10);
    connect(_graphLoader, &GraphLoader::onLoadProgressChanged, controlWidget, &ControlWidget::setLoadProgress);

    setCentralWidget(v);
    this->addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

void MainWindow::openGTFS()
{
    QSettings settings;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open GTFS Directory"),
                                                    settings.value("defaultGTFSDiretory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    GTFSDialog importDialog(dir);
    int ret = importDialog.exec(); // synchronous
    if (ret == QDialog::Rejected) {
        return;
    }

    // Save the directory path in the app settings
    settings.setValue("defaultGTFSDiretory", dir);

    _gtfsLoader = new GTFSLoader(importDialog.createGTFSLoader());
    _gtfsLoader->load();

    ControlWidget * controlWidget = new ControlWidget();

    AbstractEvolvingGraph * evg = _gtfsLoader->evolvingGraph();
    Viewer * viewer = new Viewer(evg);
    GraphicsView * v = new GraphicsView(viewer, this);
    v->scale(10,10);
    controlWidget->setViewer(viewer);
    connect(controlWidget, SIGNAL(timeChanged(mvtime)), viewer, SLOT(setTime(mvtime)));
    setCentralWidget(v);
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

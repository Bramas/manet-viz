#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGLWidget>

#include "viewer.h"
#include "controlwidget.h"
#include "importdialog.h"
#include "graphloader.h"
#include "graphicsview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
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

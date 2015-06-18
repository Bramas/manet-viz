#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QCloseEvent>
#include <QFileDialog>

#include "viewer.h"
#include "geometricgraph.h"
#include "graphlayer.h"
#include "controlwidget.h"
# include "importdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));

}

void MainWindow::open()
{
    _evolvingGraph = new GeometricGraph();

    QSettings settings;
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open a trace",
                                                    settings.value("defaultTracePath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    tr("Trace file (*.csv *.txt);;All files (*.*)"));
    if(_evolvingGraph->load(filename))
    {
        ImportDialog importDialog(filename);
        int ret = importDialog.exec(); // synchronous
        if (ret == QDialog::Rejected) {
            return;
        }
        // Save the filename path in the app settings
        settings.setValue("defaultTracePath", (QFileInfo(filename).absolutePath()));

        ControlWidget * controlWidget = new ControlWidget();

        Viewer * gw = new Viewer(this);
        gw->addLayer(new GraphLayer(gw, _evolvingGraph));
        connect(_evolvingGraph, &AbstractEvolvingGraph::onLoadProgressChanged, controlWidget, &ControlWidget::setLoadProgress);
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

#include "controlwidget.h"
#include "ui_controlwidget.h"
#include "viewer.h"
#include "iplugin.h"
#include "project.h"

#include <QSpacerItem>

ControlWidget::ControlWidget(QWidget *parent, Project * project) :
    QDockWidget(parent),
    ui(new Ui::ControlWidget)
{
    _project = project;
    ui->setupUi(this);
    ui->dockWidgetContents->setLayout(ui->verticalLayout);

    _viewer = 0;
}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::setLoadProgress(qreal f)
{
    ui->progressBarImport->setValue(f*1000);
}

void ControlWidget::setViewer(Viewer *viewer)
{
    _viewer = viewer;

    foreach(IPlugin * layer, _project->layers())
    {
        QWidget * w = layer->createControlWidget();
        if(w)
        {
            ui->verticalLayout->addWidget(w);
            qDebug()<<"add control for "<<layer->getQObject()->objectName();
        }
    }
    //add vertical Spacer
    ui->verticalLayout->addSpacerItem(new QSpacerItem(20,20,QSizePolicy::Minimum, QSizePolicy::Expanding));
}

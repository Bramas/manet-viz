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
    connect(ui->pushButtonPlus, SIGNAL(pressed()), this, SLOT(increaseZoom()));
    connect(ui->pushButtonMinus, SIGNAL(pressed()), this, SLOT(decreaseZoom()));
    connect(ui->pushButtonReset, SIGNAL(pressed()), this, SLOT(resetZoom()));

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

QPointF ControlWidget::getWeightedCenter()
{
    QPointF centroid(0,0);
    int nrofPoints = 0;
    for(auto item : _project->viewer()->items()) {
        if(item->type() == QGraphicsItem::UserType + 2) {
            centroid += item->pos();
            nrofPoints++;
        }
    }
    centroid /= nrofPoints;
    return centroid;
}

void ControlWidget::increaseZoom()
{
    _project->viewer()->views().front()->scale(2,2);
}

void ControlWidget::decreaseZoom()
{
    _project->viewer()->views().front()->scale(0.5,0.5);
}

void ControlWidget::resetZoom()
{
    QRectF rect(_project->viewer()->sceneRect());
    _project->viewer()->views().front()->fitInView(rect);
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

//    QPainter p(viewer->views().front()->viewport());
//    p.setRenderHints(viewer->views().front()->renderHints());
//    p.drawRect(0,0,50,50);
//    viewer->views().front()->render(&p, viewer->views().front()->viewport()->rect());

}

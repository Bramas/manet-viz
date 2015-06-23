#include "viewer.h"
#include "iviewerlayer.h"
#include "igraphlayout.h"
#include "pluginmanager.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QGLWidget>

Viewer::Viewer(const AbstractEvolvingGraph * evg)
{
    _afterTranslate = QPointF(-83000, -12.35*2000);
    _layout = 0;
    _zoom=2000;
    //setMouseTracking(true);
    _timeSinceLastFrame.start();
    _evolvingGraph = evg;
    connect(&PluginManager::instance, SIGNAL(pluginsChanged()), this, SLOT(onPluginsChanged()));
    onPluginsChanged();
}

Viewer::~Viewer()
{

}


QPointF Viewer::toLocalCoordinates(QPointF globalCoordinates) const
{
    return _afterTranslate + _zoom * (globalCoordinates);
}

void Viewer::setTime(mvtime time)
{
    _time = time;
    updateLayers();
}
void Viewer::updateLayers()
{
    if(!_layout)
        return;

    IGraph * graph = new Graph();
    _layout->footprint(_time, graph);
    foreach(auto layer, _layers)
    {
        layer->paint(graph);
    }

    update();
}

/*
void Viewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawText(QPoint(0,50), QString::number(1000.0/_timeSinceLastFrame.elapsed()));
    _timeSinceLastFrame.start();
    for(qreal x = 41; x < 43; x += 0.1)
    {
        qreal localX = toLocalCoordinates(QPointF(x,0)).x();
        painter.drawLine(localX, 0, localX, height());
    }
    for(qreal y = 12; y < 14; y += 0.1)
    {
        qreal localY = toLocalCoordinates(QPointF(0, y)).y();
        painter.drawLine(0, localY, width(), localY);
    }


    foreach(auto layer, _layers)
    {
        painter.save();
        layer->paint(&painter);
        painter.restore();
    }
}

void Viewer::wheelEvent(QWheelEvent * e)
{

    qreal factor = (1+e->delta()/200.0);
    factor = qMin(factor,1.1);
    factor = qMax(factor, 0.9);

    _zoom *= factor;
    _afterTranslate -= e->pos();
    _afterTranslate *= factor;
    _afterTranslate += e->pos();

    update();
}

void Viewer::mouseMoveEvent(QMouseEvent * e)
{
    //qDebug()<<e->buttons();
    if(e->buttons() == Qt::LeftButton)
    {
        _afterTranslate += e->pos() - _lastMousePos;
        _lastMousePos = e->pos();
        update();
    }
}

void Viewer::mousePressEvent(QMouseEvent * e)
{
    _lastMousePos = e->pos();
}

*/

void Viewer::addLayer(IViewerLayer *layer, int priority)
{
    QObject::connect(layer->getQObject(), SIGNAL(requestUpdate()), this, SLOT(updateLayers()));
    _layers.insertMulti(priority, layer);
}

void Viewer::onPluginsChanged()
{
    foreach(IViewerLayer * layer, PluginManager::getObjects<IViewerLayer>())
    {
        layer->setEvolvingGraph(_evolvingGraph);
        layer->setGraphicsScene(this);
        addLayer(layer);
        continue;
    }

    foreach(IGraphLayout * layout, PluginManager::getObjects<IGraphLayout>())
    {
         layout->setEvolvingGraph(_evolvingGraph);
         _layout = layout;
         break;
    }
}

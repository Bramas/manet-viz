#include "viewer.h"
#include "iviewerlayer.h"
#include "igraphlayout.h"
#include "igraphdecorator.h"
#include "pluginmanager.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QGLWidget>
#include <QGraphicsItem>
#include <QPointF>
#include <QDebug>

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
    _time = time + _evolvingGraph->beginTime();
    updateLayers();
}
void Viewer::updateLayers()
{
    if(!_layout)
        return;

    IGraph * graph = new Graph();


    foreach(auto decorator, _graphDecorators)
    {
        decorator->decorateNodes(_time, graph);
    }
    foreach(auto decorator, _graphDecorators)
    {
        decorator->decorateEdges(_time, graph);
    }

    _layout->footprint(_time, graph);

//    this->clear();

    if(this->items().contains(_items)) {
        this->removeItem(_items);
        delete _items;
    }

    _items = new QGraphicsItemGroup();

    // set pen to display edges
    QPen p(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);

    foreach(const Node& n1, graph->nodes())
    {
        foreach(int n2, n1.neighbors())
        {
            if(n1.id() < n2)
            {
                QPointF p1(n1.properties().value(X).toDouble(), n1.properties().value(Y).toDouble());
                QPointF p2(graph->nodes().value(n2).properties().value(X).toDouble(), graph->nodes().value(n2).properties().value(Y).toDouble());
                QPointF middle = (p1 + p2) / 2.0;
                GraphicsEdgeItem * edge = new GraphicsEdgeItem(p1 - middle, p2 - middle);
                edge->setPen(p);
                edge->setPos(middle);

                foreach(auto decorator, _graphDecorators)
                {
                    decorator->decoratesGraphicsEdge(edge);
                }
                foreach(auto layer, _layers)
                {
                    layer->decoratesGraphicsEdge(edge);
                }

                _items->addToGroup(edge);
            }
        }
    }

    // Set pen to dsiplay nodes
    p.setColor(QColor(0,0,0));
    p.setWidthF(4);

    foreach(const Node &n, graph->nodes())
    {
        QPointF position(n.properties().value(X).toDouble(), n.properties().value(Y).toDouble());
        GraphicsNodeItem * node = new GraphicsNodeItem();
        node->setPen(p);
        node->setPos(position);
        foreach(auto decorator, _graphDecorators)
        {
            decorator->decoratesGraphicsNode(node);
        }
        foreach(auto layer, _layers)
        {
            layer->decoratesGraphicsNode(node);
        }
        _items->addToGroup(node);
    }
    this->addItem(_items);
    update();

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
    connect(layer->getQObject(), SIGNAL(requestUpdate()), this, SLOT(updateLayers()));
    _layers.insertMulti(priority, layer);
}

void Viewer::onPluginsChanged()
{
    foreach(IViewerLayer * layer, PluginManager::getObjects<IViewerLayer>())
    {
        layer->setEvolvingGraph(_evolvingGraph);
        layer->setGraphicsScene(this);
        addLayer(layer);
        qDebug() << "layer" << layer->toString();
   }

    foreach(IGraphLayout * layout, PluginManager::getObjects<IGraphLayout>())
    {
         layout->setEvolvingGraph(_evolvingGraph);
         _layout = layout;
         break;
    }
    foreach(IGraphDecorator * decorator, PluginManager::getObjects<IGraphDecorator>())
    {
         decorator->setEvolvingGraph(_evolvingGraph);
         _graphDecorators.insertMulti(100, decorator);
         qDebug() << "decorator" << decorator->toString();
         connect(decorator->getQObject(), SIGNAL(requestUpdate()), this, SLOT(updateLayers()));
    }
}

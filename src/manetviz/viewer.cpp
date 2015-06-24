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


class GraphicsNodeItem : public QGraphicsItem
{
public:

    GraphicsNodeItem(QPointF position) : _position(position) { }
    GraphicsNodeItem() : _position(QPointF(0,0)) { }
    QRectF boundingRect() const
    {
        return QRectF(_position.x()-0.00001, _position.y()-0.00001,0.00002,0.00002);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QTransform t = painter->transform().inverted();
        QPointF point = painter->transform().map(_position);
        QPointF offset(_pen.widthF()/2.0,_pen.widthF()/2.0);
        QRectF r(t.map(point - offset), t.map(point+offset));
        painter->setPen(Qt::NoPen);
        painter->setBrush(_pen.brush());
        painter->drawEllipse(r);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _position;
};

class GraphicsEdgeItem : public QGraphicsItem
{
public:

    GraphicsEdgeItem(QPointF p1, QPointF p2) : _p1(p1), _p2(p2) { }
    QRectF boundingRect() const
    {
        QPointF topLeft(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y()));
        QPointF bottomRight(qMax(_p1.x(), _p2.x()), qMax(_p1.y(), _p2.y()));
        return QRectF(topLeft, bottomRight);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        //painter->setRenderHint(QPainter::Antialiasing);
        QLineF line(_p1, _p2);
        painter->setPen(_pen);
        painter->drawLine(line);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _p1;
    QPointF _p2;
};



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


    this->clear();


    QGraphicsItemGroup * items = new QGraphicsItemGroup();
    QPen p(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);

    foreach(const Node& n1, graph->nodes())
    {
        foreach(int n2, n1.neighbors())
        {
            if(n1.id() < n2)
            {
                QPointF p1(n1.properties().value("X").toDouble(), n1.properties().value("Y").toDouble());
                QPointF p2(graph->nodes().value(n2).properties().value("X").toDouble(), graph->nodes().value(n2).properties().value("Y").toDouble());
                GraphicsEdgeItem * line = new GraphicsEdgeItem(p1, p2);
                line->setPen(p);
                items->addToGroup((QGraphicsItem*) line);
            }
        }
    }
    p.setColor(QColor(0,0,0));
    p.setWidthF(4);

    foreach(const Node &n, graph->nodes())
    {
        QPointF position(n.properties().value("X").toDouble(), n.properties().value("Y").toDouble());
        GraphicsNodeItem * i = new GraphicsNodeItem();
        i->setPen(p);
        i->setPos(position);
        foreach(auto decorator, _graphDecorators)
        {
            QGraphicsItem * c = decorator->graphicsNodeChildItem(n);
            if(c)
            {
                c->setParentItem(i);
            }

        }
        items->addToGroup((QGraphicsItem*)i);
    }
    this->addItem(items);
    update();
    return;

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
        continue;
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
         connect(decorator->getQObject(), SIGNAL(requestUpdate()), this, SLOT(updateLayers()));
    }
}

#include "graphviewer.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>

GraphViewer::GraphViewer(const AbstractEvolvingGraph *graph) :
    _evolvingGraph(graph),
    _time(0)
{
    _beforeTranslate = QPointF(0,0);//QPointF(-41.55, -12.35);
    _afterTranslate = QPointF(-83000, -12.35*2000);
    _zoom=2000;
    connect(_evolvingGraph, &AbstractEvolvingGraph::onLoadProgressChanged, [this](qreal) { this->update(); });

    setMouseTracking(true);
}

QPointF GraphViewer::toLocalCoordinates(QPointF globalCoordinates) const
{
    return _afterTranslate + _zoom * (globalCoordinates + _beforeTranslate);
}

void GraphViewer::setTime(mvtime time)
{
    _time = time;
    update();
}

void GraphViewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    Graph g = _evolvingGraph->footprint(_time);
    painter.drawText(QPoint(0,50), QString::number(_evolvingGraph->loadProgress())+"% loaded");


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

    painter.setBrush(QBrush(QColor(0,0,0)));
    foreach(const Node& n, g.nodes())
    {
        painter.drawEllipse(toLocalCoordinates(n.position()), 1,1);

        painter.save();
        painter.setPen(QPen(QColor(0,0,0,100)));
        painter.drawText(toLocalCoordinates(n.position())+QPointF(2,-2), QString::number(n.id()));
        painter.restore();
    }
}

void GraphViewer::wheelEvent(QWheelEvent * e)
{

    qreal factor = (1+e->delta()/200.0);
    factor = qMin(factor,1.1);
    factor = qMax(factor, 0.9);
    _zoom *= factor;
    //qDebug()<<_zoom<<factor;
    //qDebug()<<_afterTranslate;
    _afterTranslate -= e->pos();
    _afterTranslate *= factor;
    _afterTranslate += e->pos();
    //qDebug()<<_afterTranslate;
    //_afterTranslate -= e->pos();
    //_afterTranslate *= factor;
    //_afterTranslate += e->pos();
    //_afterTranslate += (-e->pos() + _afterTranslate) * factor;
    update();
}

void GraphViewer::mouseMoveEvent(QMouseEvent * e)
{
    //qDebug()<<e->buttons();
    if(e->buttons() == Qt::LeftButton)
    {
        _afterTranslate += e->pos() - _lastMousePos;
        _lastMousePos = e->pos();
        update();
    }
}

void GraphViewer::mousePressEvent(QMouseEvent * e)
{
    _lastMousePos = e->pos();
}

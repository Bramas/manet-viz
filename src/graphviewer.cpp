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

    QPointF mapPos(41.900272,12.490888);
    //mapPos //144447.644200
    qreal metersPerPixel = 156543.03392 * cos(41.900272 * M_PI / 180.0) / pow(2, 13);
    //qDebug()<<metersPerPixel;
    QImage map("/Users/quentinbramas/Downloads/staticmap-roma-41.900272,12.490888.png");
    QPointF topLeft = toLocalCoordinates(QPointF(41.858369,12.4373972));
    QPointF bottomRight = toLocalCoordinates(QPointF(41.9609321,12.5430271));
    QRectF imageBounds(topLeft, bottomRight);
    //painter.drawImage(toLocalCoordinates(mapPos) - QPointF(map.width(), map.height())/2.0, map, );
    painter.drawImage(imageBounds, map);

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
    foreach(const Node& n1, g.nodes())
    {
        foreach(const Node& n2, n1.neighbors())
        {
            painter.drawLine(toLocalCoordinates(n1.position()), toLocalCoordinates(n2.position()));
        }
    }

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

#include "graphviewer.h"

#include <QPainter>

GraphViewer::GraphViewer(const AbstractEvolvingGraph *graph) :
    _evolvingGraph(graph),
    _time(0)
{

}

QPointF GraphViewer::toLocalCoordinates(QPointF globalCoordinates) const
{
    QPointF translate(-41.55, -12.35);
    qreal zoom=2000;
    return zoom*(globalCoordinates+translate);
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

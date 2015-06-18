#include "graphlayer.h"
#include "viewer.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>


GraphLayer::GraphLayer(Viewer *viewer, const AbstractEvolvingGraph * graph) : AbstractLayer(viewer), _evolvingGraph(graph)
{

}

void GraphLayer::paint(QPainter * painter)
{
    Graph g = _evolvingGraph->footprint(_parent->time());
    painter->setBrush(QBrush(QColor(0,0,0)));

    foreach(const Node& n1, g.nodes())
    {
        foreach(const Node& n2, n1.neighbors())
        {
            painter->drawLine(_parent->toLocalCoordinates(n1.position()), _parent->toLocalCoordinates(n2.position()));
        }
    }
    foreach(const Node& n, g.nodes())
    {
        painter->drawEllipse(_parent->toLocalCoordinates(n.position()), 1,1);

        painter->save();
        painter->setPen(QPen(QColor(0,0,0,100)));
        painter->drawText(_parent->toLocalCoordinates(n.position())+QPointF(2,-2), QString::number(n.id()));
        painter->restore();
    }
}

LoaderLayer::LoaderLayer(Viewer *viewer, const AbstractEvolvingGraph *graph) : AbstractLayer(viewer), _evolvingGraph(graph)
{
    QObject::connect(_evolvingGraph, &AbstractEvolvingGraph::onLoadProgressChanged, [this](qreal) { _parent->update(); });
}

void LoaderLayer::paint(QPainter * painter)
{
    painter->setPen(QPen(QColor(255,0,0)));
    painter->drawText(QPoint(0,50), QString::number(_evolvingGraph->loadProgress())+"% loaded");
}



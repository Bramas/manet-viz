#include "graphlayer.h"
#include "viewer.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>


GraphLayer::GraphLayer(Viewer *viewer, const AbstractEvolvingGraph * graph) : IViewerLayer(viewer), _evolvingGraph(graph)
{

}

void GraphLayer::paint(QPainter * painter)
{
    Graph g = _evolvingGraph->footprint(_evolvingGraph->beginTime() + _parent->time());
    painter->setBrush(QBrush(QColor(0,0,0)));

    if(_displayContacts) {
        foreach(const Node& n1, g.nodes())
        {
            foreach(const Node& n2, n1.neighbors())
            {
                painter->drawLine(_parent->toLocalCoordinates(n1.position()), _parent->toLocalCoordinates(n2.position()));
            }
        }
    }
    foreach(const Node& n, g.nodes())
    {
        QPointF nodeCoords = _parent->toLocalCoordinates(n.position());
        painter->drawEllipse(nodeCoords, 1,1);

        if(_displayRange) {
            qreal range = _evolvingGraph->getCommunicationRange();
            painter->drawEllipse(nodeCoords, range, range);
        }

        painter->save();
        painter->setPen(QPen(QColor(0,0,0,100)));
        painter->drawText(_parent->toLocalCoordinates(n.position())+QPointF(2,-2), QString::number(n.id()));
        painter->restore();
    }
}

void GraphLayer::setDisplayRange(bool state)
{
    _displayRange = state;
}

void GraphLayer::setDisplayContact(bool state)
{
    _displayContacts = state;
}

LoaderLayer::LoaderLayer(Viewer *viewer, const AbstractEvolvingGraph *graph) : IViewerLayer(viewer), _evolvingGraph(graph)
{
    QObject::connect(_evolvingGraph, &AbstractEvolvingGraph::onLoadProgressChanged, [this](qreal) { _parent->update(); });
}

void LoaderLayer::paint(QPainter * painter)
{
    painter->setPen(QPen(QColor(255,0,0)));
    painter->drawText(QPoint(0,50), QString::number(_evolvingGraph->loadProgress())+"% loaded");
}



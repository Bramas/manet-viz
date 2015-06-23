#include "latlngcoordinateslayout.h"
#include "node.h"

#include <QVector2D>

LatLngCoordinatesLayout::LatLngCoordinatesLayout()
{
}


void LatLngCoordinatesLayout::footprint(mvtime time, IGraph * graph) const
{
    /*
    foreach(int id, _evolvingGraph->nodes().keys())
    {
        auto it = _evolvingGraph->nodes().value(id)->find(time);
        if(it != _evolvingGraph->nodes().value(id)->end())
        {
            graph->addNode(id, it.value());
        }
    }*/

  /*  foreach(int id, _evolvingGraph->nodes().keys())
    {
        auto nodeTimeline = _evolvingGraph->nodes().value(id);
        QMap<mvtime, QHash<QString, QVariant> >::const_iterator up = nodeTimeline->lowerBound(time);
        if(up == nodeTimeline->constEnd())
        {
            continue;
        }
        QPointF upPoint(up.value().value("X").toDouble(), up.value().value("Y").toDouble());
        if(up != nodeTimeline->constBegin() && up.key() - (up - 1).key() < 300)
        {
            QMap<mvtime, QHash<QString, QVariant> >::const_iterator low = up - 1;
            QPointF lowPoint(low.value().value("X").toDouble(), low.value().value("Y").toDouble());
            QPointF p = (up.key() - time)*lowPoint + (time - low.key())*upPoint;
            p /= (up.key() - low.key());
            graph->addNode(id, p);
        }
        else if(up.key() == time)
        {
            graph->addNode(id, upPoint);
        }
    }*/
/*
    foreach(const Node& n1, graph->nodes())
    {
        foreach(const Node& n2, graph->nodes())
        {
            QVector2D v1(n1.position());
            if(v1.distanceToPoint(QVector2D(n2.position())) <= 3/1000.0)
            {
                graph->addEdge(n1,n2);
            }
        }
    }*/
}

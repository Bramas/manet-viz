#include "geometricgraph.h"
#include <iostream>
#include <memory>
#include <ctime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVector2D>

GeometricGraph::GeometricGraph()
{
    _loaded = 0;
    _forceStop = false;
    _communicationRange = 1;
    _beginTime = MaxTime;
    _endTime = 0;
}
GeometricGraph::~GeometricGraph(){
    _forceStop = true;
}


typedef QMap<mvtime, QPointF> NodePositions;
Graph GeometricGraph::footprint(mvtime time) const
{
    Graph g;
    foreach(int id, _nodesPosition.keys())
    {
        NodePositions * nodePos = _nodesPosition.value(id);
        NodePositions::const_iterator up = nodePos->lowerBound(time);
        if(up != nodePos->constBegin() && abs(up.key() - (up - 1).key()) < 300)
        {
            NodePositions::const_iterator low = up - 1;
            QPointF p = (up.key() - time)*low.value() + (time - low.key())*up.value();
            p /= (up.key() - low.key());
            g.addNode(id, p);
        }
        else if(up.key() == time)
        {
            g.addNode(id, up.value());
        }
    }

    foreach(const Node& n1, g.nodes())
    {
        foreach(const Node& n2, g.nodes())
        {
            QVector2D v1(n1.position());
            if(v1.distanceToPoint(QVector2D(n2.position())) <= _communicationRange/1000.0)
            {
                g.addEdge(n1,n2);
            }
        }
    }
    return g;
}

void GeometricGraph::setCommunicationRange(qreal range)
{
    _communicationRange = range;
}

void GeometricGraph::addNodePosition(int nodeId, mvtime time, QPointF position)
{
    _beginTime = qMin(_beginTime, time);
    _endTime = qMax(_endTime, time);
    if(!_nodesPosition.contains(nodeId))
    {
        _nodesPosition.insert(nodeId, new QMap<mvtime, QPointF>());
    }
    _nodesPosition.value(nodeId)->insert(_nodesPosition.value(nodeId)->constEnd(), time, position);
}

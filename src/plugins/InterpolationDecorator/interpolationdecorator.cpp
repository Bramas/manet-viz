#include "interpolationdecorator.h"
#include "project.h"
#include "iloader.h"
#include <QDebug>

InterpolationDecorator::InterpolationDecorator()
{
}

void InterpolationDecorator::decorateEdges(mvtime time, IGraph *graph)
{

}

void InterpolationDecorator::decorateNodes(mvtime time, IGraph *graph)
{
    foreach(int id, _project->loader()->constEvolvingGraph()->nodes().keys())
    {
        auto nodeTimeline = _project->loader()->constEvolvingGraph()->nodes().value(id);
        QMap<mvtime, NodeProperties>::const_iterator up = nodeTimeline->lowerBound(time);
        if(up == nodeTimeline->constEnd())
        {
            continue;
        }
        if(up != nodeTimeline->constBegin() && up.key() - (up - 1).key() < 300)
        {
            QPointF upPoint(up.value().value(X).toDouble(), up.value().value(Y).toDouble());
            QMap<mvtime, NodeProperties >::const_iterator low = up - 1;
            QPointF lowPoint(low.value().value(X).toDouble(), low.value().value(Y).toDouble());
            QPointF p = (up.key() - time)*lowPoint + (time - low.key())*upPoint;
            p /= (up.key() - low.key());

            NodeProperties node;
            node.insert(X, p.x());
            node.insert(Y, p.y());
            graph->addNode(id, node);
        }
        else if(up.key() == time)
        {
            QPointF upPoint(up.value().value(X).toDouble(), up.value().value(Y).toDouble());
            NodeProperties node;
            node.insert(X, upPoint.x());
            node.insert(Y, upPoint.y());
            graph->addNode(id, node);
        }
    }
}


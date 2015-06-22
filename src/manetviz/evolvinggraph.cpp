#include "evolvinggraph.h"

EvolvingGraph::EvolvingGraph()
{
    _beginTime = MaxTime;
    _endTime = 0;
}


void EvolvingGraph::addNode(int id, mvtime time, QHash<QString, QVariant> props)
{
    _beginTime = qMin(_beginTime, time);
    _endTime = qMax(_endTime, time);
    if(!_nodes.contains(id))
    {
        _nodes.insert(id, new QMap<mvtime, NodeProperties>());
    }
    _nodes.value(id)->insert(time, props);
}

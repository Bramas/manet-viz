#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

#include <QString>

#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph
{
public:
    AbstractEvolvingGraph();
    virtual ~AbstractEvolvingGraph();

    virtual bool load(QString filename) = 0;

    virtual Graph footprint(mvtime time) const = 0;
};

#endif // ABSTRACTGRAPH_H

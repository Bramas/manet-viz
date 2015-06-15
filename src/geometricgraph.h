#ifndef GEOMETRICGRAPH_H
#define GEOMETRICGRAPH_H

#include "abstractevolvinggraph.h"
#include "types.h"

#include <QMap>
#include <QHash>

class GeometricGraph : public AbstractEvolvingGraph
{

public:
    GeometricGraph();
    ~GeometricGraph();

    bool load(QString filename);

    Graph footprint(mvtime time) const;

private:
    void processLine(QString line);

    QHash<int, QMap<mvtime, QPointF>* > _nodesPosition;

    qreal _loaded;

};

#endif // GEOMETRICGRAPH_H

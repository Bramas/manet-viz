#ifndef GEOMETRICGRAPH_H
#define GEOMETRICGRAPH_H

#include "abstractevolvinggraph.h"
#include "types.h"

#include <QtConcurrent>
#include <QMap>
#include <QHash>

class GeometricGraph : public AbstractEvolvingGraph
{

public:
    GeometricGraph();
    ~GeometricGraph();

    bool load(QString filename);
    qreal loadProgress() const { return _loaded * 100; }
    void cancelLoadAndWait();

    Graph footprint(mvtime time) const;

private:
    bool concurrentLoad(QFile *file);
    void processLine(QString line);

    QHash<int, QMap<mvtime, QPointF>* > _nodesPosition;

    qreal _loaded;
    bool _forceStop;
    QFuture<bool> _loadResult;

};

#endif // GEOMETRICGRAPH_H

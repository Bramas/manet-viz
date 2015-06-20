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


    Graph footprint(mvtime time) const;

    void setCommunicationRange(qreal);

    void addNodePosition(int nodeId, mvtime time, QPointF position);


    mvtime beginTime() const { return _beginTime; }
    mvtime endTime() const { return _endTime; }


private:
    bool concurrentLoad(QFile *file);
    void processLine(QString line);

    QHash<int, QMap<mvtime, QPointF>* > _nodesPosition;

    qreal _loaded;
    bool _forceStop;
    QFuture<bool> _loadResult;
    qreal _communicationRange;

    mvtime _beginTime;
    mvtime _endTime;

};

#endif // GEOMETRICGRAPH_H

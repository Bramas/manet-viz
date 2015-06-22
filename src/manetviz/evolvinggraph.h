#ifndef EVOLVINGGRAPH_H
#define EVOLVINGGRAPH_H

#include <QtCore>

#include "abstractevolvinggraph.h"

class EvolvingGraph : public AbstractEvolvingGraph
{
public:
    EvolvingGraph();

    virtual mvtime beginTime() const { return _beginTime; }
    virtual mvtime endTime() const { return _endTime; }

    virtual const QHash<int, QMap<mvtime, NodeProperties>* > & nodes() const { return _nodes; }
    void addNode(int id, mvtime time, QHash<QString, QVariant> props);
private:

    QHash<int, QMap<mvtime, NodeProperties>* > _nodes;

    mvtime _beginTime;
    mvtime _endTime;
};

#endif // EVOLVINGGRAPH_H

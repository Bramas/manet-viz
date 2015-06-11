#ifndef GRAPH_H
#define GRAPH_H

#include <QVector>

#include "node.h"

class Graph
{
public:
    Graph();

    const QVector<Node> & nodes() { return _nodes; }

    void addNode(int id, QPointF point);

private:
    QVector<Node> _nodes;


};

#endif // GRAPH_H

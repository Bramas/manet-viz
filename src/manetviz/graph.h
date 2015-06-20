#ifndef GRAPH_H
#define GRAPH_H

#include <QHash>

#include "node.h"

class Graph
{
public:
    Graph();

    const QHash<int, Node> & nodes() { return _nodes; }

    void addNode(int id, QPointF point);

    void addEdge(const Node & n1, const Node & n2);

private:
    QHash<int, Node> _nodes;


};

#endif // GRAPH_H

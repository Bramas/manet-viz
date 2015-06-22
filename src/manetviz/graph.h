#ifndef GRAPH_H
#define GRAPH_H

#include <QHash>

#include "node.h"

class IGraph
{
public:
    virtual ~IGraph() { }
    virtual const QHash<int, Node> & nodes() const = 0;
    virtual void addNode(int id, QPointF point) = 0;
    virtual void addEdge(const Node & n1, const Node & n2) = 0;
};


class Graph : public IGraph
{
public:
    Graph();

    const QHash<int, Node> & nodes() const { return _nodes; }

    void addNode(int id, QPointF point);
    void addEdge(const Node & n1, const Node & n2);

private:
    QHash<int, Node> _nodes;


};

#endif // GRAPH_H

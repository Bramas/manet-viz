#include "graph.h"

Graph::Graph()
{

}

void Graph::addNode(int id, QPointF point)
{
    _nodes.insert(id, Node(id, point));
}


void Graph::addEdge(const Node & n1, const Node & n2)
{
    _nodes[n1.id()]._neighbors << n2;
    _nodes[n2.id()]._neighbors << n1;
}

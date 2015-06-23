#include "graph.h"

#include <QDebug>

Graph::Graph()
{

}

void Graph::addNode(int id, NodeProperties properties)
{
    _nodes.insert(id, Node(id, properties));
}


void Graph::addEdge(int n1, int n2)
{
    if(!_nodes.contains(n1) || !_nodes.contains(n2))
    {
        return;
    }
    _nodes[n1]._neighbors << n2;
    _nodes[n2]._neighbors << n1;
}

#include "graph.h"

Graph::Graph()
{

}

void Graph::addNode(int id, QPointF point)
{
    _nodes << Node(id, point);
}

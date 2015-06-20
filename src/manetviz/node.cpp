#include "node.h"

Node::Node() : _id(0), _position(QPoint(0,0))
{

}


Node::Node(int id, QPointF position) : _id(id), _position(position)
{

}


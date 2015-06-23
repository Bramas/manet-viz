#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include <QVector>

class Graph;

class Node
{
public:
    Node() : _id(0), _position(QPoint(0,0)) { }

    const QPointF & position() const { return _position; }
    const QVector<Node> & neighbors() const { return _neighbors; }
    int id() const { return _id; }

private:
    Node(int id, QPointF position) : _id(id), _position(position) { }
    friend class Graph;

    int _id;
    QPointF _position;
    QVector<Node> _neighbors;
};

#endif // NODE_H

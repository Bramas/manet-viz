#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include <QVector>
#include <QHash>
#include <QVariant>


typedef QHash<QString, QVariant> NodeProperties;
class Graph;

class Node
{
public:
    Node() : _id(-1) { }

    const QVector<int> & neighbors() const { return _neighbors; }
    const NodeProperties & properties() const {return _properties; }
    int id() const { return _id; }

private:
    Node(int id, NodeProperties properties) : _id(id), _properties(properties) { }
    friend class Graph;

    int _id;
    NodeProperties _properties;
    QVector<int> _neighbors;
};

#endif // NODE_H

#ifndef STATICNODES_H
#define STATICNODES_H

#include "staticnodes_global.h"
#include <QObject>

#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

class StaticNode
{
public:
    StaticNode():
        _coordinates(QPointF()), _transmissionRange(0.0), _id(idGenerator++) {}
    StaticNode(QPointF coordinates, int transmissionRange):
        _coordinates(coordinates), _transmissionRange(transmissionRange), _id(idGenerator++) {}

    QPointF getCoordinates() const { return _coordinates; }
    int getTransmissionRange() const { return _transmissionRange; }
    int getId() const { return _id; }

private:
    static int idGenerator;
    QPointF _coordinates;
    int _transmissionRange;
    int _id;
};

class GraphicsStaticNodeItem : public QGraphicsItem
{
public:

    GraphicsStaticNodeItem(StaticNode * node) : _node(node) { }
    GraphicsStaticNodeItem() : _node(new StaticNode()) { }
    QRectF boundingRect() const
    {
        double range = (double)_node->getTransmissionRange()+0.1;
        return QRectF(_node->getCoordinates().x()-range, _node->getCoordinates().y()-range,range*2,range*2);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QTransform t = painter->transform().inverted();
        QPointF point = painter->transform().map(_node->getCoordinates());
        QPointF offset(_pen.widthF()/2.0,_pen.widthF()/2.0);
        QRectF r(t.map(point - offset), t.map(point+offset));
        painter->setPen(Qt::NoPen);
        painter->setBrush(_pen.brush());
        painter->drawEllipse(r);

        // draw the communication range
        int range = _node->getTransmissionRange();
        offset = QPointF(range,range);
        r = QRectF(t.map(point - offset), t.map(point+offset));
        QPen p;
        p.setCosmetic(true);
        p.setWidth(2);
        p.setColor(QColor(0,150,0));
        painter->setPen(p);
        painter->setBrush(QBrush(QColor(0,255,0,130)));
        painter->drawEllipse(r);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    StaticNode * _node;
    QPen _pen;
};

class STATICNODESSHARED_EXPORT StaticNodes : public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "staticnodes.json")
    Q_INTERFACES(IViewerLayer)

public:
    StaticNodes();
    void setProject(Project * project);

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    void decoratesGraphicsNode(const Node &n, GraphicsNodeItem * node) const;

    QObject * getQObject() { return this; }
    void addDependancy(QObject* plugin);

    virtual QString toString() const { return "StaticNodes"; }

private:
    QMap<int,StaticNode*> _nodes;
    int _communicationRange;
    QObject * _wirelessCommunicationPlugin;
    QGraphicsItemGroup * _nodesItems;


private slots:
    void onMousePressed(QGraphicsSceneMouseEvent*);
    void setCommunicationRange(int);

};

#endif // STATICNODES_H

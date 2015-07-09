#ifndef STATICNODES_H
#define STATICNODES_H

#include "staticnodes_global.h"
#include <QObject>
#include <QGraphicsSceneDragDropEvent>

#include "iplugin.h"
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
    void setCoordinates(double x, double y) { _coordinates.setX(x); _coordinates.setY(y); }

private:
    static int idGenerator;
    QPointF _coordinates;
    int _transmissionRange;
    int _id;
};

class GraphicsStaticNodeItem : public QGraphicsItem
{
public:

    GraphicsStaticNodeItem(StaticNode * node) : _node(node) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }
    GraphicsStaticNodeItem() : _node(new StaticNode()) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }
    QRectF boundingRect() const
    {
        int range = _node->getTransmissionRange();
        QPointF offset = QPointF(range,range);
        return QRectF(_node->getCoordinates()-offset, _node->getCoordinates()+offset);
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
        r = QRectF(_node->getCoordinates() - offset, _node->getCoordinates()+offset);
        QPen p;
        p.setWidth(1);
        p.setCosmetic(true);
        p.setColor(QColor(0,150,0));
        painter->setPen(p);
        painter->setBrush(QBrush(QColor(0,255,0,130)));
        painter->drawEllipse(r);
    }

    void setPen(const QPen &p)
    {
        _pen = p;
    }

protected:
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
        event->setAccepted(true);// tell the base class we are handling this event
        this->setPos(_location);
    }

    void mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
        // allow the user to drag the box, capture the starting position on mouse-down
        event->setAccepted(true);
        qDebug() << "node" << _node->getId() << "left button pressed" << event->scenePos();
        _dragStart = event->pos();
    }

    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
    {
        // user have moved the mouse, move the location of the box
        QPointF newPos = event->pos() ;
        _location += (newPos - _dragStart);
        this->setPos(_location);
    }

private:
    StaticNode * _node;
    QPen _pen;
    QPointF _dragStart;
    QPointF _location;
};

class STATICNODESSHARED_EXPORT StaticNodes : public QObject, public IPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "staticnodes.json")
    Q_INTERFACES(IPlugin)

public:
    StaticNodes();
    void setProject(Project * project);

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    void decorateGraphicsNode(const Node &n, GraphicsNodeItem * node) const;

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

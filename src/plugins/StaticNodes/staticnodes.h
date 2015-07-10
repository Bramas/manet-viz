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
        _coordinates(QPointF()), _transmissionRange(0.0), _id(idGenerator++), _isEnabled(true) {}
    StaticNode(QPointF coordinates, int transmissionRange):
        _coordinates(coordinates), _transmissionRange(transmissionRange), _id(idGenerator++), _isEnabled(true) {}

    QPointF getCoordinates() const { return _coordinates; }
    int getTransmissionRange() const { return _transmissionRange; }
    int getId() const { return _id; }
    void setCoordinates(double x, double y) { _coordinates.setX(x); _coordinates.setY(y); }
    void setCoordinates(QPointF coodinates) { _coordinates = coodinates; }
    void setEnabled(bool state) { _isEnabled = state; }
    void setRange(int range) { _transmissionRange = range; }
    bool isEnabled() { return _isEnabled; }

private:
    static int idGenerator;
    QPointF _coordinates;
    int _transmissionRange;
    int _id;
    bool _isEnabled;
};

class GraphicsStaticNodeItem : public QGraphicsItem
{
public:

    GraphicsStaticNodeItem(StaticNode * node) : _node(node), _initPos(node->getCoordinates()), _range(node->getTransmissionRange()) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }
    GraphicsStaticNodeItem() : _node(new StaticNode()), _initPos(QPointF()), _range(0) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }

    int getShapeNodeId() { return _node->getId(); }

    QRectF boundingRect() const
    {
        QPointF offset = QPointF(_range,_range);
        return QRectF(_initPos-offset, _initPos+offset);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QTransform t = painter->transform().inverted();

        QPointF point = painter->transform().map(_initPos);
        QPointF offset(_pen.widthF()/2.0,_pen.widthF()/2.0);
        QRectF r(t.map(point - offset), t.map(point+offset));
        painter->setPen(Qt::NoPen);
        painter->setBrush(_pen.brush());
        painter->drawEllipse(r);

        // draw the communication range
        offset = QPointF(_range,_range);
        r = QRectF(_initPos-offset, _initPos+offset);
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

    enum { Type = UserType + 1 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

protected:
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
        event->setAccepted(true);

        if(event->modifiers() == Qt::AltModifier) {
            _isResizing = false;
        } else if(event->modifiers() != Qt::ShiftModifier) {
            QGraphicsItem::mouseReleaseEvent(event);
            _node->setCoordinates(_node->getCoordinates() + (pos() - _startDrag));
        }
        _node->setEnabled(true);


    }

    void mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
        event->setAccepted(true);
        _node->setEnabled(false);

        if(event->button() == Qt::LeftButton) {
            if(event->modifiers() == Qt::AltModifier) {
                double radius = boundingRect().width() / 2.0;
                QPointF clikPos = event->scenePos();
                double dist = sqrt(pow(_node->getCoordinates().x()-clikPos.x(), 2) + pow(_node->getCoordinates().y()-clikPos.y(), 2));
                if(dist / radius > 0.8) {
                    qDebug() << dist << radius << dist / radius;
                    _isResizing = true;
                } else {
                    _isResizing = false;
                }
            } else {
                _startDrag = pos();
                QGraphicsItem::mousePressEvent(event);
            }
        } else if(event->button() == Qt::RightButton) {
            event->ignore();
        }
    }

    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
    {
        // user have moved the mouse, move the location of the box
        event->setAccepted(true);
        if(event->modifiers() == Qt::AltModifier && _isResizing){
            QPointF mousePos = event->scenePos();
            double dist = sqrt(pow(_node->getCoordinates().x()-mousePos.x(), 2) + pow(_node->getCoordinates().y()-mousePos.y(), 2));
            _range = dist;
            _node->setRange(_range);

        } else if(event->modifiers() != Qt::AltModifier) {
            QGraphicsItem::mouseMoveEvent(event);
        }
    }

private:
    StaticNode * _node;
    QPointF _initPos;
    QPointF _startDrag;
    bool _isResizing;
    int _range;
    QPen _pen;
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

private slots:
    void onMousePressed(QGraphicsSceneMouseEvent*event);
    void setCommunicationRange(int);
};

#endif // STATICNODES_H

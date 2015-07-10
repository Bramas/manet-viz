#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

// #include <QAbstractGraphicsShapeItem> TODO
#include <QGraphicsItem>
#include <QPen>
#include <QPointF>
#include <QLineF>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class GraphicsNodeItem : public QGraphicsItem
{
public:

    GraphicsNodeItem(QPointF position) : _position(position) { }
    GraphicsNodeItem() : _position(QPointF(0,0)) { }
    QRectF boundingRect() const
    {
        return QRectF(_position.x()-0.00001, _position.y()-0.00001,0.00002,0.00002);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QTransform t = painter->transform().inverted();
        QPointF point = painter->transform().map(_position);
        QPointF offset(_pen.widthF()/2.0,_pen.widthF()/2.0);
        QRectF r(t.map(point - offset), t.map(point+offset));
        painter->setPen(Qt::NoPen);
        painter->setBrush(_pen.brush());
        painter->drawEllipse(r);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _position;
};

class GraphicsEdgeItem : public QGraphicsItem
{
public:

    GraphicsEdgeItem(QPointF p1, QPointF p2) : _p1(p1), _p2(p2) { }
    QRectF boundingRect() const
    {
        QPointF topLeft(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y()));
        QPointF bottomRight(qMax(_p1.x(), _p2.x()), qMax(_p1.y(), _p2.y()));
        return QRectF(topLeft, bottomRight);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        //painter->setRenderHint(QPainter::Antialiasing);
        QLineF line(_p1, _p2);
        painter->setPen(_pen);
        painter->drawLine(line);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _p1;
    QPointF _p2;
};


#endif // GRAPHICSITEMS_H


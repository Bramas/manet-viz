#include "graphlayer.h"
#include "viewer.h"
#include "geometricgraph.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>

class GraphicsNodeItem : public QGraphicsItem
{
public:

    GraphicsNodeItem(QPointF position) : _position(position) { }
    QRectF boundingRect() const
    {
        return QRectF(_position - QPointF(1,1), _position + QPointF(1,1));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setPen(_pen);
        painter->drawPoint(_position);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _position;
};

void GraphLayer::paint(mvtime time)
{
    Graph g = _evolvingGraph->footprint(_evolvingGraph->beginTime() + time);//_parent->time());

    _scene->removeItem(items);
    delete items;
    items = new QGraphicsItemGroup();


    QPen p(QColor(255,0,0));
    p.setWidth(2);
    p.setCosmetic(true);
    foreach(const Node& n1, g.nodes())
    {
        foreach(const Node& n2, n1.neighbors())
        {
            //items->addToGroup((QGraphicsItem*) new QGraphicsLineItem(QLineF(n1.position(), (n2.position()))));
        }
    }
    foreach(const Node& n, g.nodes())
    {
        GraphicsNodeItem * i = new GraphicsNodeItem(n.position());
        i->setPen(p);
        items->addToGroup((QGraphicsItem*)i);
        qDebug()<<"add Point "<<n.position();
        /*painter->drawEllipse((n.position()), 1,1);

        painter->save();
        painter->setPen(QPen(QColor(0,0,0,100)));
        painter->drawText((n.position())+QPointF(2,-2), QString::number(n.id()));
        painter->restore();*/
    }
    _scene->addItem(items);
}

void GraphLayer::setGraphicsScene(QGraphicsScene *scene)
{
    _scene = scene;
    items = new QGraphicsItemGroup();
    _scene->addItem(items);
}


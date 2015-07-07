#include "staticnodes.h"
#include <QVector2D>

StaticNodes::StaticNodes()
{
}
//+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs
//+proj=utm +zone=33 +ellps=WGS84 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs

void StaticNodes::decorateEdges(mvtime time, IGraph *graph)
{
    foreach(const Node &n, graph->nodes())
    {
        if(n.id() == 1000)
        {
            continue;
        }

        double x = n.properties().value(X).toDouble();
        double y = n.properties().value(Y).toDouble();
        QVector2D p1(x,y);

        x = graph->nodes().value(1000).properties().value(X).toDouble();
        y = graph->nodes().value(1000).properties().value(Y).toDouble();

        QVector2D p2(x,y);
        if(p1.distanceToPoint(p2) <= 160)
        {
            graph->addEdge(n.id(), 1000);
        }
        //qDebug()<<p1<<p2<<p1.distanceToPoint(p2);
    }
}


void StaticNodes::decorateNodes(mvtime time, IGraph *graph)
{
    qreal x = 291189.83048024075;//41.8949486;
    qreal y = 4641176.482165459;//12.4828996;

    NodeProperties props;
    props.insert(X, x);
    props.insert(Y, y);
    graph->addNode(1000, props);
}

void StaticNodes::decoratesGraphicsNode(const Node &n, GraphicsNodeItem * node) const
{
    if(n.id() != 1000)
    {
        return;
    }
    QGraphicsEllipseItem * e = new QGraphicsEllipseItem(QRectF(-160, -160, 320, 320), node);
    QPen p;
    p.setWidth(2);
    p.setColor(QColor(0,150,0));
    e->setBrush(QBrush(QColor(0,255,0,130)));
    e->setPen(p);
}

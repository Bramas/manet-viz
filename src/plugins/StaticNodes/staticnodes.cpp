#include "staticnodes.h"
#include "project.h"
#include <QVector2D>
#include <QGraphicsSceneMouseEvent>

int StaticNode::idGenerator = 1000000;

StaticNodes::StaticNodes()
{
    _communicationRange = 100;
    _nodesItems = new QGraphicsItemGroup();
}

void StaticNodes::setProject(Project *project)
{
    _project = project;
    connect(project->viewer(), SIGNAL(mousePressedEvent(QGraphicsSceneMouseEvent*)), this, SLOT(onMousePressed(QGraphicsSceneMouseEvent*)));
//    StaticNode * staticNode = new StaticNode(QPointF(291189.83048024075, 4641176.482165459), 160);
//    _nodes.insert(staticNode->getId(), staticNode);
//    _nodesItems->addToGroup(new GraphicsStaticNodeItem(staticNode));

    _project->viewer()->addItem(_nodesItems);
}
//+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs
//+proj=utm +zone=33 +ellps=WGS84 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs

void StaticNodes::decorateEdges(mvtime time, IGraph *graph)
{
    foreach(const Node &n, graph->nodes())
    {
        if(_nodes.contains(n.id()))
            continue;

        double x = n.properties().value(X).toDouble();
        double y = n.properties().value(Y).toDouble();
        QVector2D p1(x,y);

        for(auto it = _nodes.begin(); it != _nodes.end() ;++it) {
            StaticNode * staticNode = it.value();
            x = staticNode->getCoordinates().x();
            y = staticNode->getCoordinates().y();
            QVector2D p2(x,y);
            if(p1.distanceToPoint(p2) <= staticNode->getTransmissionRange())
            {
                graph->addEdge(n.id(), staticNode->getId());
            }
        }

        //qDebug()<<p1<<p2<<p1.distanceToPoint(p2);
    }
}


void StaticNodes::decorateNodes(mvtime time, IGraph *graph)
{
    foreach(auto node, _nodes) {
        NodeProperties props;
        props.insert(X, node->getCoordinates().x());
        props.insert(Y, node->getCoordinates().y());
        graph->addNode(node->getId(), props);
    }
}

void StaticNodes::decorateGraphicsNode(const Node &n, GraphicsNodeItem * node) const
{
    if(!_nodes.contains(n.id()))
        return;

    node->setVisible(false);
}

void StaticNodes::addDependancy(QObject *plugin)
{
    if(plugin->objectName() == "WComDecorator")
    {
        _wirelessCommunicationPlugin = plugin;
        connect(_wirelessCommunicationPlugin, SIGNAL(transmissionRangeChanged(int)), this, SLOT(setCommunicationRange(int)));
    }
}

void StaticNodes::onMousePressed(QGraphicsSceneMouseEvent * e)
{
    e->setAccepted(true);
    if(e->button() == Qt::LeftButton) {
        qDebug() << "left button pressed" << e->scenePos() << e->screenPos() << e->lastPos() << e->pos();
        // create new static node where the user clicked
        StaticNode * staticNode = new StaticNode(e->scenePos(),_communicationRange);
        _nodes.insert(staticNode->getId(),staticNode);
        GraphicsStaticNodeItem * graphicsNode = new GraphicsStaticNodeItem(staticNode);
        qDebug() << "bounding rectangle" << graphicsNode->boundingRect();
        QPen p;
        p.setWidthF(3);
        graphicsNode->setPen(p);
        _nodesItems->addToGroup(graphicsNode);

    } else if (e->button() == Qt::RightButton) {
        // delete the static node from the map
        qDebug() << "right button pressed" << e->scenePos();
    }
}

void StaticNodes::setCommunicationRange(int range)
{
    _communicationRange = range;
}

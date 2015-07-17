#include "staticnodes.h"
#include "project.h"
#include <QVector2D>
#include <QGraphicsSceneMouseEvent>

int StaticNode::idGenerator = 1000000;

StaticNodes::StaticNodes()
{
    _communicationRange = 100;
}

void StaticNodes::setProject(Project *project)
{
    _project = project;
    connect(project->viewer(), SIGNAL(mousePressedEvent(QGraphicsSceneMouseEvent*)), this, SLOT(onMousePressed(QGraphicsSceneMouseEvent*)));
//    StaticNode * staticNode = new StaticNode(QPointF(291189.83048024075, 4641176.482165459), 160);
//    _nodes.insert(staticNode->getId(), staticNode);
//    _nodesItems->addToGroup(new GraphicsStaticNodeItem(staticNode));
}
//+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs
//+proj=utm +zone=33 +ellps=WGS84 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs

void StaticNodes::decorateEdges(mvtime time, IGraph *graph)
{
    foreach(const Node &n, graph->nodes())
    {
        bool isStaticNode = false;
        if(_nodes.contains(n.id()))
            isStaticNode = true;

        // check whether the static node is enabled
        if(isStaticNode && !_nodes.value(n.id())->isEnabled())
            continue;

        double x = n.properties().value(X).toDouble();
        double y = n.properties().value(Y).toDouble();
        QVector2D p1(x,y);

        // add edges for the nodes within the distance of each static node
        for(auto it = _nodes.begin(); it != _nodes.end() ;++it) {
            StaticNode * staticNode = it.value();
            if(!staticNode->isEnabled())
                continue;
            if(isStaticNode && _nodes.value(n.id())->getTransmissionRange() < staticNode->getTransmissionRange())
                continue;

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
        if(!node->isEnabled())
            continue;
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
}

void StaticNodes::addDependancy(QObject *plugin)
{
    if(plugin->objectName() == "WComDecorator")
    {
        _wirelessCommunicationPlugin = plugin;
        connect(_wirelessCommunicationPlugin, SIGNAL(transmissionRangeChanged(int)), this, SLOT(setCommunicationRange(int)));
    }
}

void StaticNodes::onMousePressed(QGraphicsSceneMouseEvent * event)
{
//    e->setAccepted(true);
    if(event->button() == Qt::LeftButton) {
        if(event->modifiers() != Qt::AltModifier) {
            // create new static node where the user clicked
            StaticNode * staticNode = new StaticNode(event->scenePos(),_communicationRange);
            _nodes.insert(staticNode->getId(),staticNode);
            GraphicsStaticNodeItem * graphicsNode = new GraphicsStaticNodeItem(staticNode);
            QPen p;
            p.setWidthF(4);
            graphicsNode->setPen(p);
            _project->viewer()->addItem(graphicsNode);
        }
    } else if (event->button() == Qt::RightButton) {
        // delete the static node from the map
        qDebug() << "right button pressed" << event->scenePos();
        QGraphicsItem * itemToRemove = NULL;
        foreach(auto item, _project->viewer()->items(event->scenePos())) {
            if(item->type() == QGraphicsItem::UserType+1) {
                itemToRemove = item;
                break;
            }
        }

        if(itemToRemove) {
            // remove the item from the graphicsScene
            GraphicsStaticNodeItem * graphicsNode = qgraphicsitem_cast<GraphicsStaticNodeItem *>(itemToRemove);
            _nodes.remove(graphicsNode->getShapeNodeId());
            _project->viewer()->removeItem(itemToRemove);
        }

//        _project->viewer()->removeItem();
    }
}



void StaticNodes::setCommunicationRange(int range)
{
    _communicationRange = range;
}

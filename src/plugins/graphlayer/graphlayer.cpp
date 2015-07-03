#include "graphlayer.h"
#include "viewer.h"
#include "ui_graphlayercontrol.h"
#include "project.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>

GraphLayer::GraphLayer()
{
    _displayContacts = false;
    items = 0;
}

void GraphLayer::paint(IGraph * graph)
{
    //Graph g = _evolvingGraph->footprint(_evolvingGraph->beginTime() + graph);//_parent->time());

    if(items)
    {
        _project->viewer()->removeItem(items);
        delete items;
    }
    items = new QGraphicsItemGroup();


    QPen p(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);

    if(_displayContacts) {
        foreach(const Node& n1, graph->nodes())
        {
            foreach(int n2, n1.neighbors())
            {
                QPointF p1(n1.properties().value(X).toDouble(), n1.properties().value(Y).toDouble());
                QPointF p2(graph->nodes().value(n2).properties().value(X).toDouble(), graph->nodes().value(n2).properties().value(Y).toDouble());
                GraphicsEdgeItem * line = new GraphicsEdgeItem(p1, p2);
                line->setPen(p);
                items->addToGroup((QGraphicsItem*) line);
            }
        }
    }
    p.setColor(QColor(0,0,0));
    p.setWidthF(4);
    foreach(const Node &n, graph->nodes())
    {
        QPointF position(n.properties().value(X).toDouble(), n.properties().value(Y).toDouble());
        GraphicsNodeItem * i = new GraphicsNodeItem(position);
        i->setPen(p);
        items->addToGroup((QGraphicsItem*)i);
    }

    _project->viewer()->addItem(items);
}


void GraphLayer::setDisplayContact(bool state)
{
    _displayContacts = state;
    emit requestUpdate();
}
QWidget* GraphLayer::createControlWidget() const
{
    QWidget *control = new QWidget();
    Ui::GraphLayerControl  *ui = new Ui::GraphLayerControl();
    ui->setupUi(control);

    connect(ui->displayContactCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayContact(bool)));

    return control;
}

void GraphLayer::decoratesGraphicsEdge(GraphicsEdgeItem * node) const
{
    QPen p(QColor(0,255,0));
    p.setWidth(1);
    p.setCosmetic(true);
    node->setPen(p);
    if(!_displayContacts)
        node->setVisible(false);
}



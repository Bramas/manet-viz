#include "wirelesscommunicationdecorator.h"
#include <QWidget>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsEllipseItem>
#include <QPen>

#include  "ui_control.h"


#include "graph.h"
#include "types.h"

WirelessCommunicationDecorator::WirelessCommunicationDecorator()
{
    _communicationRange = 2;
    _displayRange = false;
}
void WirelessCommunicationDecorator::setEvolvingGraph(const AbstractEvolvingGraph *evolvingGraph)
{
    _evolvingGraph = evolvingGraph;
}

void WirelessCommunicationDecorator::decorateNodes(mvtime time, IGraph *graph)
{

}

void WirelessCommunicationDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    foreach(const Node& n1, graph->nodes())
    {
        foreach(const Node& n2, graph->nodes())
        {
            if(n1.id() != n2.id())
            {
                QVector2D p1(n1.properties().value("X").toDouble(), n1.properties().value("Y").toDouble());
                QVector2D p2(n2.properties().value("X").toDouble(), n2.properties().value("Y").toDouble());
                if(p1.distanceToPoint(p2) <= _communicationRange / 1000.0)
                {
                    graph->addEdge(n1.id(),n2.id());
                }
            }
        }
    }
}

void WirelessCommunicationDecorator::setCommunicationRange(int range)
{
    _communicationRange = range;
    emit requestUpdate();
}

void WirelessCommunicationDecorator::setDisplayRange(bool state)
{
    _displayRange = state;
    emit requestUpdate();
}

QWidget * WirelessCommunicationDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    Ui::Control * ui = new Ui::Control();
    ui->setupUi(control);

    connect(ui->displayRangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayRange(bool)));
    connect(ui->transmissionRangeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCommunicationRange(int)));
    return control;
}

QGraphicsItem*  WirelessCommunicationDecorator::graphicsNodeChildItem(const Node& n) const
{
    if(!_displayRange)
        return 0;
    qreal radius = _communicationRange / 1000.0;
    QPointF offset(radius,radius);
    QPen p;
    p.setWidth(0);
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem(QRectF(-offset, offset));
    item->setPen(p);
    return item;
}

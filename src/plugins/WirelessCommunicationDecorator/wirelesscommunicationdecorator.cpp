#include "wirelesscommunicationdecorator.h"
#include <QWidget>
#include <QDebug>
#include <QVector2D>

#include  "ui_control.h"


#include "graph.h"
#include "types.h"

WirelessCommunicationDecorator::WirelessCommunicationDecorator()
{
    _communicationRange = 2;
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

QWidget * WirelessCommunicationDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    Ui::Control * ui = new Ui::Control();
    ui->setupUi(control);

    connect(ui->transmissionRangeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCommunicationRange(int)));
    return control;
}

#include "wirelesscommunicationdecorator.h"
#include <QWidget>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsEllipseItem>
#include <QPen>

WirelessCommunicationDecorator::WirelessCommunicationDecorator():
    ui(new Ui::Control)
{
    _communicationRange = 100;
    _displayRange = false;
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
                QVector2D p1(n1.properties().value(X).toDouble(), n1.properties().value(Y).toDouble());
                QVector2D p2(n2.properties().value(X).toDouble(), n2.properties().value(Y).toDouble());
                if(p1.distanceToPoint(p2) <= _communicationRange)
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
    ui->labelTransmissionRange->setText(QString::number(range));
    emit requestUpdate();
    emit transmissionRangeChanged(range);
}

void WirelessCommunicationDecorator::setDisplayRange(bool state)
{
    _displayRange = state;
    emit requestUpdate();
}

QWidget * WirelessCommunicationDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);
    ui->labelTransmissionRange->setText(QString::number(_communicationRange));
    connect(ui->displayRangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayRange(bool)));
    connect(ui->transmissionRangeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCommunicationRange(int)));
    return control;
}

void  WirelessCommunicationDecorator::decoratesGraphicsNode(const Node &n, GraphicsNodeItem * node) const
{
    if(!_displayRange)
        return ;
    qreal radius = _communicationRange;
    QPointF offset(radius,radius);
    QPen p;
    p.setWidth(0);
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem(QRectF(-offset, offset));
    item->setPen(p);
    item->setParentItem(node);
}

#include "wcomdecorator.h"
#include "project.h"
#include <QtCore>
#include <QWidget>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsEllipseItem>
#include <QPen>

#include "ui_control.h"

WComDecorator::WComDecorator():
    ui(new Ui::Control)
{
    _communicationRange = 100;
    _displayRange = false;
    _cellSize = 2*_communicationRange;

    _pen = QPen(QColor(255,0,0));
    _pen.setWidth(1);
    _pen.setCosmetic(true);
}


inline uint qHash(const QPoint &key)
{
    return qHash(key.x()) ^ qHash(key.y());
}

QList<QPoint> WComDecorator::getNeighborCells(double x, double y) {
    QList<QPoint> ret;
    QStringList tmp;
    QPoint gc((int)qFloor(x / _cellSize), (int)qFloor(y / _cellSize));
    double dx = _communicationRange, dy = _communicationRange;

    ret.append(gc);

    // Consider the cell of the same point, translated by communicationRange on both x and y axis
    QPoint gc_p((int)qFloor((x + _communicationRange) / _cellSize), (int)qFloor((y + _communicationRange) / _cellSize));

    if(gc_p.x() == gc.x()) // if they have the same x coordinates, collisions can only occurs with cells on the left
    {
        ret.append(QPoint(gc.x()-1, gc.y()));
        if(gc_p.y() == gc.y()) // if they have the same y coordinates, collisions can only occurs with cells on the top
        {
            ret.append(QPoint(gc.x(), gc.y()-1));
            ret.append(QPoint(gc.x()-1, gc.y()-1));
        }
        else  // if they have different y coordinates, collisions can only occurs with cells on the bottom
        {
            ret.append(QPoint(gc.x(), gc.y()+1));
            ret.append(QPoint(gc.x()-1, gc.y()+1));
        }
    }
    else // if they have different x coordinates, collisions can only occurs with cells on the right
    {
        ret.append(QPoint(gc.x()+1, gc.y()));
        if(gc_p.y() == gc.y()) // if they have the same y coordinates, collisions can only occurs with cells on the top
        {
            ret.append(QPoint(gc.x(), gc.y()-1));
            ret.append(QPoint(gc.x()+1, gc.y()-1));
        }
        else  // if they have different y coordinates, collisions can only occurs with cells on the bottom
        {
            ret.append(QPoint(gc.x(), gc.y()+1));
            ret.append(QPoint(gc.x()+1, gc.y()+1));
        }
    }
    return ret;
}

void WComDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    _grid.clear();

    // Populate the grid
    int nrofNodes = 0, nrofContacts = 0;
    foreach(const Node& n1, graph->nodes())
    {
        double x = n1.properties().value(X).toDouble();
        double y = n1.properties().value(Y).toDouble();

        QPoint gc((int)qFloor(x / _cellSize), (int)qFloor(y / _cellSize));

        _grid.insert(gc,n1);
        nrofNodes++;
    }

    foreach(const QPoint gc, _grid.uniqueKeys()) {
        foreach(const Node& n1, _grid.values(gc)) {
            double x = n1.properties().value(X).toDouble();
            double y = n1.properties().value(Y).toDouble();
            QVector2D p1(x,y);
            QList<QPoint> gcList = getNeighborCells(x,y);
            foreach(auto c, gcList) {
                if(!_grid.keys().contains(c)) {
                    continue;
                }

                foreach(const Node& n2, _grid.values(c)) {
                    if(n1.id() < n2.id())
                    {
                        QVector2D p2(n2.properties().value(X).toDouble(), n2.properties().value(Y).toDouble());
                        if(p1.distanceToPoint(p2) <= _communicationRange)
                        {
                            graph->addEdge(n1.id(),n2.id());
                            nrofContacts++;
                        }
                    }
                }
            }
        }
    }

    // update the number of nodes and contacts in the UI

    ui->labelNrofNodes->setText(QString::number(nrofNodes));
    ui->labelNrofContacts->setText(QString::number(nrofContacts));
}

void WComDecorator::setCommunicationRange(int range)
{
    _communicationRange = range;
    _cellSize = 2*range;
    ui->labelTransmissionRange->setText(QString::number(range));
    emit requestUpdate();
    emit transmissionRangeChanged(range);
}

void WComDecorator::setDisplayRange(bool state)
{
    _displayRange = state;
    emit requestUpdate();
}

QWidget * WComDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);
    ui->labelTransmissionRange->setText(QString::number(_communicationRange));
    connect(ui->displayRangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayRange(bool)));
    connect(ui->transmissionRangeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCommunicationRange(int)));
    return control;
}

void  WComDecorator::decorateGraphicsNode(const Node &n, GraphicsNodeItem * node) const
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

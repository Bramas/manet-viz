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

    // check northwest neighbor cell
    if(((x-dx) < (gc.x())) && ((y-dy) < (gc.y()))) {
        ret.append(QPoint(gc.x()-1, gc.y()-1));
        tmp << "northwest";
    }
    // check north neighbor cell
    if(((y-dy) < (gc.y()))) {
        ret.append(QPoint(gc.x(), gc.y()-1));
        tmp << "north";
    }
    // check northeast neighbor cell
    if(((x+dx) > (gc.x()+1)) && ((y-dy) < (gc.y()))) {
        ret.append(QPoint(gc.x()+1, gc.y()-1));
        tmp << "northeast";
    }
    // check west neighbor cell
    if(((x+dx) > (gc.x()+1))) {
        ret.append(QPoint(gc.x()+1, gc.y()));
        tmp << "west";
    }
    // check east neighbor cell
    if(((x-dx) < (gc.x()))) {
        ret.append(QPoint(gc.x()-1, gc.y()));
        tmp << "east";
    }
    // check southwest neighbor cell
    if(((x+dx) < (gc.x())) && ((y+dy) > (gc.y()+1))) {
        ret.append(QPoint(gc.x()-1, gc.y()+1));
        tmp << "southwest";
    }
    // check south neighbor cell
    if(((y+dy) > (gc.y()+1))) {
        ret.append(QPoint(gc.x(), gc.y()+1));
        tmp << "south";
    }
    // check southeast neighbor cell
    if(((x+dx) > (gc.x()+1)) && ((y+dy) > (gc.y()+1))) {
        ret.append(QPoint(gc.x()+1, gc.y()+1));
        tmp << "southeast";
    }

    return ret;
}

void WComDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    _grid.clear();

    // Populate the grid
    foreach(const Node& n1, graph->nodes())
    {
        double x = n1.properties().value(X).toDouble();
        double y = n1.properties().value(Y).toDouble();

        QPoint gc((int)qFloor(x / _cellSize), (int)qFloor(y / _cellSize));

        _grid.insert(gc,n1);
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
                        }
                    }
                }
            }
        }
    }
}

void WComDecorator::setCommunicationRange(int range)
{
    _communicationRange = range;
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

void  WComDecorator::decoratesGraphicsNode(GraphicsNodeItem * node) const
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

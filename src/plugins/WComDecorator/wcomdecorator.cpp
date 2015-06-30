#include "wcomdecorator.h"

#include <QtCore>
#include <QWidget>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsEllipseItem>
#include <QPen>

WComDecorator::WComDecorator():
    ui(new Ui::Control)
{
    _communicationRange = 100;
    _displayRange = false;
    _cellSize = 2*_communicationRange;
    _grid = QMultiHash<QPointF,Node>();
    _oldSceneRect = QRectF();
}
void WComDecorator::setEvolvingGraph(const AbstractEvolvingGraph *evolvingGraph)
{
    _evolvingGraph = evolvingGraph;

}

void WComDecorator::paint(IGraph *graph)
{
    QRectF sceneRect = _scene->sceneRect();

//    if(_oldSceneRect.isEmpty() || _oldSceneRect != sceneRect) {
//        // Compute the new grid
//        _gridGroupItems.clear();
//        double xMin = (int)qFloor(sceneRect.left() / _cellSize)  * _cellSize;
//        double xMax = (int)qCeil(sceneRect.right() / _cellSize)  * _cellSize;
//        double yMin = (int)qFloor(sceneRect.top() / _cellSize)   * _cellSize;
//        double yMax = (int)qCeil(sceneRect.bottom() / _cellSize) * _cellSize;

//        for(int i = xMin;i<=xMax; i+=_cellSize) {
//            QGraphicsLineItem * item = new QGraphicsLineItem(i,sceneRect.top(),i,sceneRect.bottom());
//            item->setPen(_pen);
//            _gridGroupItems.append(item);
//        }

//        for(int j = yMin;j<=yMax;j+=_cellSize) {
//            QGraphicsLineItem * item = new QGraphicsLineItem(sceneRect.left(),j, sceneRect.right(), j);
//            item->setPen(_pen);
//            _gridGroupItems.append(item);
//        }

//        _scene->createItemGroup(_gridGroupItems);
//        _oldSceneRect = sceneRect;
//    }
}

void WComDecorator::setGraphicsScene(QGraphicsScene *scene)
{
    _scene = scene;
    _pen = QPen(QColor(255,0,0));
    _pen.setWidth(1);
    _pen.setCosmetic(true);
}

void WComDecorator::decorateNodes(mvtime time, IGraph *graph)
{

}

QList<QPointF> WComDecorator::getNeighborCells(double x, double y) {
    QList<QPointF> ret;
    QStringList tmp;
    QPointF gc((int)qFloor(x / _cellSize)*_cellSize, (int)qFloor(y / _cellSize)*_cellSize);
    double dx = _communicationRange, dy = _communicationRange;

    ret.append(gc);

    // check northwest neighbor cell
    if(((x-dx) < (gc.x())) && ((y-dy) < (gc.y()))) {
        ret.append(QPointF(gc.x()-_cellSize, gc.y()-_cellSize));
        tmp << "northwest";
    }
    // check north neighbor cell
    if(((y-dy) < (gc.y()))) {
        ret.append(QPointF(gc.x(), gc.y()-_cellSize));
        tmp << "north";
    }
    // check northeast neighbor cell
    if(((x+dx) > (gc.x()+_cellSize)) && ((y-dy) < (gc.y()))) {
        ret.append(QPointF(gc.x()+_cellSize, gc.y()-_cellSize));
        tmp << "northeast";
    }
    // check west neighbor cell
    if(((x+dx) > (gc.x()+_cellSize))) {
        ret.append(QPointF(gc.x()+_cellSize, gc.y()));
        tmp << "west";
    }
    // check east neighbor cell
    if(((x-dx) < (gc.x()))) {
        ret.append(QPointF(gc.x()-_cellSize, gc.y()));
        tmp << "east";
    }
    // check southwest neighbor cell
    if(((x+dx) < (gc.x())) && ((y+dy) > (gc.y()+_cellSize))) {
        ret.append(QPointF(gc.x()-_cellSize, gc.y()+_cellSize));
        tmp << "southwest";
    }
    // check south neighbor cell
    if(((y+dy) > (gc.y()+_cellSize))) {
        ret.append(QPointF(gc.x(), gc.y()+_cellSize));
        tmp << "south";
    }
    // check southeast neighbor cell
    if(((x+dx) > (gc.x()+_cellSize)) && ((y+dy) > (gc.y()+_cellSize))) {
        ret.append(QPointF(gc.x()+_cellSize, gc.y()+_cellSize));
        tmp << "southeast";
    }

    return ret;
}

inline uint qHash(const QPointF &key)
{
    return qHash(key.x()) ^ qHash(key.y());
}


void WComDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    _grid.clear();

    // Populate the grid
    foreach(const Node& n1, graph->nodes())
    {
        double x = n1.properties().value(X).toDouble();
        double y = n1.properties().value(Y).toDouble();

        QPointF gc((int)qFloor(x / _cellSize) * _cellSize, (int)qFloor(y / _cellSize) * _cellSize);

        _grid.insert(gc,n1);
    }

    foreach(const QPointF gc, _grid.keys()) {
        foreach(const Node& n1, _grid.values(gc)) {
            double x = n1.properties().value(X).toDouble();
            double y = n1.properties().value(Y).toDouble();
            QVector2D p1(x,y);
            QList<QPointF> gcList = getNeighborCells(x,y);
            foreach(auto c, gcList) {
                if(!_grid.keys().contains(c)) {
                    continue;
                }

                foreach(const Node& n2, _grid.values(c)) {
                    if(n1.id() != n2.id())
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
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
    _gridCount = QHash<QPointF,QHash<mvtime,int> >();
    _contactCount = QHash<QPointF,int>();
    _timeWindow = 60*60; // 60 minutes
}
void WComDecorator::setEvolvingGraph(const AbstractEvolvingGraph *evolvingGraph)
{
    _evolvingGraph = evolvingGraph;
}



void WComDecorator::paint(IGraph *graph)
{
    // Display the cell counts
    _gridGroupItems.clear();

    for(auto cellIt = _contactCount.begin(); cellIt != _contactCount.end(); ++cellIt) {
        int contactSum = cellIt.value();

        // Display the sum on the grid
        if(contactSum > 0) {
            QGraphicsRectItem * item = new QGraphicsRectItem(cellIt.key().x(), cellIt.key().y(), _cellSize, _cellSize);
            item->setBrush(QBrush(selectCellColor(contactSum)));
            item->setPen(Qt::NoPen);
//            QGraphicsTextItem * item = new QGraphicsTextItem(QString::number(contactSum)); // _scene->addText(text);
//            item->setPos(cellIt.key());
            _gridGroupItems.append(item);
        }
    }

    _scene->createItemGroup(_gridGroupItems);


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

void WComDecorator::increaseCellCount(QPointF cell, mvtime time)
{
    if(!_gridCount.contains(cell)) {
        _gridCount.insert(cell,QHash<mvtime,int>());
    }
    if(!_gridCount.value(cell).contains(time)) {
        _gridCount[cell].insert(time, 1);
    } else {
        int count = _gridCount.value(cell).value(time);
        _gridCount[cell].insert(time, count + 1);
    }
}

void WComDecorator::deleteObsoleteCells(mvtime time)
{
    _contactCount.clear();
    for(auto cellIt = _gridCount.begin(); cellIt != _gridCount.end(); ++cellIt) {
        int cellCount = 0;
        for(auto i = cellIt.value().begin(); i != cellIt.value().end(); ++i) {
            if(time - i.key() > _timeWindow) {
                // erase the item in the hash
                cellIt.value().erase(i);
            } else {
                cellCount += i.value();
            }
        }

        // delete grid in hash table if there is no counts
        if(cellCount == 0) {
            _gridCount.erase(cellIt);
        } else {
            _contactCount.insert(cellIt.key(), cellCount);
        }
    }
}

QColor WComDecorator::selectCellColor(int cellCount)
{
    // use quartiles
    // TODO: use natural jenks
    QList<int> l(_contactCount.values());
    std::sort(l.begin(), l.end());
    int q12 = l[l.size()*1/4];
    int q23 = l[l.size()*2/4];
    int q34 = l[l.size()*3/4];

    if(cellCount > q34) return QColor(210,211,231);
    else if(cellCount > q23) return QColor(219,218,234);
    else if(cellCount > q12) return QColor(226,225,239);
    else return QColor(233,233,243);
}

QList<double> WComDecorator::getJenksBreaks(QList<double> sListDouble, int sClassCount)
{
    QList<double> pResult = QList<double>();
    for (int i = 0; i < sClassCount; i++)
        pResult.append(0);

    int numdata = sListDouble.size();
    QList<double> l(sListDouble);
    std::sort(l.begin(), l.end());

    QHash<QPair<int,int>,double> mat1 = QHash<QPair<int,int>,double>();
    QHash<QPair<int,int>,double> mat2 = QHash<QPair<int,int>,double>();

    for (int i = 1; i <= sClassCount; i++)
    {
        mat1.insert(QPair<int,int>(1,i), 1);
        mat2.insert(QPair<int,int>(1,i), 0);
        for (int j = 2; j <= numdata; j++)
        {
            mat2.insert(QPair<int,int>(j,i), MaxDist);
        }
    }

    double ssd = 0;
    for (int rangeEnd = 2; rangeEnd <= numdata; rangeEnd++)
    {
        double sumX = 0;
        double sumX2 = 0;
        double w = 0;
        int dataId;
        for (int m = 1; m <= rangeEnd; m++)
        {
            dataId = rangeEnd - m + 1;
            double val = l[dataId - 1];
            sumX2 += val * val;
            sumX += val;
            w++;
            ssd = sumX2 - (sumX * sumX) / w;
            for (int j = 2; j <= sClassCount; j++)
            {
                QPair<int,int> p1(rangeEnd,j);
                QPair<int,int> p2(dataId - 1, j - 1);
                if (!(mat2[p1] < (ssd + mat2[p2])))
                {
                    mat1[p1] = dataId;
                    mat2[p1] = ssd + mat2[p2];
                }
            }
        }
        QPair<int,int> p(rangeEnd, 1);
        mat1[p] = 1;
        mat2[p] = ssd;
    }

    pResult[sClassCount - 1] = l[numdata - 1];

    int k = numdata;
    for (int j = sClassCount; j >= 2; j--)
    {
        QPair<int,int> p(k,j);
        int id = (int)(mat1[p]) - 2;
        pResult[j - 2] = l[id];
        k = (int)mat1[p] - 1;
    }

    return pResult;
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
                            // increase the contact count for the grid cells
                            increaseCellCount(gc, time);
                            if(gc != c)
                                increaseCellCount(c, time);
                        }
                    }
                }
            }
        }
    }

    // update the grid count hash by deleting the obsolete cells
    deleteObsoleteCells(time);
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

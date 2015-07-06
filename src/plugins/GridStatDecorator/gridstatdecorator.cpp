#include "gridstatdecorator.h"
#include "ui_control.h"
#include "project.h"
#include "iloader.h"

GridStatDecorator::GridStatDecorator():
    ui(new Ui::Control)
{
//    QObject * instance = PluginManager::getPlugin("WirelessCommunicationDecorator");
//    if(instance) {
//        connect(instance, SIGNAL(transmissionRangeChanged(int)), this, SLOT(setTransmissionRange(int)));
//    }

    _gridCount = QHash<QPoint,QHash<mvtime,int> >();
    _contactCount = QHash<QPoint,int>();
    _timeWindow = 10*60; // 60 minutes
    _showGrid = true;
    _cellSize = 500;
}

void GridStatDecorator::setProject(Project * project)
{

    _project = project;

    foreach(auto layer, _project->layers()) {
        qDebug() << "plugin:" + layer->getQObject()->objectName();
    }

    qDebug() << "GridStatDecorator set project";
}

void GridStatDecorator::paint(IGraph *graph)
{
    // Display the cell counts
    _gridGroupItems.clear();

    for(auto cellIt = _contactCount.begin(); cellIt != _contactCount.end(); ++cellIt) {
        int contactSum = cellIt.value();

        // Display the sum on the grid
        if(contactSum > 0) {
            QGraphicsRectItem * item = new QGraphicsRectItem(_cellSize*cellIt.key().x(), _cellSize*cellIt.key().y(), _cellSize, _cellSize);
            item->setBrush(QBrush(selectCellColor(contactSum)));
            item->setPen(Qt::NoPen);
            item->setOpacity(0.5);
            _gridGroupItems.append(item);
        }
    }

    _project->viewer()->createItemGroup(_gridGroupItems);
}

void GridStatDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    if(_showGrid) {
        // update the grid count hash by deleting the obsolete cells
        deleteObsoleteCells(time);

        // Populate the grid
        foreach(const Node& n1, graph->nodes())
        {
            QPoint gc1((int)qFloor(n1.properties().value(X).toDouble() / _cellSize), (int)qFloor(n1.properties().value(Y).toDouble() / _cellSize));
            foreach(int n2, n1.neighbors())
            {
                QPoint gc2((int)qFloor(graph->nodes().value(n2).properties().value(X).toDouble() / _cellSize), (int)qFloor(graph->nodes().value(n2).properties().value(Y).toDouble() / _cellSize));
                // increase the contact count for the grid cells
                increaseCellCount(gc1, time);
                if(gc1 != gc2)
                    increaseCellCount(gc2, time);
            }
        }
    }
}

QWidget *GridStatDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);
    qDebug() << "gridstatdecorator control widget";
    connect(ui->showGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShowGrid(bool)));
    return control;
}

void GridStatDecorator::increaseCellCount(QPoint cell, mvtime time)
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

void GridStatDecorator::deleteObsoleteCells(mvtime time)
{
    _contactCount.clear();
    for(auto cellIt = _gridCount.begin(); cellIt != _gridCount.end();) {
        int cellCount = 0;
        for(auto i = cellIt.value().begin(); i != cellIt.value().end();) {
            mvtime timeDiff = time - i.key();
            if(timeDiff > _timeWindow) {
                // erase the item in the hash
                qDebug() << i.key() << i.value();
                i = cellIt.value().erase(i);
            } else {
                cellCount += i.value();
                i++;
            }
        }

        // delete grid in hash table if there is no counts
        if(cellCount < 5) {
            cellIt = _gridCount.erase(cellIt);
        } else {
            _contactCount.insert(cellIt.key(), cellCount);
            cellIt++;
        }
    }
}

QColor GridStatDecorator::selectCellColor(int cellCount)
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

QList<double> GridStatDecorator::getJenksBreaks(QList<double> sListDouble, int sClassCount)
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

void GridStatDecorator::setShowGrid(bool show)
{
    _showGrid = show;
    _contactCount.clear();
    _gridGroupItems.clear();
}

inline uint qHash(const QPoint &key)
{
    return qHash(key.x()) ^ qHash(key.y());
}


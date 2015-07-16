#include "gridstatdecorator.h"
#include "ui_control.h"
#include "project.h"
#include "iloader.h"
#include <QFileDialog>

GridStatDecorator::GridStatDecorator():
    ui(new Ui::Control)
{
    _gridCount = QHash<QPoint,QLinkedList<QPair<mvtime,int> > >();
    _contactCount = QHash<QPoint,int>();
    _contacts = QMap<QPair<int,int>,ContactInfo*>();
    _timeWindow = 60*60; // 60 minutes
    _minContactDuration = 0; // 0 1/10 seconds
    _showGrid = true;
    _cellSize = 200;
    _communicationRange = 100;
    _gridGroupItems = new QGraphicsItemGroup();
}

void GridStatDecorator::setProject(Project * project)
{
    _project = project;
}

void GridStatDecorator::setCommunicationRange(int com)
{
    _communicationRange = com;
    _cellSize = 2*com;
    _contactCount.clear();
    emit requestUpdate();
}

void GridStatDecorator::paint(IGraph *graph)
{
    // Display the cell counts
    delete _gridGroupItems;
    _gridGroupItems = new QGraphicsItemGroup();
    _project->viewer()->addItem(_gridGroupItems);
    for(auto cellIt = _contactCount.begin(); cellIt != _contactCount.end(); ++cellIt) {
        int contactSum = cellIt.value();

        if(contactSum > 0) {
            GraphicsCellItem * item = new GraphicsCellItem(_cellSize*cellIt.key().x(), _cellSize*cellIt.key().y(), _cellSize, _cellSize);
            item->setBrush(QBrush(selectCellColor(contactSum)));
            item->setPen(Qt::NoPen);
            item->setOpacity(0.5);
            _gridGroupItems->addToGroup(item);
        }
    }
}

void GridStatDecorator::decorateEdges(mvtime time, IGraph *graph)
{
    if(_showGrid) {
        // update the grid count hash by deleting the obsolete cells
        deleteObsoleteCells(time);

        // Populate the grid
        QSet<QPair<int,int> > examinedContacts;
        foreach(const Node& n1, graph->nodes())
        {
            QPoint gc1((int)qFloor(n1.properties().value(X).toDouble() / _cellSize), (int)qFloor(n1.properties().value(Y).toDouble() / _cellSize));
            foreach(int n2, n1.neighbors())
            {
                if(n1.id() < n2)
                {
                    QPair<int,int> nodes = qMakePair(n1.id(),n2);
                     // add the contact to the examined contacts
                    examinedContacts.insert(nodes);

                    if(_contacts.contains(nodes)) {
                        ContactInfo * ci = _contacts.value(nodes);

                        // increment the contact duration
                        ci->setEndTime(time);

                        // display on the grid if the duration is greater than the minimum contact duration
                        if(ci->getDuration() >= _minContactDuration) {
                            QPoint gc2((int)qFloor(graph->nodes().value(n2).properties().value(X).toDouble() / _cellSize), (int)qFloor(graph->nodes().value(n2).properties().value(Y).toDouble() / _cellSize));
                            // increase the contact count for the grid cells
                            increaseCellCount(gc1, time);
                            if(gc1 != gc2)
                                increaseCellCount(gc2, time);
                        }
                    } else {
                        // add the contact to the hash
                        _contacts.insert(nodes,new ContactInfo(n1.id(),n2,time));
                    }
                }
            }
        }

        // delete all the contacts that have not been examined
        QSet<QPair<int,int> > contactsToRemove = _contacts.uniqueKeys().toSet().subtract(examinedContacts);
        for(auto it = contactsToRemove.begin(); it != contactsToRemove.end(); ++it) {
            ContactInfo *ci = _contacts.value(*it);
            if(_contacts.remove(*it))
                delete ci;
        }
    }
}


QWidget *GridStatDecorator::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);
    ui->labelTimeWindow->setText(QString::number(_timeWindow/60));
    ui->labelContactDuration->setText(QString::number(_minContactDuration) + "s");

    connect(ui->showGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShowGrid(bool)));
    connect(ui->timeWindowSlider, SIGNAL(valueChanged(int)), this, SLOT(setTimeWindow(int)));
    connect(ui->contactDurationSlider, SIGNAL(valueChanged(int)), this, SLOT(setMinContactDuration(int)));

    return control;
}

void GridStatDecorator::addDependancy(QObject *plugin)
{
    if(plugin->objectName() == "WComDecorator")
    {
        _wirelessCommunicationPlugin = plugin;
        connect(_wirelessCommunicationPlugin, SIGNAL(transmissionRangeChanged(int)), this, SLOT(setCommunicationRange(int)));
    }
}

void GridStatDecorator::increaseCellCount(QPoint cell, mvtime time)
{
    if(!_gridCount.contains(cell)) {
        _gridCount.insert(cell,QLinkedList<QPair<mvtime,int> >());
    }
    if(!_contactCount.contains(cell))
    {
        _contactCount.insert(cell, 0);
    }
    _contactCount[cell]++;
    if(_gridCount.value(cell).isEmpty() || !_gridCount.value(cell).back().first != time) {
        _gridCount[cell].append(QPair<mvtime, int>(time, 1));
    } else {
        _gridCount[cell].back().second++;
    }
}

void GridStatDecorator::deleteObsoleteCells(mvtime time)
{
    //_contactCount.clear();
    for(auto cellIt = _gridCount.begin(); cellIt != _gridCount.end(); ) {

        while(!cellIt.value().isEmpty() && cellIt.value().front().first < time - _timeWindow)
        {
            _contactCount[cellIt.key()] -= cellIt.value().front().second;
            cellIt.value().pop_front();
        }
        if(cellIt.value().isEmpty())
        {
            _contactCount.remove(cellIt.key());
            cellIt = _gridCount.erase(cellIt);
        }
        else
        {
            ++cellIt;
        }
    }
}

QColor GridStatDecorator::selectCellColor(int cellCount)
{
    // use quartiles
    // TODO: use natural jenks
    QList<int> l(_contactCount.values());
    std::sort(l.begin(), l.end());
    if(cellCount == 0 || l.isEmpty())
    {
        return QColor(255,255,255,0);
    }
    return QColor(0,0, 255, cellCount * 200 / l.last());
    /*int q12 = l[l.size()*1/4];
    int q23 = l[l.size()*2/4];
    int q34 = l[l.size()*3/4];

    if(cellCount > q34) return QColor(210,211,231);
    else if(cellCount > q23) return QColor(219,218,234);
    else if(cellCount > q12) return QColor(226,225,239);
    else return QColor(233,233,243);*/
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
    emit requestUpdate();
}

void GridStatDecorator::setTimeWindow(int timeWindow)
{
    _timeWindow = timeWindow*60;
    ui->labelTimeWindow->setText(QString::number(timeWindow));
    update();
}

void GridStatDecorator::update()
{
    // delete all the contacts
    qDeleteAll(_contacts);
    _contacts.clear();
    // delete all the contact counts
    _contactCount.clear();
    emit requestUpdate();
}

void GridStatDecorator::setMinContactDuration(int value)
{
    _minContactDuration = value;
    ui->labelContactDuration->setText(QString::number(value)+"s");
    update();
}


/**
 * qHash implementations
 */

inline uint qHash(const QPoint &key)
{
    return qHash(key.x()) ^ qHash(key.y());
}

inline uint qHash(const ContactInfo &key)
{
    QPair<int,int> nodes = key.getNodes();
    return qHash(nodes.first) ^ qHash(nodes.second);
}

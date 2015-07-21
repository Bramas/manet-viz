#ifndef GRIDSTATDECORATOR_H
#define GRIDSTATDECORATOR_H
#include <QObject>
#include <QRectF>
#include <QList>
#include <QPen>

#include "iplugin.h"
#include "pluginmanager.h"
#include "gridstatdecorator_global.h"
#include "types.h"
#include "graph.h"

namespace Ui {
class Control;
}

class GraphicsCellItem: public QGraphicsRectItem
{
public:
    GraphicsCellItem():
        QGraphicsRectItem() {}
    GraphicsCellItem(qreal x, qreal y, qreal w, qreal h):
        QGraphicsRectItem(x, y, w, h), _count(0) { }

    void setCount(int count) { _count = count; }
    int getCount() const { return _count; }
    void incCount() { _count++; }
    void incCount(int count) { _count += count; }
    void decCount() { _count--; }
    void decCount(int count) { _count -= count; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton) {
            event->accept();
            qDebug() << this->rect() << _count;
        } else {
            event->ignore();
        }
    }

private:
    int _count;
};

class ContactInfo
{
public:
    ContactInfo(int id1, int id2, mvtime t):
        _id1(id1), _id2(id2), _startTime(t), _endTime(t) { }
    ContactInfo():
        _id1(-1), _id2(-1), _startTime(-1), _endTime(-1) { }

    void setEndTime(mvtime endTime) { _endTime = endTime; }
    mvtime getDuration() const { return _endTime - _startTime; }
    QPair<int,int> getNodes() const { return qMakePair(_id1, _id2); }

private:
    mvtime _startTime;
    mvtime _endTime;
    int _id1;
    int _id2;
};

class GRIDSTATDECORATORSHARED_EXPORT GridStatDecorator: public QObject, public IPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "gridstatdecorator.json")
    Q_INTERFACES(IPlugin)

public:
    GridStatDecorator();
    void setProject(Project * project);

    void paint(IGraph *graph);
    void decorateEdges(mvtime time, IGraph *graph);
    QWidget * createControlWidget() const;

    QObject * getQObject() { return this; }
    virtual QString toString() const { return "GridStatDecorator"; }
    void addDependancy(QObject* plugin);

signals:
    void requestUpdate();

public slots:
    void setCommunicationRange(int);


private:
    void increaseCellCount(QPoint cell, mvtime time);
    void deleteObsoleteCells(mvtime time);
    QColor selectCellColor(int cellCount);
    QList<double> getJenksBreaks(QList<double> sListDouble, int sClassCount);
    void update();

    Ui::Control *ui;
    int _cellSize;
    bool _showGrid;
    QHash<QPoint,QLinkedList<QPair<mvtime,int> > > _gridCount;
    QHash<QPoint,GraphicsCellItem *> _contactCount;
    QMap<QPair<int,int>, ContactInfo> _contacts;
    mvtime _timeWindow;
    mvtime _minContactDuration;
    int _communicationRange;
    bool _isUpdating;

    QObject * _wirelessCommunicationPlugin;

private slots:
    void setShowGrid(bool);
    void setTimeWindow(int);
    void setMinContactDuration(int);

};

#endif // GRIDSTATDECORATOR_H

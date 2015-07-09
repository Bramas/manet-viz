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

    Ui::Control *ui;
    int _cellSize;
    bool _showGrid;
    QHash<QPoint,QLinkedList<QPair<mvtime,int> > > _gridCount;
    QHash<QPoint,int> _contactCount;
    mvtime _timeWindow;
    QGraphicsItemGroup * _gridGroupItems;
    int _communicationRange;

    QObject * _wirelessCommunicationPlugin;

private slots:
    void setShowGrid(bool);
    void setTimeWindow(int);
    void open();

};

#endif // GRIDSTATDECORATOR_H

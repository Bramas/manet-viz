#ifndef GRIDSTATDECORATOR_H
#define GRIDSTATDECORATOR_H
#include <QObject>
#include <QRectF>
#include <QList>
#include <QPen>

#include "iviewerlayer.h"
#include "pluginmanager.h"
#include "gridstatdecorator_global.h"
#include "types.h"
#include "graph.h"

namespace Ui {
class Control;
}

class GRIDSTATDECORATORSHARED_EXPORT GridStatDecorator: public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "gridstatdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    GridStatDecorator();
    void setProject(Project * project);

    void paint(IGraph *graph);
    void decorateEdges(mvtime time, IGraph *graph);
    QWidget * createControlWidget() const;

    QObject * getQObject() { return this; }
    virtual QString toString() const { return "GridStatDecorator"; }

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
    QList<QGraphicsItem*> _gridGroupItems;

private slots:
    void setShowGrid(bool);

};

#endif // GRIDSTATDECORATOR_H

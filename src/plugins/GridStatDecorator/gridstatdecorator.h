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

class GRIDSTATDECORATORSHARED_EXPORT GridStatDecorator: public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "gridstatdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    GridStatDecorator();
    void setProject(Project * project);

    QObject * getQObject() { return this; }
    virtual QString toString() const { return "GridStatDecorator"; }

private:
    int _transmissionRange = 500;
    QList<QRectF> _grid;
    QGraphicsItemGroup * _items;
    QPen p;
    QList<QGraphicsItem*> _gridGroupItems;

private slots:
    void setTransmissionRange(int);

};

#endif // GRIDSTATDECORATOR_H

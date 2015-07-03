#ifndef INTERPOLATIONDECORATOR_H
#define INTERPOLATIONDECORATOR_H

#include "interpolationdecorator_global.h"
#include <QObject>

#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph;

class INTERPOLATIONDECORATORSHARED_EXPORT InterpolationDecorator : public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "interpolationdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    InterpolationDecorator();

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    QObject * getQObject() { return this; }

    virtual QString toString() const { return "InterpolationDecorator"; }


};

#endif // INTERPOLATIONDECORATOR_H

#ifndef INTERPOLATIONDECORATOR_H
#define INTERPOLATIONDECORATOR_H

#include "interpolationdecorator_global.h"
#include <QObject>

#include "iplugin.h"
#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph;

class INTERPOLATIONDECORATORSHARED_EXPORT InterpolationDecorator : public QObject, public IPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "interpolationdecorator.json")
    Q_INTERFACES(IPlugin)

public:
    InterpolationDecorator();

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    QObject * getQObject() { return this; }

    virtual QString toString() const { return "InterpolationDecorator"; }


};

#endif // INTERPOLATIONDECORATOR_H

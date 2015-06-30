#ifndef INTERPOLATIONDECORATOR_H
#define INTERPOLATIONDECORATOR_H

#include "interpolationdecorator_global.h"
#include <QObject>

#include "igraphdecorator.h"
#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph;

class INTERPOLATIONDECORATORSHARED_EXPORT InterpolationDecorator : public QObject, public IGraphDecorator
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IGraphDecorator" FILE "interpolationdecorator.json")
    Q_INTERFACES(IGraphDecorator)

public:
    InterpolationDecorator();

    void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph);

    virtual void decorateEdges(mvtime time, IGraph *graph);
    virtual void decorateNodes(mvtime time, IGraph *graph);


    QObject * getQObject() { return this; }

    virtual QString toString() const { return "InterpolationDecorator"; }


private:
    const AbstractEvolvingGraph * _evolvingGraph;
};

#endif // INTERPOLATIONDECORATOR_H

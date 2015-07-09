#ifndef STATICNODES_H
#define STATICNODES_H

#include "staticnodes_global.h"
#include <QObject>

#include "iplugin.h"
#include "types.h"
#include "graph.h"

class STATICNODESSHARED_EXPORT StaticNodes : public QObject, public IPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "staticnodes.json")
    Q_INTERFACES(IPlugin)

public:
    StaticNodes();

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    void decorateGraphicsNode(const Node &n, GraphicsNodeItem * node) const;

    QObject * getQObject() { return this; }

    virtual QString toString() const { return "StaticNodes"; }
};

#endif // STATICNODES_H

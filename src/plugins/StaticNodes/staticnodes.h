#ifndef STATICNODES_H
#define STATICNODES_H

#include "staticnodes_global.h"
#include <QObject>

#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

class STATICNODESSHARED_EXPORT StaticNodes : public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "staticnodes.json")
    Q_INTERFACES(IViewerLayer)

public:
    StaticNodes();

    void decorateEdges(mvtime time, IGraph *graph);
    void decorateNodes(mvtime time, IGraph *graph);

    void decoratesGraphicsNode(const Node &n, GraphicsNodeItem * node) const;

    QObject * getQObject() { return this; }

    virtual QString toString() const { return "StaticNodes"; }
};

#endif // STATICNODES_H

#ifndef ABSTRACTLAYER_H
#define ABSTRACTLAYER_H

#include <QPointF>
#include <QGraphicsScene>

#include "viewer.h"
#include "node.h"
#include "abstractevolvinggraph.h"
#include "graphicsitems.h"

class QPainter;
class AbstractEvolvingGraph;
class IGraph;

class IViewerLayer
{
public:
    IViewerLayer() {}
    virtual ~IViewerLayer() {}

    virtual void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph) { _evolvingGraph = evolvingGraph; }
    virtual void setGraphicsScene(QGraphicsScene * scene) { _scene = scene; }
    virtual void paint(IGraph * graph) { return; }

    virtual void decorateEdges(mvtime time, IGraph *graph) { return; }
    virtual void decorateNodes(mvtime time, IGraph *graph) { return; }

    virtual QStringList requiredField() const { return QStringList(); }

    virtual QWidget * createControlWidget() const { return NULL; }
    virtual void decoratesGraphicsEdge(GraphicsEdgeItem * edge) const { return; }
    virtual void decoratesGraphicsNode(GraphicsNodeItem * node) const { return; }

    virtual QObject* getQObject() = 0;
    virtual QString toString() const { return ""; }

protected:
    const AbstractEvolvingGraph * _evolvingGraph;
    QGraphicsScene * _scene;
};

QT_BEGIN_NAMESPACE

#define IViewerLayer_iid "org.manet-viz.IViewerLayer"
Q_DECLARE_INTERFACE(IViewerLayer, IViewerLayer_iid)

QT_END_NAMESPACE

#endif // ABSTRACTLAYER_H

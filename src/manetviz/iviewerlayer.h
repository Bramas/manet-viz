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
class Project;
class QMenu;

class IViewerLayer
{
public:
    IViewerLayer() {}
    virtual ~IViewerLayer() {}

    virtual void addDependancy(QObject* plugin) { }

    virtual void setProject(Project * project) { _project = project; }
    virtual void paint(IGraph * graph) { return; }

    virtual void decorateEdges(mvtime time, IGraph *graph) { return; }
    virtual void decorateNodes(mvtime time, IGraph *graph) { return; }

    virtual QStringList requiredField() const { return QStringList(); }

    virtual QWidget * createControlWidget() const { return NULL; }
    virtual QMenu *   createMenu() const{ return NULL; }
    virtual void decoratesGraphicsEdge(GraphicsEdgeItem * edge) const { return; }
    virtual void decoratesGraphicsNode(const Node &n, GraphicsNodeItem * node) const { return; }

    virtual QObject* getQObject() = 0;
    virtual QString toString() const { return ""; }

protected:
    Project * _project;
};

QT_BEGIN_NAMESPACE

#define IViewerLayer_iid "org.manet-viz.IViewerLayer"
Q_DECLARE_INTERFACE(IViewerLayer, IViewerLayer_iid)

QT_END_NAMESPACE

#endif // ABSTRACTLAYER_H

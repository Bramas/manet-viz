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

class IPlugin
{
public:
    IPlugin() {}
    virtual ~IPlugin() {}

    virtual void addDependancy(QObject* plugin) { }

    virtual void setProject(Project * project) { _project = project; }
    virtual void paint(IGraph * graph) { return; }

    virtual void decorateEdges(mvtime time, IGraph *graph) { return; }
    virtual void decorateNodes(mvtime time, IGraph *graph) { return; }

    virtual QStringList requiredField() const { return QStringList(); }

    virtual QWidget * createControlWidget() const { return NULL; }
    virtual QMenu *   createMenu() const{ return NULL; }

    virtual void decorateGraphicsEdge(GraphicsEdgeItem * edge) const { return; }
    virtual void decorateGraphicsNode(const Node &n, GraphicsNodeItem * node) const { return; }

    virtual QObject* getQObject() = 0;
    virtual QString toString() const { return ""; }

protected:
    Project * _project;
};

QT_BEGIN_NAMESPACE

#define IPlugin_iid "org.manet-viz.IPlugin"
Q_DECLARE_INTERFACE(IPlugin, IPlugin_iid)

QT_END_NAMESPACE

#endif // ABSTRACTLAYER_H

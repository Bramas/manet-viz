#ifndef IGRAPHDECORATOR
#define IGRAPHDECORATOR

#include <QPointF>
#include "abstractevolvinggraph.h"
#include "node.h"
#include "graphicsitems.h"

class QGraphicsItem;

class IGraphDecorator
{
public:
    virtual ~IGraphDecorator() {}

    virtual void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph) = 0;

    virtual void decorateEdges(mvtime time, IGraph *graph) = 0;
    virtual void decorateNodes(mvtime time, IGraph *graph) = 0;

    virtual QStringList requiredField() const { return QStringList(); }

    virtual QWidget*        createControlWidget() const { return 0; }
    virtual void decoratesGraphicsEdge(GraphicsEdgeItem * edge) const { return; }
    virtual void decoratesGraphicsNode(GraphicsNodeItem * node) const { return; }

    virtual QObject* getQObject() = 0;
};



QT_BEGIN_NAMESPACE

#define IGraphDecorator_iid "org.manet-viz.IGraphDecorator"
Q_DECLARE_INTERFACE(IGraphDecorator, IGraphDecorator_iid)

QT_END_NAMESPACE



#endif // IGRAPHDECORATOR


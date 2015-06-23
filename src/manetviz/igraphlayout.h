#ifndef IGRAPHLAYOUT
#define IGRAPHLAYOUT

#include <QPointF>
#include "abstractevolvinggraph.h"
#include "node.h"

class IGraphLayout
{
public:
    virtual ~IGraphLayout() {}

    virtual void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph) = 0;

    virtual void footprint(mvtime time, IGraph *graph) const = 0;

    virtual QStringList requiredField() const = 0;
};



QT_BEGIN_NAMESPACE

#define IGraphLayout_iid "org.manet-viz.IGraphLayout"
Q_DECLARE_INTERFACE(IGraphLayout, IGraphLayout_iid)

QT_END_NAMESPACE


#endif // IGRAPHLAYOUT


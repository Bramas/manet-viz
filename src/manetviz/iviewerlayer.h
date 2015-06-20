#ifndef ABSTRACTLAYER_H
#define ABSTRACTLAYER_H

#include <QPointF>
#include <QGraphicsScene>

#include "viewer.h"
class QPainter;
class AbstractEvolvingGraph;

class IViewerLayer
{
public:
    IViewerLayer() {}
    virtual ~IViewerLayer() {}

    virtual void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph) { _evolvingGraph = evolvingGraph; }
    virtual void setGraphicsScene(QGraphicsScene * scene) { _scene = scene; }
    virtual void paint(mvtime time) = 0;

protected:
    const AbstractEvolvingGraph * _evolvingGraph;
    QGraphicsScene * _scene;
};

QT_BEGIN_NAMESPACE

#define IViewerLayer_iid "org.manet-viz.IViewerLayer"
Q_DECLARE_INTERFACE(IViewerLayer, IViewerLayer_iid)

QT_END_NAMESPACE

#endif // ABSTRACTLAYER_H

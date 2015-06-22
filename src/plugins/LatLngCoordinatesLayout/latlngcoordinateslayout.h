#ifndef LATLNGCOORDINATESLAYOUT_H
#define LATLNGCOORDINATESLAYOUT_H

#include <QObject>

#include "latlngcoordinateslayout_global.h"
#include "abstractevolvinggraph.h"
#include "igraphlayout.h"
#include "graph.h"

class LATLNGCOORDINATESLAYOUTSHARED_EXPORT LatLngCoordinatesLayout: public QObject, public IGraphLayout
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IGraphLayout" FILE "latlngcoordinateslayout.json")
    Q_INTERFACES(IGraphLayout)
public:
    LatLngCoordinatesLayout();

    void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph) { _evolvingGraph = evolvingGraph; }

    void footprint(mvtime time, IGraph *graph) const;

    QStringList requiredField() const
    {
        QStringList l;
        l << "X";
        l << "Y";
        return l;
    }

    const AbstractEvolvingGraph * _evolvingGraph;
};

#endif // LATLNGCOORDINATESLAYOUT_H

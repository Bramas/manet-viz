#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

#include <QObject>
#include <QString>

#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph : public QObject
{
    Q_OBJECT
public:
    AbstractEvolvingGraph();
    virtual ~AbstractEvolvingGraph();

    virtual bool load(QString filename) = 0;
    virtual qreal loadProgress() const = 0;
    virtual void cancelLoadAndWait() = 0;

    virtual Graph footprint(mvtime time) const = 0;

    virtual mvtime beginTime() const = 0;
    virtual mvtime endTime() const = 0;

signals:
    void onLoadProgressChanged(qreal);

};

#endif // ABSTRACTGRAPH_H

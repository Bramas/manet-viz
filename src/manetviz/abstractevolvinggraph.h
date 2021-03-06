#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

#include <QObject>
#include <QString>

#include "types.h"
#include "graph.h"
#include "node.h"

class ILoader;

class AbstractEvolvingGraph : public QObject
{
    Q_OBJECT
public:
    AbstractEvolvingGraph();
    virtual ~AbstractEvolvingGraph();

    virtual mvtime beginTime() const = 0;
    virtual mvtime endTime() const = 0;

    virtual const QHash<int, QMap<mvtime, NodeProperties>* > & nodes() const = 0;

    virtual void addNode(int id, mvtime time, QHash<QString, QVariant> props) = 0;

    virtual ILoader * loader() const { return _loader; }
    virtual void setLoader(ILoader * loader) { _loader = loader; }
protected:
    ILoader * _loader;
};

#endif // ABSTRACTGRAPH_H

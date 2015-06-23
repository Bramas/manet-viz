#ifndef WIRELESSCOMMUNICATIONDECORATOR_H
#define WIRELESSCOMMUNICATIONDECORATOR_H

#include "wirelesscommunicationdecorator_global.h"

#include <QObject>

#include "igraphdecorator.h"
#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph;

class WIRELESSCOMMUNICATIONDECORATORSHARED_EXPORT WirelessCommunicationDecorator : public QObject, public IGraphDecorator
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IGraphDecorator" FILE "wirelesscommunicationdecorator.json")
    Q_INTERFACES(IGraphDecorator)

public:
    WirelessCommunicationDecorator();

    void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph);

    virtual void decorateEdges(mvtime time, IGraph *graph);
    virtual void decorateNodes(mvtime time, IGraph *graph);

    QObject * getQObject() { return this; }

    QWidget * createControlWidget() const;
public slots:
    void setCommunicationRange(int);

signals:
    void requestUpdate();
private:
    const AbstractEvolvingGraph * _evolvingGraph;

    int _communicationRange;
};
#endif // WIRELESSCOMMUNICATIONDECORATOR_H

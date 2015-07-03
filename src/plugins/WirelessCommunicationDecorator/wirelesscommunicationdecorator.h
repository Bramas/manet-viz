#ifndef WIRELESSCOMMUNICATIONDECORATOR_H
#define WIRELESSCOMMUNICATIONDECORATOR_H

#include "wirelesscommunicationdecorator_global.h"

#include <QObject>

#include  "ui_control.h"
#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

class AbstractEvolvingGraph;

class WIRELESSCOMMUNICATIONDECORATORSHARED_EXPORT WirelessCommunicationDecorator : public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "wirelesscommunicationdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    WirelessCommunicationDecorator();


    virtual void decorateEdges(mvtime time, IGraph *graph);
    virtual void decorateNodes(mvtime time, IGraph *graph);

    QObject * getQObject() { return this; }

    QWidget * createControlWidget() const;
    void  decoratesGraphicsNode(GraphicsNodeItem * node) const;
    QString toString() const { return "WirelessCommunicationDecorator"; }

public slots:
    void setCommunicationRange(int);
    void setDisplayRange(bool);

signals:
    void requestUpdate();
    void transmissionRangeChanged(int);

private:
    Ui::Control *ui;
    bool _displayRange;
    int _communicationRange;

};
#endif // WIRELESSCOMMUNICATIONDECORATOR_H

#ifndef WCOMDECORATOR_H
#define WCOMDECORATOR_H

#include "wcomdecorator_global.h"

#include <QObject>
#include <QMultiHash>


#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

namespace Ui {
class Control;
}


class WCOMDECORATORSHARED_EXPORT WComDecorator: public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "wcomdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    WComDecorator();

    virtual void decorateEdges(mvtime time, IGraph *graph);

    QObject * getQObject() { return this; }

    QWidget * createControlWidget() const;
    void  decoratesGraphicsNode(GraphicsNodeItem * node) const;
    QString toString() const { return "WComDecorator"; }

public slots:
    void setCommunicationRange(int);
    void setDisplayRange(bool);

signals:
    void requestUpdate();
    void transmissionRangeChanged(int);

private:
    QList<QPoint> getNeighborCells(double x, double y);

    Ui::Control *ui;
    bool _displayRange;
    int _communicationRange;
    QMultiHash<QPoint,Node> _grid;
    int _cellSize;
    QPen _pen;

};

#endif // WCOMDECORATOR_H

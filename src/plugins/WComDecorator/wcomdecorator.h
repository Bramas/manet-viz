#ifndef WCOMDECORATOR_H
#define WCOMDECORATOR_H

#include "wcomdecorator_global.h"

#include <QObject>
#include <QMultiHash>

#include  "ui_control.h"
#include "iviewerlayer.h"
#include "types.h"
#include "graph.h"

class WCOMDECORATORSHARED_EXPORT WComDecorator: public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "wcomdecorator.json")
    Q_INTERFACES(IViewerLayer)

public:
    WComDecorator();
    void setEvolvingGraph(const AbstractEvolvingGraph * evolvingGraph);

    virtual void paint(IGraph * graph);
    void setGraphicsScene(QGraphicsScene * scene);

    virtual void decorateEdges(mvtime time, IGraph *graph);
    virtual void decorateNodes(mvtime time, IGraph *graph);

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
    QList<QPointF> getNeighborCells(double x, double y);
    void increaseCellCount(QPointF cell, mvtime time);
    void deleteObsoleteCells(mvtime time);
    QColor selectCellColor(int cellCount);
    QList<double> getJenksBreaks(QList<double> sListDouble, int sClassCount);

    const AbstractEvolvingGraph * _evolvingGraph;
    Ui::Control *ui;
    bool _displayRange;
    int _communicationRange;
    QMultiHash<QPointF,Node> _grid;
    QHash<QPointF,QHash<mvtime,int> > _gridCount;
    mvtime _timeWindow;
    int _cellSize;
    QHash<QPointF,int> _contactCount;
    QList<QGraphicsItem*> _gridGroupItems;
    QPen _pen;
    QRectF _oldSceneRect;

};

#endif // WCOMDECORATOR_H

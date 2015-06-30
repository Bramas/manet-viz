#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

#include "abstractevolvinggraph.h"
#include "iviewerlayer.h"

class Viewer;

class GraphLayer : public QObject, public IViewerLayer
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "graphlayer.json")
    Q_INTERFACES(IViewerLayer)

public:
    GraphLayer();
    void paint(IGraph * graph) Q_DECL_OVERRIDE;
    void setGraphicsScene(QGraphicsScene * scene);

    QWidget* createControlWidget() const;

    QGraphicsItem*  graphicsNodeChildItem(const Node &n) const { return 0; }
    void decoratesGraphicsEdge(GraphicsEdgeItem * node) const;

    QObject* getQObject() { return this; }
    virtual QString toString() const { return "GraphLayer"; }


public slots:
    void setDisplayContact(bool);

signals:
    void requestUpdate();

private:
    QGraphicsItemGroup * items;

    bool _displayContacts;

};

#endif // GRAPHVIEWER_H

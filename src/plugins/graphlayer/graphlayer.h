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
    void paint(mvtime time) Q_DECL_OVERRIDE;
    void setGraphicsScene(QGraphicsScene * scene);

private:
    QGraphicsItemGroup * items;
};

#endif // GRAPHVIEWER_H

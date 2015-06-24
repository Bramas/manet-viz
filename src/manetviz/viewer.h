#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsScene>
#include <QElapsedTimer>

#include "types.h"
#include "viewer.h"
class IViewerLayer;
class IGraphLayout;
class IGraphDecorator;
class AbstractEvolvingGraph;

class Viewer : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Viewer(const AbstractEvolvingGraph *evg);
    ~Viewer();

    QPointF toLocalCoordinates(QPointF globalCoordinates) const;
    void addLayer(IViewerLayer * layer, int priority = 100);

    mvtime time() const { return _time; }

    QList<IViewerLayer* > layers() const { return _layers.values(); }
    QList<IGraphDecorator* > graphDecorators() const { return _graphDecorators.values(); }



signals:

public slots:
    void setTime(mvtime time);
    void onPluginsChanged();
    void updateLayers();

protected:
    /*void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);*/

private:

    IGraphLayout * _layout;
    QMap<int, IGraphDecorator *> _graphDecorators;
    QMap<int, IViewerLayer*> _layers;
    mvtime _time;
    QPoint _lastMousePos;
    QPointF _afterTranslate;
    qreal _zoom;
    QElapsedTimer _timeSinceLastFrame;
    const AbstractEvolvingGraph * _evolvingGraph;
};

#endif // VIEWER_H

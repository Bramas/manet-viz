#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsScene>
#include <QElapsedTimer>

#include "types.h"
#include "viewer.h"
class IViewerLayer;
class IGraphLayout;
class AbstractEvolvingGraph;

class Viewer : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Viewer(const AbstractEvolvingGraph *evg);
    ~Viewer();

    QPointF toLocalCoordinates(QPointF globalCoordinates) const;
    void addLayer(IViewerLayer * layer, int priority = 100) { _layers.insertMulti(priority, layer); }

    mvtime time() const { return _time; }



signals:

public slots:
    void setTime(mvtime time);
    void onPluginsChanged();

protected:
    /*void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);*/

private:


    IGraphLayout * _layout;
    QMap<int, IViewerLayer*> _layers;
    mvtime _time;
    QPoint _lastMousePos;
    QPointF _afterTranslate;
    qreal _zoom;
    QElapsedTimer _timeSinceLastFrame;
    const AbstractEvolvingGraph * _evolvingGraph;
};

#endif // VIEWER_H

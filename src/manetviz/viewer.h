#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsScene>
#include <QElapsedTimer>

#include "types.h"
#include "viewer.h"

class Project;
class IViewerLayer;
class IGraphLayout;
class AbstractEvolvingGraph;

class Viewer : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Viewer();
    ~Viewer();

    void setProject(Project * project);

    QPointF toLocalCoordinates(QPointF globalCoordinates) const;

    mvtime time() const { return _time; }

signals:
    void mousePressedEvent(QGraphicsSceneMouseEvent *);

public slots:
    void setTime(mvtime time);
    void onPluginsChanged();
    void updateLayers();

protected:
    /*void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseMoveEvent(QMouseEvent *);*/
    void mousePressEvent(QGraphicsSceneMouseEvent * e) { emit mousePressedEvent(e); }

private:

    IGraphLayout * _layout;
    mvtime _time;
    QPoint _lastMousePos;
    QPointF _afterTranslate;
    qreal _zoom;
    QElapsedTimer _timeSinceLastFrame;
    Project * _project;
    QGraphicsItemGroup * _items;
};

#endif // VIEWER_H

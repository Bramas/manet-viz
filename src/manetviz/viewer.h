#ifndef VIEWER_H
#define VIEWER_H

#include <QGraphicsScene>
#include <QElapsedTimer>
#include <QtConcurrent>
#include <QFutureWatcher>

#include "types.h"
#include "viewer.h"

class Project;
class IViewerLayer;
class IGraphLayout;
class AbstractEvolvingGraph;
class IGraph;

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

public slots:
    void setTime(mvtime time);
    void onPluginsChanged();
    void onUpdateRequested();

protected:
    /*void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);*/

private:
    void updateLayers();
    IGraph * prepareUpdate();
    QFuture<IGraph*> _futureGraph;
    QFutureWatcher<IGraph*> _futureGraphWatcher;
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

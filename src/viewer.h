#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>

#include "types.h"
class AbstractLayer;

class Viewer : public QWidget
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();

    QPointF toLocalCoordinates(QPointF globalCoordinates) const;
    void addLayer(AbstractLayer * layer, int priority = 100) { _layers.insertMulti(priority, layer); }

    mvtime time() const { return _time; }

signals:

public slots:
    void setTime(mvtime time);

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private:

    QMap<int, AbstractLayer*> _layers;
    mvtime _time;
    QPoint _lastMousePos;
    QPointF _afterTranslate;
    qreal _zoom;
};

#endif // VIEWER_H

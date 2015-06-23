#ifndef GRAPHICSVIEWER_H
#define GRAPHICSVIEWER_H

#include <QGraphicsView>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QGraphicsScene * scene, QWidget * parent = 0);

protected:
    void wheelEvent(QWheelEvent *);

    qreal _scale;
};

#endif // GRAPHICSVIEWER_H

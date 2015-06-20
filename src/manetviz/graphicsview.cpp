#include "graphicsview.h"

#include <QDebug>

GraphicsView::GraphicsView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(scene, parent)
{
    _scale = 1;
}


void GraphicsView::wheelEvent(QWheelEvent * e)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(e->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);

    } else {
        // Zooming out
         scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

}

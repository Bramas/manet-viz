#include "gridstatdecorator.h"

GridStatDecorator::GridStatDecorator()
{
//    QObject * instance = PluginManager::getPlugin("WirelessCommunicationDecorator");
//    if(instance) {
//        connect(instance, SIGNAL(transmissionRangeChanged(int)), this, SLOT(setTransmissionRange(int)));
//    }

}

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;
    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    return numToRound + multiple - remainder;
}

void GridStatDecorator::setEvolvingGraph(const AbstractEvolvingGraph *evolvingGraph)
{

    _evolvingGraph = evolvingGraph;

    // Get the bounding box of the initial topology
    QPointF topLeft(MaxDist,MaxDist);
    QPointF bottomRight(-MaxDist,-MaxDist);

    foreach(auto id, _evolvingGraph->nodes().keys()) {
        auto nodeTimeline = _evolvingGraph->nodes().value(id);
        for(auto it = nodeTimeline->begin(); it != nodeTimeline->end(); ++it) {
            double x = it.value().value(X).toDouble();
            double y = it.value().value(Y).toDouble();
            if(x > bottomRight.x()) { bottomRight.setX(x); }
            if(y > bottomRight.y()) { bottomRight.setY(y); }
            if(x < topLeft.x())     { topLeft.setX(x);     }
            if(y < topLeft.y())     { topLeft.setY(y);     }
        }
    }

    // create a grid over the evolving graph
    QPointF center(topLeft.x() + (bottomRight.x() - topLeft.x()) / 2, topLeft.y() + (bottomRight.y() - topLeft.y()) / 2);
    double bufferedLength = roundUp(1.5 * (bottomRight.x() - topLeft.x()), _transmissionRange);
    int nbXCells = bufferedLength / _transmissionRange;
    double bufferedHeight = roundUp(1.5 * (bottomRight.y() - topLeft.y()), _transmissionRange);
    int nbYCells = bufferedHeight / _transmissionRange;
    QPointF gridTopLeft(center.x() - (bufferedLength/2),  center.y() - (bufferedHeight/2));

    for(int i = 0; i<nbXCells; i++) {
        for(int j = 0; j<nbYCells; j++) {
            _grid.append(QRectF(QPointF(gridTopLeft.x() + i*_transmissionRange, gridTopLeft.y() + j*_transmissionRange),
                                QPointF(gridTopLeft.x() + (i+1)*_transmissionRange, gridTopLeft.y() + (j+1)*_transmissionRange)));
        }
    }

    // get the transmission range
    // see: https://www.cfa.harvard.edu/~fbecerra/pavoreal/algorithm.html
    // binning algorithm
    // real-time collision detection
    // hexgrid http://www.redblobgames.com/ || http://www.redblobgames.com/grids/hexagons/implementation.html

}

void GridStatDecorator::setGraphicsScene(QGraphicsScene *scene)
{
    _scene = scene;

    p = QPen(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);

    foreach(auto rect, _grid) {
        QGraphicsRectItem* item = new QGraphicsRectItem(rect);
        item->setPen(p);
        _gridGroupItems.append(item);
    }

    _scene->createItemGroup(_gridGroupItems);
}

void GridStatDecorator::setTransmissionRange(int range)
{
    _transmissionRange = range;
    qDebug() << "Changed transmission range: " << range;
}

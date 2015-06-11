#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>

#include "abstractevolvinggraph.h"

class GraphViewer : public QWidget
{
    Q_OBJECT
public:
    GraphViewer(const AbstractEvolvingGraph * graph);
    QPointF toLocalCoordinates(QPointF globalCoordinates) const;

public slots:
    void setTime(mvtime time);
protected:
    void paintEvent(QPaintEvent *);

private:
    const AbstractEvolvingGraph * _evolvingGraph;

    mvtime _time;
};

#endif // GRAPHVIEWER_H

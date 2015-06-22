#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QWidget>

#include "abstractevolvinggraph.h"
#include "iviewerlayer.h"
class Viewer;

class GraphLayer : public IViewerLayer
{

public:
    GraphLayer(Viewer * viewer, const AbstractEvolvingGraph * graph);
    void paint(QPainter *);
    void setDisplayRange(bool);
    void setDisplayContact(bool);

private:
    const AbstractEvolvingGraph * _evolvingGraph;
    bool _displayContacts = false;
    bool _displayRange = false;
};

class LoaderLayer : public IViewerLayer
{
public:
    LoaderLayer(Viewer * viewer, const AbstractEvolvingGraph * graph);
    void paint(QPainter *);

private:
    const AbstractEvolvingGraph * _evolvingGraph;

};

#endif // GRAPHVIEWER_H

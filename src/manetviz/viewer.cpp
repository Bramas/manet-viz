#include "viewer.h"
#include "iviewerlayer.h"
#include "igraphlayout.h"
#include "pluginmanager.h"
#include "project.h"
#include "iloader.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>
#include <QGLWidget>
#include <QGraphicsItem>
#include <QPointF>
#include <QDebug>

Viewer::Viewer()
{
    _afterTranslate = QPointF(-83000, -12.35*2000);
    _layout = 0;
    _zoom=2000;
    //setMouseTracking(true);
    _timeSinceLastFrame.start();
}

Viewer::~Viewer()
{

}
void Viewer::setProject(Project *project)
{
    _project = project;
    connect(&PluginManager::instance, SIGNAL(pluginsChanged()), this, SLOT(onPluginsChanged()));
    onPluginsChanged();
}


QPointF Viewer::toLocalCoordinates(QPointF globalCoordinates) const
{
    return _afterTranslate + _zoom * (globalCoordinates);
}

void Viewer::setTime(mvtime time)
{
    _time = time + _project->loader()->constEvolvingGraph()->beginTime();
    updateLayers();
}
void Viewer::updateLayers()
{
    if(!_layout) {
        qDebug() << "no layout";
        return;
    }
    IGraph * graph = _project->constructSnapshot(_time);


    _layout->footprint(_time, graph);

//    this->clear();

    if(this->items().contains(_items)) {
        this->removeItem(_items);
        delete _items;
    }

    _items = new QGraphicsItemGroup();

    // set pen to display edges
    QPen p(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);

    foreach(const Node& n1, graph->nodes())
    {
        foreach(int n2, n1.neighbors())
        {
            if(n1.id() < n2)
            {
                QPointF p1(n1.properties().value(X).toDouble(), n1.properties().value(Y).toDouble());
                QPointF p2(graph->nodes().value(n2).properties().value(X).toDouble(), graph->nodes().value(n2).properties().value(Y).toDouble());
                QPointF middle = (p1 + p2) / 2.0;
                GraphicsEdgeItem * edge = new GraphicsEdgeItem(p1 - middle, p2 - middle);
                edge->setPen(p);
                edge->setPos(middle);

                foreach(auto layer, _project->layers())
                {
                    layer->decoratesGraphicsEdge(edge);
                }

                _items->addToGroup(edge);
            }
        }
    }

    // Set pen to dsiplay nodes
    p.setColor(QColor(0,0,0));
    p.setWidthF(4);

    foreach(const Node &n, graph->nodes())
    {
        QPointF position(n.properties().value(X).toDouble(), n.properties().value(Y).toDouble());
        GraphicsNodeItem * node = new GraphicsNodeItem();
        node->setPen(p);
        node->setPos(position);
        foreach(auto layer, _project->layers())
        {
            layer->decoratesGraphicsNode(node);
        }
        _items->addToGroup(node);
    }
    this->addItem(_items);
    update();

    foreach(auto layer, _project->layers())
    {
        layer->paint(graph);
    }
    update();
}


void Viewer::onPluginsChanged()
{
    foreach(IGraphLayout * layout, PluginManager::getObjects<IGraphLayout>())
    {
         layout->setProject(_project);
         _layout = layout;
         break;
    }
}

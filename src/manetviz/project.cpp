#include "project.h"
#include "graph.h"
#include "iviewerlayer.h"
#include "pluginmanager.h"
#include "iloader.h"

Project::Project(Viewer *viewer, ILoader *loader, QMenuBar *menuBar)
{
    _viewer = viewer;
    _loader = loader;
    _menuBar = menuBar;
    _viewer->setProject(this);
    _loader->setProject(this);

    connect(&PluginManager::instance, SIGNAL(pluginsChanged()), this, SLOT(onPluginsChanged()));
    onPluginsChanged();
}


Project::~Project()
{

}

IGraph * Project::constructSnapshot(mvtime time) const
{
    IGraph * graph = new Graph();

    foreach(auto layer, _layers)
    {
        layer->decorateNodes(time, graph);
    }
    foreach(auto layer, _layers)
    {
        layer->decorateEdges(time, graph);
    }
    return graph;
}

void Project::addLayer(IViewerLayer *layer, int priority)
{
    if(layer->getQObject()->metaObject()->indexOfSignal("requestUpdate()") != -1)
    {
        connect(layer->getQObject(), SIGNAL(requestUpdate()), _viewer, SLOT(onUpdateRequested()));
    }
    _layers.insertMulti(priority, layer);
}

void Project::onPluginsChanged()
{
    int i = 0;
    foreach(IViewerLayer * layer, PluginManager::getObjects<IViewerLayer>())
    {
        layer->setProject(this);
        QMenu * menu = layer->createMenu();
        if(menu) {
            _menuBar->addMenu(menu);
        }
        addLayer(layer, i++);
    }
}

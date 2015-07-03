#include "graphconvertor.h"
#include "iloader.h"
#include <QPushButton>

GraphConvertor::GraphConvertor()
{
}

QWidget* GraphConvertor::createControlWidget() const
{
    QPushButton * widget = new QPushButton("Convert");
    widget->setEnabled(false);
    connect(this, &GraphConvertor::graphLoaded, widget, &QPushButton::setEnabled);
    if(_evolvingGraph &&  _evolvingGraph->loader() && _evolvingGraph->loader()->isLoaded())
    {
        widget->setEnabled(true);
    }
    return widget;
}

void GraphConvertor::onGraphLoaded()
{
    emit graphLoaded(true);
}

void GraphConvertor::setEvolvingGraph(const AbstractEvolvingGraph *evolvingGraph)
{
    _evolvingGraph = evolvingGraph;
    if(_evolvingGraph->loader())
    {
        connect(_evolvingGraph->loader()->getQObject(), SIGNAL(loaded()), this, SLOT(onGraphLoaded()));
    }
}

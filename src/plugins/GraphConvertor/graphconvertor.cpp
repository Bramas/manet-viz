#include "graphconvertor.h"
#include "iloader.h"
#include "project.h"
#include <QPushButton>

GraphConvertor::GraphConvertor()
{
}

QWidget* GraphConvertor::createControlWidget() const
{
    QPushButton * widget = new QPushButton("Convert");
    widget->setEnabled(false);
    connect(this, &GraphConvertor::graphLoaded, widget, &QPushButton::setEnabled);
    if(_project->loader()->isLoaded())
    {
        widget->setEnabled(true);
    }
    return widget;
}

void GraphConvertor::onGraphLoaded()
{
    emit graphLoaded(true);
}

void GraphConvertor::setProject(Project * project)
{
    _project = project;
    connect(_project->loader()->getQObject(), SIGNAL(loaded()), this, SLOT(onGraphLoaded()));
}

void GraphConvertor::convert()
{
    //_project->constructSnapshot(_time);
}


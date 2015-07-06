#include "graphconvertor.h"
#include "iloader.h"
#include "project.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>

GraphConvertor::GraphConvertor()
{
}

QWidget* GraphConvertor::createControlWidget() const
{
    QPushButton * widget = new QPushButton("Convert");
    widget->setEnabled(false);
    connect(this, &GraphConvertor::graphLoaded, widget, &QPushButton::setEnabled);
    connect(widget, &QPushButton::clicked, this, &GraphConvertor::convert);
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
    if(!_project->loader()->isLoaded())
    {
        qDebug() << "graph is not fully loaded ";
        return;
    }
    const AbstractEvolvingGraph * evg =  _project->loader()->constEvolvingGraph();

    QString filename = QFileDialog::getSaveFileName();
    if(filename.isEmpty())
        return;

    qDebug()<<"convertion => "<<filename;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << "Unable to write in file "<<filename;
        return;
    }
    QDataStream out(&file);

    out << evg->beginTime();
    out << evg->endTime();
    qDebug()<<evg->beginTime()<<" "<<evg->endTime();
    for(mvtime i = evg->beginTime(); i < evg->endTime(); ++i)
    {
        IGraph * snapshot = _project->constructSnapshot(i);
        QList<QPair<int, int> > edges;
        foreach(const Node &n1, snapshot->nodes())
        {
            foreach(int n2, n1.neighbors())
            {
                if(n1.id() < n2)
                {
                    edges << QPair<int,int>(n1.id(), n2);
                }
            }
        }
        if(edges.size())
        {
            out << i << edges.size();
            foreach(auto p, edges)
            {
                out << p.first << p.second;
            }
        }
        if(!(i%1000))
        {
            qDebug()<<((i - evg->beginTime())/qreal(evg->endTime() - evg->beginTime()));
        }
        delete snapshot;
    }
    qDebug()<<"DONE";
    //_project->constructSnapshot(_time);
}


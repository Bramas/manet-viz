#include "geometricgraph.h"
#include <iostream>
#include <memory>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtConcurrent>

GeometricGraph::GeometricGraph()
{
    _loaded = 0;
}
GeometricGraph::~GeometricGraph(){

}
/*
void GeometricGraph::concurrentLoad(std::unique_ptr<QFile> file)
{

}*/

bool GeometricGraph::load(QString filename)
{
    std::unique_ptr<QFile> file(new QFile(filename));
    if(!file->open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    qDebug()<<file->bytesAvailable();
    for(int i = 0; i< 100000; ++i)
    {
        if(!file->atEnd())
            processLine(QString(file->readLine()));
    }

    _loaded = file->bytesAvailable() / file->size();
    //QtConcurrent::run()
    //foreach(mvtime t, _nodesPosition.value(89)->keys())
    //    qDebug()<<t<<" "<<_nodesPosition.value(89)->value(t);
    return true;
}

void GeometricGraph::processLine(QString line)
{
    QStringList l = line.split(";");
    if(l.size() != 3)
    {
        return;
    }
    int id = l.at(0).toInt();

    QStringList date = l.at(1).split(" ").at(0).split("-");
    QStringList time = l.at(1).split(" ").at(1).split("+").at(0).split(":");

    mvtime t = date.at(2).toInt()*3600*24+time.at(0).toInt()*3600+time.at(1).toInt()*60+time.at(2).toInt();

    QStringList pointPart = l.at(2).split(")").at(0).split("(").at(1).split(" ");
    QPointF p(pointPart.at(0).toDouble(), pointPart.at(1).toDouble());

    if(!_nodesPosition.contains(id))
    {
        _nodesPosition.insert(id, new QMap<mvtime, QPointF>());
    }
    _nodesPosition.value(id)->insert(_nodesPosition.value(id)->constEnd(), t, p);

}

typedef QMap<mvtime, QPointF> NodePositions;
Graph GeometricGraph::footprint(mvtime time) const
{
    Graph g;
    foreach(int id, _nodesPosition.keys())
    {
        NodePositions * nodePos = _nodesPosition.value(id);
        NodePositions::const_iterator up = nodePos->lowerBound(time);
        QPointF p = up.value();
        if(up != nodePos->constBegin() && abs(up.key() - (up - 1).key()) < 300)
        {
            NodePositions::const_iterator low = up - 1;
            p = (up.key() - time)*low.value() + (time - low.key())*up.value();
            p /= (up.key() - low.key());
            g.addNode(id, p);
        }
        //qDebug()<<p;
    }
    return g;
}


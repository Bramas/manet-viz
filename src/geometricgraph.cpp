#include "geometricgraph.h"
#include <iostream>
#include <memory>
#include <ctime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVector2D>

GeometricGraph::GeometricGraph()
{
    _loaded = 0;
    _forceStop = false;
    _communicationRange = 1;
}
GeometricGraph::~GeometricGraph(){
    _forceStop = true;
}

bool GeometricGraph::concurrentLoad(QFile * file)
{
    while(!file->atEnd())
    {
        for(int i = 0; i< 10000; ++i)
        {
            if(!file->atEnd())
                processLine(QString(file->readLine()));
        }
        _loaded = 1.0 - file->bytesAvailable() / (qreal)file->size();
        emit onLoadProgressChanged(_loaded);
        if(_forceStop)
        {
            return false;
        }
    }

    file->close();
    file->deleteLater();
    return true;
}
void GeometricGraph::cancelLoadAndWait()
{
    if(!_loadResult.isFinished())
    {
        _forceStop = true;
        _loadResult.waitForFinished();
    }
}

bool GeometricGraph::load(QString filename)
{
    QFile * file = new QFile(filename);
    if(!file->open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    for(int i = 0; i< 100; ++i)
    {
        if(!file->atEnd())
            processLine(QString(file->readLine()));
    }

    _loaded = 1.0 - file->bytesAvailable() / (qreal)file->size();
    _loadResult = QtConcurrent::run(this, &GeometricGraph::concurrentLoad, file);
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
    QStringList time = l.at(1).split(" ").at(1).split(".").at(0).split(":");

    struct std::tm origin;
    origin.tm_year = 2014 - 1900;
    origin.tm_mon = 2 - 1;
    origin.tm_mday = 1;
    origin.tm_hour = 0;
    origin.tm_min = 0;
    origin.tm_sec = 0;

    struct std::tm tm;
    tm.tm_year = date.at(0).toInt() - 1900;
    tm.tm_mon = date.at(1).toInt() - 1;
    tm.tm_mday = date.at(2).toInt();
    tm.tm_hour = time.at(0).toInt();
    tm.tm_min = time.at(1).toInt();
    tm.tm_sec = time.at(2).toInt();

    mvtime t = static_cast<mvtime> (std::mktime (&tm)) - static_cast<mvtime> (std::mktime (&origin));
    //qDebug()<<std::difftime(std::mktime (&tm), std::mktime (&origin));
    //qDebug()<<l.at(1)<<" => "<<t;//<<static_cast<mvtime> (std::mktime (&origin))<<static_cast<mvtime> (std::mktime (&tm));
        //date.at(1).toInt()*3600*24*31+date.at(2).toInt()*3600*24+time.at(0).toInt()*3600+time.at(1).toInt()*60+time.at(2).toInt();

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
        if(up != nodePos->constBegin() && abs(up.key() - (up - 1).key()) < 300)
        {
            NodePositions::const_iterator low = up - 1;
            QPointF p = (up.key() - time)*low.value() + (time - low.key())*up.value();
            p /= (up.key() - low.key());
            g.addNode(id, p);
        }
        else if(up.key() == time)
        {
            g.addNode(id, up.value());
        }
    }

    foreach(const Node& n1, g.nodes())
    {
        foreach(const Node& n2, g.nodes())
        {
            QVector2D v1(n1.position());
            if(v1.distanceToPoint(QVector2D(n2.position())) <= _communicationRange/1000.0)
            {
                g.addEdge(n1,n2);
            }
        }
    }
    return g;
}

void GeometricGraph::setCommunicationRange(qreal range)
{
    _communicationRange = range;
}

#ifndef GTFSLOADER_H
#define GTFSLOADER_H

#include <QObject>
#include "abstractevolvinggraph.h"
#include "types.h"

class WayPoint
{
public:
    WayPoint():
        _coords(QPointF()) {}
    WayPoint(QPointF coords):
        _coords(coords) {}

    QPointF getCoords() {return _coords;}

protected:
    QPointF _coords;
};

class Stop: public WayPoint
{
public:
    Stop():
        WayPoint(), _id(""), _name("") {}
    Stop(QString id, QString name, QPointF coords):
        WayPoint(coords), _id(id), _name(name) {}

    QString toString() {
        return QString("%1 - %2 (%3, %4)").arg(_id).arg(_name).arg(_coords.x()).arg(_coords.y());
    }

private:
    QString _id;
    QString _name;
};

class Trip
{
public:
    Trip():
        _routeId(""), _serviceId(""), _tripId(""), _tripHeadsign(""), _directionId(""), _shapeId("") {}
    Trip(QString routeId, QString serviceId, QString tripId, QString tripHeadsign, QString directionId, QString shapeId):
        _routeId(routeId),
        _serviceId(serviceId),
        _tripId(tripId),
        _tripHeadsign(tripHeadsign),
        _directionId(directionId),
        _shapeId(shapeId) {}
    Trip(Trip &t):
        _routeId(t._routeId),
        _serviceId(t._serviceId),
        _tripId(t._tripId),
        _tripHeadsign(t._tripHeadsign),
        _directionId(t._directionId),
        _shapeId(t._shapeId) {}

    QString getRouteId() const { return _routeId; }
    QString getServiceId() const { return _serviceId; }

protected:
    QString _routeId;
    QString _serviceId;
    QString _tripId;
    QString _tripHeadsign;
    QString _directionId;
    QString _shapeId;

};

class Trajectory: public Trip
{
public:
    Trajectory():
        Trip() {}

    Trajectory(QString routeId, QString serviceId, QString tripId, QString tripHeadsign, QString directionId, QString shapeId):
        Trip(routeId, serviceId, tripId, tripHeadsign, directionId, shapeId),
        _trajectory(QMap<mvtime, WayPoint*>()) {}
    Trajectory(Trip t):
        Trip(t),
        _trajectory(QMap<mvtime, WayPoint*>()) {}

    QMap<mvtime, WayPoint*> getTrajectory() { return _trajectory; }

    // returns whether the vehicle is active at timestamp time
    bool isActive(mvtime time) {
        return time <= _trajectory.lastKey() && time >= _trajectory.firstKey();
    }

    void addWayPoint(mvtime time, WayPoint * p) {
        _trajectory.insert(time, p);
    }

    QString toString() {
        QString ret;
        return QString("%1 / %2 / %3 / %4").arg(_tripId).arg(_serviceId).arg(_routeId).arg(_trajectory.count());
        return ret;
    }

private:
    QMap<mvtime, WayPoint *> _trajectory;
};

class GTFSLoader
{
public:
    // initialize the GTFS loader to parse into the traj variable
    GTFSLoader(QString folderPath);
    ~GTFSLoader();

    AbstractEvolvingGraph * evolvingGraph() const;

public slots:
    void load();

private:
    QString _folderPath;
    QString _stopTimesFilePath;
    QString _stopsFilePath;
    QString _shapesFilePath;
    QString _tripsFilePath;

    // trajectories indexed by the trip id of each trajectory
    QMap<QString,Trajectory *> _trajectories;

    AbstractEvolvingGraph * _evolvingGraph;

    void parseTrips();
    mvtime toSeconds(QString time);
};

#endif // GTFSLOADER_H

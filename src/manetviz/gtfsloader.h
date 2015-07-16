#ifndef GTFSLOADER_H
#define GTFSLOADER_H

#include <QObject>
#include "abstractevolvinggraph.h"
#include "types.h"
#include "iloader.h"

#include "/usr/local/include/proj_api.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/linearref/LinearLocation.h>
#include <geos/linearref/LocationIndexedLine.h>
#include <geos/linearref/ExtractLineByLocation.h>

using namespace geos;
using namespace geos::geom;
using namespace geos::linearref;

class Stop
{
public:
    Stop():
        _id(""), _name(""), _coords(QPointF()) {}
    Stop(QString id, QString name, QPointF coords):
        _id(id), _name(name), _coords(coords) {}

    QPointF getCoords() {return _coords;}
    void setCoords(double x, double y) { _coords = QPointF(x,y); }
    void setCoords(QPointF p) { _coords = QPointF(p); }

    QString toString() {
        return QString("%1 - %2 (%3, %4)").arg(_id).arg(_name).arg(_coords.x()).arg(_coords.y());
    }

private:
    QString _id;
    QString _name;
    QPointF _coords;
};

class WayPoint
{
public:
    WayPoint():
        _coords(QPointF()), _departureTime(0), _arrivalTime(0) {}
    WayPoint(QPointF coords):
        _coords(coords), _departureTime(0), _arrivalTime(0) {}
    WayPoint(Stop * stop):
        _coords(stop->getCoords()), _departureTime(0), _arrivalTime(0), _stop(stop) {}
    WayPoint(Stop * stop, mvtime departureTime, mvtime arrivalTime):
        _coords(stop->getCoords()), _departureTime(departureTime), _arrivalTime(arrivalTime), _stop(stop) {}
    WayPoint(QPointF coords, mvtime departureTime, mvtime arrivalTime, Stop * stop = NULL):
        _coords(coords), _departureTime(departureTime), _arrivalTime(arrivalTime), _stop(stop) {}

    QPointF getCoords() {return _coords;}
    void setCoords(double x, double y) { _coords = QPointF(x,y); }
    void setCoords(QPointF p) { _coords = QPointF(p); }
    void setTimes(mvtime departureTime, mvtime arrivalTime) { _departureTime = departureTime; _arrivalTime = arrivalTime; }
    mvtime getDepartureTime() { return _departureTime; }
    mvtime getArrivalTime() { return _arrivalTime; }

protected:
    QPointF _coords;
    mvtime _departureTime;
    mvtime _arrivalTime;
    Stop * _stop;
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
    QString getShapeId() const { return _shapeId; }

protected:
    QString _routeId;
    QString _serviceId;
    QString _tripId;
    QString _tripHeadsign;
    QString _directionId;
    QString _shapeId;

};

class GTFSLoader;

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

    void addWayPoint(WayPoint * p) {
        _trajectory.insert(p->getArrivalTime(), p);
    }

    QMap<mvtime, WayPoint *> getTrajectory() const { return _trajectory; }

    QString toString() {
        QString ret;
        return QString("%1 / %2 / %3 / %4").arg(_tripId).arg(_serviceId).arg(_routeId).arg(_trajectory.count());
        return ret;
    }

    friend class GTFSLoader;

private:
    QMap<mvtime, WayPoint *> _trajectory;
};


class GTFSLoader : public QObject, public ILoader
{
    Q_OBJECT
public:
    GTFSLoader(QString folderPath);
    GTFSLoader(QString folderPath, QString projIn, QString projOut, bool snapToShape);
    GTFSLoader(const GTFSLoader &other);
    ~GTFSLoader();

    AbstractEvolvingGraph * evolvingGraph() const;
    const AbstractEvolvingGraph * constEvolvingGraph() const;
    QObject * getQObject() { return this; }
    QString getType() const { return "GTFSLoader"; }
    QString getPath() const { return _folderPath; }
    bool isLoaded() const { return true; }

signals:
    void onLoaded();

public slots:
    void load();

private:
    QString _folderPath;
    QString _stopTimesFilePath;
    QString _stopsFilePath;
    QString _shapesFilePath;
    QString _tripsFilePath;
    bool _snapToShape;

    // trajectories indexed by the trip id of each trajectory
    QMap<QString,Trajectory *> _trajectories;

    AbstractEvolvingGraph * _evolvingGraph;

    void parseTrips();
    mvtime toSeconds(QString time);
};

#endif // GTFSLOADER_H

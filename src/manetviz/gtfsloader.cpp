#include "gtfsloader.h"
#include "csvparser.h"
#include "evolvinggraph.h"

// TODO: Do a script to get the extension (either .csv or .txt)

GTFSLoader::GTFSLoader(QString folderPath):
    _folderPath(folderPath),
    _stopTimesFilePath(folderPath+"/stop_times.csv"),
    _stopsFilePath(folderPath+"/stops.csv"),
    _shapesFilePath(folderPath+"/shapes.csv"),
    _tripsFilePath(folderPath+"/trips.csv")
{
    _evolvingGraph = new EvolvingGraph();
    _evolvingGraph->setLoader(this);
    _trajectories = QMap<QString, Trajectory *>();
}

GTFSLoader::GTFSLoader(QString folderPath, QString projIn, QString projOut):
    GTFSLoader(folderPath)
{
    if(!projIn.isEmpty() && !projOut.isEmpty()) {
        _pjIn  = pj_init_plus(projIn.toStdString().c_str());
        _pjOut = pj_init_plus(projOut.toStdString().c_str());
    }
}

GTFSLoader::GTFSLoader(const GTFSLoader &other) :
    _folderPath(other._folderPath),
    _stopTimesFilePath(other._stopTimesFilePath),
    _stopsFilePath(other._stopsFilePath),
    _shapesFilePath(other._shapesFilePath),
    _tripsFilePath(other._tripsFilePath)
{
    _evolvingGraph = new EvolvingGraph();
    _trajectories = QMap<QString, Trajectory *>();
    _pjIn = other._pjIn;
    _pjOut = other._pjOut;
}


GTFSLoader::~GTFSLoader()
{
}

AbstractEvolvingGraph * GTFSLoader::evolvingGraph() const
{
    return _evolvingGraph;
}

const AbstractEvolvingGraph * GTFSLoader::constEvolvingGraph() const
{
    return _evolvingGraph;
}

void GTFSLoader::parseTrips()
{
    CSVParser parser = CSVParser();
    QVector<QMap<QString, QString>> stopList  = QVector<QMap<QString, QString>>();
    QVector<QMap<QString, QString>> timesList = QVector<QMap<QString, QString>>();
    QVector<QMap<QString, QString>> tripsList = QVector<QMap<QString, QString>>();

    parser.parseCSV(_stopsFilePath, stopList, ",");
    parser.parseCSV(_stopTimesFilePath, timesList, ",");
    parser.parseCSV(_tripsFilePath, tripsList, ",");

    qDebug() << stopList.count() << " / " << timesList.count() << " / " << tripsList.count();

    // get all the trips (trajectories)
    QMap<QString,Trip*> tripsMap = QMap<QString,Trip*>();
    foreach (auto trip, tripsList) {
        QString routeId = trip.value("route_id");
        QString serviceId = trip.value("service_id");
        QString tripId = trip.value("trip_id");
        QString tripHeadsign = trip.value("trip_headsign");
        QString directionId = trip.value("direction_id");
        QString shapeId = trip.value("shape_id");
        tripsMap.insert(tripId,
                        new Trip(routeId, serviceId, tripId, tripHeadsign, directionId, shapeId));
    }

    qDebug() << "trajectories count: " << tripsMap.count();

    // get all the stops
    QMap<QString,Stop*> stopMap = QMap<QString, Stop*>();
    foreach (auto stop, stopList) {
        QString stopId = stop.value("stop_id");
        QString stopName = stop.value("stop_name");
        double lat = stop.value("stop_lat").toDouble();
        double lon = stop.value("stop_lon").toDouble();
        double x=0, y=0;

        // Transformation of the lat/lon coordinates to projected coordinates
        if(_pjIn && _pjOut) {
            x = lon * DEG_TO_RAD;
            y = lat * DEG_TO_RAD;
            pj_transform(_pjIn, _pjOut, 1, 1, &x, &y, NULL);
        } else {
            x = lon;
            y = lat;
        }

        QPointF coord(x, y);
        stopMap.insert(stopId,
                       new Stop(stopId, stopName, coord));
    }

    qDebug() << "stopMap " << stopMap.count();

    // TODO snap the trajectory to the corresponding shape

    // initialize all the trajectories
    mvtime arrivalTime;
    mvtime departureTime;
    QString tripId = "";
    QString stopId = "";

    foreach(auto time, timesList) {
        tripId = time.value("trip_id");
        stopId = time.value("stop_id");
        Stop * stop = stopMap.value(stopId);
        arrivalTime = toSeconds(time.value("arrival_time"));
        departureTime = toSeconds(time.value("departure_time"));
        if(!_trajectories.contains(tripId)){
            // Add the trajectory to the Map
            _trajectories.insert(tripId,
                                 new Trajectory(*tripsMap.value(tripId)));
        }
        _trajectories.value(tripId)->addWayPoint(arrivalTime,stop);
        if(departureTime > arrivalTime) {
            _trajectories.value(tripId)->addWayPoint(departureTime,stop);
        }
    }

    qDebug() << "trajectories " << _trajectories.count();
}

mvtime GTFSLoader::toSeconds(QString time)
{
    long hours = time.mid(0, 2).toLong();
    long minutes = time.mid(3, 2).toLong();
    long seconds = time.mid(6, 2).toLong();
    return hours * 3600 + minutes * 60 + seconds;
}

void GTFSLoader::load()
{
    qDebug() << "Begin parsing trips";
    parseTrips();
    qDebug() << "Trips parsed " << _trajectories.count();
    qDebug() << "Adding trajectories";

    int id = 0;
    for(auto trajId: _trajectories.keys())
    {
        Trajectory * traj = _trajectories.value(trajId);
        for(auto mvt: traj->getTrajectory().keys())
        {
            WayPoint * stop = traj->getTrajectory().value(mvt);
            QHash<QString, QVariant> props;
            props.insert(X, stop->getCoords().x());
            props.insert(Y, stop->getCoords().y());
            _evolvingGraph->addNode(id, mvt, props);
        }
        id++;
        delete traj;
//        if(id > 1000) break;
    }
    qDebug() << "Trajectories added " << id;
}

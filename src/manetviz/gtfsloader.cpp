#include "gtfsloader.h"
#include "csvparser.h"
#include "evolvinggraph.h"
#include "types.h"

// TODO: Do a script to get the extension (either .csv or .txt)

GTFSLoader::GTFSLoader(QString folderPath):
    _folderPath(folderPath),
    _stopTimesFilePath(folderPath+"/stop_times.txt"),
    _stopsFilePath(folderPath+"/stops.txt"),
    _shapesFilePath(folderPath+"/shapes.txt"),
    _tripsFilePath(folderPath+"/trips.txt")
{
    _evolvingGraph = new EvolvingGraph();
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

void GTFSLoader::parseTrips()
{
    CSVParser parser = CSVParser();
    QVector<QMap<QString, QString>> stopList  = QVector<QMap<QString, QString>>();
    QVector<QMap<QString, QString>> timesList = QVector<QMap<QString, QString>>();
    QVector<QMap<QString, QString>> tripsList = QVector<QMap<QString, QString>>();
    QVector<QMap<QString, QString>> shapesList = QVector<QMap<QString, QString>>();

    parser.parseCSV(_stopsFilePath, stopList, ",");
    parser.parseCSV(_stopTimesFilePath, timesList, ",");
    parser.parseCSV(_tripsFilePath, tripsList, ",");
    parser.parseCSV(_shapesFilePath, shapesList, ",");

    // TODO: case if the folder does not contain a shapes.txt file

    qDebug() << stopList.count() << " / " << timesList.count() << " / " << tripsList.count() << " / " << shapesList.count();

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
        QPointF coord = transfromCoordinates(lat, lon);

        stopMap.insert(stopId,
                       new Stop(stopId, stopName, coord));
    }

    qDebug() << "stopMap " << stopMap.count();

    // get all the shapes
    GeometryFactory *global_factory;
    QMap<QString, QMap<int,QPointF>* > shapesMap;
    foreach(auto waypoint, shapesList) {
        QString shapeId = waypoint.value("shape_id");
        double lat = waypoint.value("shape_pt_lat").toDouble();
        double lon = waypoint.value("shape_pt_lon").toDouble();
        int seq = waypoint.value("shape_pt_sequence").toInt();
        QPointF coord = transfromCoordinates(lat, lon);

        if(!shapesMap.contains(shapeId)) {
            // instanciate a new point sequence
            shapesMap.insert(shapeId, new QMap<int,QPointF>());
        }
        shapesMap.value(shapeId)->insert(seq, coord);
        qDebug() << shapeId << seq << coord;
    }

    qDebug() << "shapesMap" << shapesMap.count();

    // create the map of shape linestring
    QMap<QString, LineString*> shapes;
    for(auto it = shapesMap.begin(); it != shapesMap.end(); it++) {
        CoordinateSequence* cl = new CoordinateArraySequence();
        for(auto sIt = it.value()->begin(); sIt != it.value()->end(); ++sIt) {
            cl->add(Coordinate(sIt.value().x(), sIt.value().y()));
        }
        LineString *ls = global_factory->createLineString(cl);
        shapes.insert(it.key(), ls);
    }

    qDebug() << "shapes" << shapes.count();

    // initialize all the trajectories
    QMap<QString, QMap<int, WayPoint*>* > trajectories = QMap<QString, QMap<int, WayPoint*>* >();
    foreach(auto time, timesList) {
        QString tripId = time.value("trip_id");
        QString stopId = time.value("stop_id");
        mvtime arrivalTime = toSeconds(time.value("arrival_time"));
        mvtime departureTime = toSeconds(time.value("departure_time"));
        int seq = time.value("stop_sequence").toInt();
        Stop * stop = stopMap.value(stopId);
        WayPoint * wp = new WayPoint(stop, arrivalTime, departureTime);

        if(!trajectories.contains(tripId)){
            // Add the trajectory to the trajectories map
            trajectories.insert(tripId,
                                new QMap<int, WayPoint*>());
        }

        trajectories.value(tripId)->insert(seq, wp);
    }

    qDebug() << "trajectories" << trajectories.count();

    for(auto it = trajectories.begin(); it != trajectories.end(); ++it) {
        QString tripId = it.key();
        Trip * trip = tripsMap.value(tripId);
        QString shapeId = trip->getShapeId();

        // convert the corresponding shape into a LineString
        if(!shapes.contains(shapeId))
            continue;

        if(tripId == "6524963") {
        // Add the trajectory to the map
        _trajectories.insert(tripId,
                             new Trajectory(*trip));
        }

        LineString* ls = shapes.value(shapeId);
        LocationIndexedLine * lineRef = new LocationIndexedLine(ls);

        // get the first waypoint
        auto tIt = it.value()->begin();
        WayPoint * wp1 = tIt.value();
        Coordinate pt1(wp1->getCoords().x(), wp1->getCoords().y());
        LinearLocation loc1 = lineRef->project(pt1);
        mvtime startTime = wp1->getDepartureTime();
        Coordinate projWp1 = loc1.getCoordinate(ls);

        if(tripId == "6524963") {
            qDebug() << QString::fromStdString(ls->toString());
        }

        tIt++;
        // snap each waypoint of the trajectory to the shape linestring
        for(; tIt != it.value()->end()-1; ++tIt) {
            WayPoint * wp2 = (tIt+1).value();
            Coordinate pt2(wp2->getCoords().x(), wp2->getCoords().y());
            LinearLocation loc2 = lineRef->project(pt2);
            mvtime endTime   = wp2->getArrivalTime();
            Coordinate projWp2 = loc2.getCoordinate(ls);

            if(tripId == "6524963") {
                qDebug() << QString::fromStdString(pt1.toString()) << QString::fromStdString(projWp1.toString());
                qDebug() << QString::fromStdString(pt2.toString()) << QString::fromStdString(projWp2.toString());
                qDebug() << startTime << endTime;
            }

            // change the coordinates of the stop to match those of the projected point on the linestring
            wp1->setCoords(QPointF(projWp1.x, projWp1.y));
            wp2->setCoords(QPointF(projWp2.x, projWp2.y));

            if(tripId == "6524963") {
            // add the way point to the corresponding trajectory
            _trajectories.value(tripId)->addWayPoint(wp1);
            _trajectories.value(tripId)->addWayPoint(wp2);
            }

            // Get the partial time
            LineString * partialLine = dynamic_cast<LineString*>(lineRef->extractLine(loc1, loc2));
            double totalLength = partialLine->getLength();
            double sumLength = 0;

            if(tripId == "6524963") {
                qDebug() << "\t" << QString::fromStdString(partialLine->toString());
            }

            // Insert the points in the linestring between loc1 and loc2
            // Interpolate the time of each waypoint
            // ignore the two extermity waypoints
            Coordinate prevPt = partialLine->getCoordinateN(0);
            Coordinate curPt;
            for(int ptIdx = 1; ptIdx < partialLine->getNumPoints()-1; ++ptIdx) {
                curPt = partialLine->getCoordinateN(ptIdx);
                double distance = prevPt.distance(curPt);
                mvtime time = startTime + (endTime - startTime) * (sumLength/totalLength);
                if(tripId == "6524963") {
                    qDebug() <<  "\t\t" << QString::fromStdString(curPt.toString()) << QString::fromStdString(prevPt.toString()) << time;
                    WayPoint * wp = new WayPoint(QPointF(curPt.x, curPt.y), time, time);
                    _trajectories.value(tripId)->addWayPoint(wp);
                }
//                WayPoint * wp = new WayPoint(QPointF(curPt.x, curPt.y), time, time);
//                _trajectories.value(tripId)->addWayPoint(wp);

                // increment the distance and previous point
                sumLength += distance;
                prevPt = curPt;
            }

            // update the waypoints
            wp1 = wp2;
            pt1 = pt2;
            loc1 = loc2;
            startTime = endTime;
            projWp1 = projWp2;
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

QPointF GTFSLoader::transfromCoordinates(double lat, double lon)
{
    double x=0,y=0;
    // Transformation of the lat/lon coordinates to projected coordinates
    if(_pjIn && _pjOut) {
        x = lon * DEG_TO_RAD;
        y = lat * DEG_TO_RAD;
        pj_transform(_pjIn, _pjOut, 1, 1, &x, &y, NULL);
    } else {
        x = lon;
        y = lat;
    }

    return QPointF(x,y);
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

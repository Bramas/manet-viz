#include "shapeloader.h"
#include "project.h"
#include "iloader.h"

ShapeLoader::ShapeLoader()
{
    _shapeFilename = "/Users/ben/Data/manet-viz/src/data-test/sf-muni/shapes.txt";
    _tripFilename  = "/Users/ben/Data/manet-viz/src/data-test/sf-muni/trips.txt";

    _pen = QPen(QColor(255,0,0));
    _pen.setWidth(1);
    _pen.setCosmetic(true);

    QVector<QMap<QString, QString> > shapesList = QVector<QMap<QString, QString> >();
    QVector<QMap<QString, QString> > tripsList = QVector<QMap<QString, QString> >();

    CSVParser::parseCSV(_shapeFilename, shapesList, ",");
    CSVParser::parseCSV(_tripFilename, tripsList, ",");

    _pjIn  = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
    _pjOut = pj_init_plus("+proj=utm +zone=10 +ellps=GRS80 +datum=NAD83 +units=m +no_defs");

    foreach(auto waypoint, shapesList) {
        QString shapeId = waypoint.value("shape_id");
        double lat = waypoint.value("shape_pt_lat").toDouble();
        double lon = waypoint.value("shape_pt_lon").toDouble();
        int seq = waypoint.value("shape_pt_sequence").toInt();
        QPointF coord = transfromCoordinates(lat, lon);

        if(!_shapesMap.contains(shapeId)) {
            // instanciate a new point sequence
            _shapesMap.insert(shapeId, new QMap<int,QPointF>());
        }
        _shapesMap.value(shapeId)->insert(seq, coord);
    }

    foreach(auto trip, tripsList) {
        QString routeId = trip.value("route_id");
        QString serviceId = trip.value("service_id");
        QString tripId = trip.value("trip_id");
        QString tripHeadsign = trip.value("trip_headsign");
        QString directionId = trip.value("direction_id");
        QString shapeId = trip.value("shape_id");

        if(!_shapesToRoutes.contains(shapeId)) {
            _shapesToRoutes.insert(shapeId, new QList<QString>());
        }
        _shapesToRoutes.value(shapeId)->append(routeId);

        if(!_shapesToTrips.contains(shapeId)) {
            _shapesToTrips.insert(shapeId, new QList<QString>());
        }
        _shapesToTrips.value(shapeId)->append(tripId);
    }

    qDebug() << "shapesMap" << _shapesMap.count() << "shapes to routes" << _shapesToRoutes.count() << "shapes to trips" << _shapesToTrips.count();

}

void ShapeLoader::setProject(Project *project)
{
    _project = project;

    int maxTripCount = 0;
    for(auto it=_shapesToTrips.begin();it!=_shapesToTrips.end();++it) {
        int count = it.value()->size();
        if(count > maxTripCount) maxTripCount = count;
    }

    for(auto it = _shapesMap.begin(); it != _shapesMap.end(); ++it) {
        QString shapeId = it.key();
        if(! _shapesToTrips.contains(shapeId)) continue;
        // compute the width
        int count = _shapesToTrips.value(shapeId)->size();
        double width = (int) (1.0 + ((double)count / (double)maxTripCount)*5.0);

        // Create the multiline
        QPainterPath path;
        auto pIt = it.value()->begin();
        path.moveTo(pIt.value());
        pIt++;
        for(; pIt != it.value()->end(); ++pIt) {
            path.lineTo(pIt.value());
        }
        QGraphicsPathItem * item = new QGraphicsPathItem(path);
        _pen.setWidth(width);
        item->setPen(_pen);
         _groupItems.append(item);
    }

    GeometryFactory *global_factory = new GeometryFactory();

    // create the map of shape linestring
    QMap<QString, LineString*> shapes;
    for(auto it = _shapesMap.begin(); it != _shapesMap.end(); it++) {
        if(! _shapesToTrips.contains(it.key())) continue;
        CoordinateSequence* cl = new CoordinateArraySequence();
        for(auto sIt = it.value()->begin(); sIt != it.value()->end(); ++sIt) {
            cl->add(Coordinate(sIt.value().x(), sIt.value().y()));
        }
        LineString *ls = global_factory->createLineString(cl);
        shapes.insert(it.key(), ls);
    }

    qDebug() << "shapes" << shapes.count();

    for(auto it = shapes.begin(); it != shapes.end(); ++it) {
        LineString * ls1 = it.value();
        int count1 = _shapesToTrips.value(it.key())->size();
        for(auto jt = it+1; jt != shapes.end(); jt++) {
            LineString * ls2 = jt.value();
            int count2 = _shapesToTrips.value(jt.key())->size();
            Geometry * lsInt = ls1->intersection(ls2);
            if(!lsInt->isEmpty()) {
                GeometryTypeId geomId = lsInt->getGeometryTypeId();
//                QString geomType = "none";
                if(geomId == GEOS_POINT || geomId == GEOS_MULTIPOINT) {
                    CoordinateSequence * points = lsInt->getCoordinates();
                    int size = (int)(35.0+(((double)count1 * (double)count2) / ((double)maxTripCount * (double)maxTripCount)) * 150.0);
                    for(int i = 0; i < points->getSize(); i++) {
                        Coordinate coord = points->getAt(i);
                        QGraphicsEllipseItem * item = new QGraphicsEllipseItem(coord.x-size/2.0, coord.y-size/2.0, size, size);
                        item->setBrush(QBrush(QColor(0,0,255)));
                        item->setPen(Qt::NoPen);
                        _groupItems.append(item);
                    }
                }

//                if(geomId == GEOS_POINT) {
//                    geomType = "point";
//                    Point  * pt = dynamic_cast<Point*>(lsInt);
//                    QGraphicsEllipseItem * item = new QGraphicsEllipseItem(pt->getX(), pt->getY(),50,50);
//                    item->setBrush(QBrush(QColor(0,0,255)));
//                    item->setPen(Qt::NoPen);
//                    _groupItems.append(item);
//                }
//                if(geomId == GEOS_LINESTRING) geomType = "linestring";
//                if(geomId == GEOS_MULTIPOINT) {
//                    geomType = "multi point";

//                }
//                if(geomId == GEOS_MULTILINESTRING) geomType = "multi linestring";
//                if(geomId == GEOS_GEOMETRYCOLLECTION) geomType = "geometry collection";

//                qDebug() << it.key() << jt.key() << lsInt->getNumGeometries() << geomId << geomType;

//                qDebug() << QString::fromStdString(lsInt->getGeometryType());
            }
        }
    }

    _project->viewer()->createItemGroup(_groupItems);

}

QPointF ShapeLoader::transfromCoordinates(double lat, double lon)
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

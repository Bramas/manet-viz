#include "shapeloader.h"
#include "ui_control.h"
#include "project.h"
#include "iloader.h"
#include "csvparser.h"

#include <QMenu>

ShapeLoader::ShapeLoader():
    ui(new Ui::Control)
{
    _showShape = true;

    _pen = QPen(QColor(255,0,0));
    _pen.setWidth(1);
    _pen.setCosmetic(true);
}

void ShapeLoader::setProject(Project *project)
{
    _project = project;

    if(_project->loader()->getType() != "GTFSLoader")
        return;

    QString pathName = _project->loader()->getPath();

    _shapeFilename = pathName + "/shapes.txt";
    _tripFilename  = pathName + "/trips.txt";

    QVector<QMap<QString, QString> > shapesList = QVector<QMap<QString, QString> >();
    QVector<QMap<QString, QString> > tripsList = QVector<QMap<QString, QString> >();

    _shapestoDraw = QList<QPair<Geometry*,GeometryAttribute*> >();
    GeometryFactory *globalFactory = new GeometryFactory();


    CSVParser::parseCSV(_shapeFilename, shapesList, ",");
    CSVParser::parseCSV(_tripFilename, tripsList, ",");

    // Get all the shapes from the GTFS folder
    foreach(auto waypoint, shapesList) {
        QString shapeId = waypoint.value("shape_id");
        double lat = waypoint.value("shape_pt_lat").toDouble();
        double lon = waypoint.value("shape_pt_lon").toDouble();
        int seq = waypoint.value("shape_pt_sequence").toInt();
        QPointF coord = _project->loader()->transformCoordinates(lat, lon);

        if(!_shapesMap.contains(shapeId)) {
            // instanciate a new point sequence
            _shapesMap.insert(shapeId, new QMap<int,QPointF>());
        }
        _shapesMap.value(shapeId)->insert(seq, coord);
    }

    // get all the trips from the GTFS folder
    foreach(auto trip, tripsList) {
        QString routeId = trip.value("route_id");
        QString tripId = trip.value("trip_id");
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


    // calculate the maximum number of trips to normalize the width of the shapes
    int maxTripCount = 0;
    for(auto it=_shapesToTrips.begin();it!=_shapesToTrips.end();++it) {
        int count = it.value()->size();
        if(count > maxTripCount) maxTripCount = count;
    }

    // create the map of shape linestring
    QMap<QString, LineString*> shapes;

    for(auto it = _shapesMap.begin(); it != _shapesMap.end(); ++it) {
        QString shapeId = it.key();
        if(! _shapesToTrips.contains(shapeId)) continue;
        CoordinateSequence* cl = new CoordinateArraySequence();
        for(auto sIt = it.value()->begin(); sIt != it.value()->end(); ++sIt) {
            cl->add(Coordinate(sIt.value().x(), sIt.value().y()));
        }
        LineString *ls = globalFactory->createLineString(cl);
        shapes.insert(it.key(), ls);

        // compute the width
        int count = _shapesToTrips.value(shapeId)->size();
        double width = (int) (1.0 + ((double)count / (double)maxTripCount)*5.0);

        // Add the linestring to the list of geometries to draw
        _shapestoDraw.append(qMakePair(ls, new GeometryAttribute(width,QColor(255,0,0))));
    }

    qDebug() << "shapes" << shapes.count();
    // Get all of the intersections between the linestrings
    for(auto it = shapes.begin(); it != shapes.end(); ++it) {
        LineString * ls1 = it.value();
        int count1 = _shapesToTrips.value(it.key())->size();
        for(auto jt = it+1; jt != shapes.end(); jt++) {
            LineString * ls2 = jt.value();
            int count2 = _shapesToTrips.value(jt.key())->size();
            Geometry * lsInt = ls1->intersection(ls2);
            if(!lsInt->isEmpty()) {
                GeometryTypeId geomId = lsInt->getGeometryTypeId();
                if(geomId == GEOS_POINT || geomId == GEOS_MULTIPOINT) {
                    CoordinateSequence * points = lsInt->getCoordinates();
                    int size = (int)(35.0+(((double)count1 * (double)count2) / ((double)maxTripCount * (double)maxTripCount)) * 150.0);
                    for(int i = 0; i < points->getSize(); i++) {
                        Point * pt = globalFactory->createPoint(points->getAt(i));
                        _shapestoDraw.append(qMakePair(pt, new GeometryAttribute(size,QColor(0,0,255))));
                    }
                }
            }
        }
    }

    if(_showShape)
        drawGeometries();
}

void ShapeLoader::drawGeometries() {
    if(_project->loader()->getType() != "GTFSLoader")
        return;

    _groupItems.clear();

    foreach (auto shape, _shapestoDraw) {
        Geometry * geom = shape.first;
        GeometryAttribute * geomAttr = shape.second;

        int size = geomAttr->getSize();
        QColor color = geomAttr->getColor();

        GeometryTypeId geomId = geom->getGeometryTypeId();
        if(geomId == GEOS_LINESTRING) {
            CoordinateSequence * points = geom->getCoordinates();
            QPainterPath path;

            path.moveTo(QPointF(points->front().x, points->front().y));
            for(int i = 1; i < points->getSize(); ++i) {
                Coordinate coord = points->getAt(i);
                path.lineTo(QPointF(coord.x, coord.y));
            }
            QGraphicsPathItem * item = new QGraphicsPathItem(path);
            _pen.setWidth(size);
            _pen.setColor(color);
            item->setPen(_pen);
             _groupItems.append(item);
        }
        else if(geomId == GEOS_POINT) {
            CoordinateSequence * points = geom->getCoordinates();
            for(int i = 0; i < points->getSize(); ++i) {
                Coordinate coord = points->getAt(i);
                QGraphicsEllipseItem * item = new QGraphicsEllipseItem(coord.x-size/2.0, coord.y-size/2.0, size, size);
                item->setBrush(QBrush(color));
                item->setPen(Qt::NoPen);
                _groupItems.append(item);
            }
        }
    }
    _project->viewer()->createItemGroup(_groupItems);
}

QWidget *ShapeLoader::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);
    connect(ui->showShapeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShowShape(bool)));
    return control;
}

QMenu * ShapeLoader::createMenu() const
{
    QMenu * menu = new QMenu();
    menu->setTitle("Shape");
    QAction * menuActionOpen = menu->addAction("Open");
    connect(menuActionOpen, SIGNAL(triggered()), this, SLOT(open()));
    return menu;
}

void ShapeLoader::setShowShape(bool show)
{
    _showShape = show;
    if(show)
        drawGeometries();
    else
        _groupItems.clear();
    emit requestUpdate();
}

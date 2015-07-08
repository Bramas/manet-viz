#include "shapeloader.h"
#include "ui_control.h"
#include "project.h"
#include "iloader.h"
#include "csvparser.h"
#include <QFileDialog>

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

    if(_project->loader()->getType() == "GTFSLoader")
        loadGTFSShapes();

    if(_showShape)
        drawGeometries();
}

void ShapeLoader::drawGeometries() {
    _groupItems.clear();

    foreach (auto shape, _shapestoDraw) {
        OGRGeometry * geom = shape.first;
        GeometryAttribute * geomAttr = shape.second;

        int size = geomAttr->getSize();
        QColor color = geomAttr->getColor();

        if(wkbFlatten(geom->getGeometryType()) == wkbLineString) {
            OGRLineString * ls = (OGRLineString *) geom;
            QPainterPath path;
            OGRPoint pt;
            ls->getPoint(0,&pt);
            path.moveTo(QPointF(pt.getX(), pt.getY()));
            for(int i = 1; i < ls->getNumPoints(); ++i) {
                ls->getPoint(i,&pt);
                path.lineTo(QPointF(pt.getX(), pt.getY()));
            }
            QGraphicsPathItem * item = new QGraphicsPathItem(path);
            _pen.setWidth(size);
            _pen.setColor(color);
            item->setPen(_pen);
             _groupItems.append(item);
        }
        else if(wkbFlatten(geom->getGeometryType()) == wkbPoint) {
            OGRPoint * pt = (OGRPoint *) geom;
            QGraphicsEllipseItem * item = new QGraphicsEllipseItem(pt->getX()-size/2.0, pt->getY()-size/2.0, size, size);
            item->setBrush(QBrush(color));
            item->setPen(Qt::NoPen);
            _groupItems.append(item);
        }
    }
    _project->viewer()->createItemGroup(_groupItems);
}

void ShapeLoader::loadGTFSShapes()
{
    QString pathName = _project->loader()->getPath();

    _shapeFilename = pathName + "/shapes.txt";
    _tripFilename  = pathName + "/trips.txt";

    QFileInfo checkFile(_shapeFilename);
    if(!checkFile.exists() || !checkFile.isFile())
        return;

    QVector<QMap<QString, QString> > shapesList = QVector<QMap<QString, QString> >();
    QVector<QMap<QString, QString> > tripsList = QVector<QMap<QString, QString> >();

    // The list of OGRGeometry objects to draw on the QGraphicsScene
    _shapestoDraw = QList<QPair<OGRGeometry*,GeometryAttribute*> >();

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
    QMap<QString, OGRLineString*> shapes;

    for(auto it = _shapesMap.begin(); it != _shapesMap.end(); ++it) {
        QString shapeId = it.key();
        if(! _shapesToTrips.contains(shapeId)) continue;
        OGRLineString * ls = new OGRLineString();

        for(auto sIt = it.value()->begin(); sIt != it.value()->end(); ++sIt) {
            ls->addPoint(sIt.value().x(), sIt.value().y());
        }
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
        OGRLineString * ls1 = it.value();
        int count1 = _shapesToTrips.value(it.key())->size();
        for(auto jt = it+1; jt != shapes.end(); jt++) {
            OGRLineString * ls2 = jt.value();
            int count2 = _shapesToTrips.value(jt.key())->size();
            OGRGeometry * geom = ls1->Intersection(ls2);
            int size = (int)(35.0+(((double)count1 * (double)count2) / ((double)maxTripCount * (double)maxTripCount)) * 150.0);
            if(!geom->IsEmpty()) {
                if(wkbFlatten(geom->getGeometryType()) == wkbPoint) {
                    OGRPoint * pt = (OGRPoint *) geom;
                    _shapestoDraw.append(qMakePair(pt, new GeometryAttribute(size,QColor(0,0,255))));
                } else if(wkbFlatten(geom->getGeometryType()) == wkbMultiPoint) {
                    OGRMultiPoint * mPt = (OGRMultiPoint *) geom;
                    for(int i = 0; i < mPt->getNumGeometries(); ++i) {
                        OGRGeometry * mGeom = mPt->getGeometryRef(i);
                        if(wkbFlatten(mGeom->getGeometryType()) == wkbPoint) {
                            OGRPoint * pt  = (OGRPoint *) mGeom->clone();
                            _shapestoDraw.append(qMakePair(pt, new GeometryAttribute(size,QColor(0,0,255))));
                        }
                    }
                }
            }
        }
    }

    qDebug() << "loaded shapefile with" << _shapestoDraw.count() << "features";
}

void ShapeLoader::loadShapeFile()
{

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
    menu->setTitle("ShapeFile");
    QAction * menuActionOpen = menu->addAction("Open Shapefile");
    connect(menuActionOpen, SIGNAL(triggered()), this, SLOT(openShapeFile()));
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

void ShapeLoader::openShapeFile()
{
    QSettings settings;
    QString filename = QFileDialog::getOpenFileName(0,
                                                    "Open a shapefile",
                                                    settings.value("defaultShapeFilePath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    tr("Shapefile (*.shp)"));
    if(filename.isEmpty())
        return;
    // Save the filename path in the app settings
    settings.setValue("defaultShapeFilePath", QFileInfo(filename).absolutePath());

    OGRRegisterAll();
    OGRDataSource *poDS;

   poDS = OGRSFDriverRegistrar::Open( filename.toStdString().c_str(), FALSE );
   if( poDS == NULL )
   {
       qWarning() << "Open failed for " << filename;
       return;
   }

   _shapestoDraw.clear();

   QSet<QString> acceptedHWFieldsSet = QSet<QString>();
   acceptedHWFieldsSet << "primary_link" << "tertiary_link" << "trunk_link" << "motorway" << "road" <<  "secondary_link" << "tertiary" << "motorway_link" << "secondary" << "trunk" << "primary";
   for(int i = 0; i < poDS->GetLayerCount(); ++i) {
       OGRLayer  *poLayer = poDS->GetLayer(i);
       qDebug() << "Loading layer" << QString::fromStdString(poLayer->GetName()) << "...";
       OGRFeature *poFeature;
       OGRSpatialReference *poTarget = new OGRSpatialReference();
       poTarget->importFromProj4(_project->loader()->getOutputProj());

       poLayer->ResetReading();
       OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
       int hwIdx = poFDefn->GetFieldIndex("highway");
       while( (poFeature = poLayer->GetNextFeature()) != NULL )
       {
           QString HWFieldStr = QString::fromStdString(poFeature->GetFieldAsString(hwIdx));
           if(acceptedHWFieldsSet.contains(HWFieldStr)) {
               OGRGeometry *poGeometry = poFeature->GetGeometryRef();
               if( poGeometry != NULL
                   && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
               {
                   poGeometry->transformTo(poTarget);
                   OGRPoint * pt = (OGRPoint *) poGeometry;
                   _shapestoDraw.append(qMakePair(pt, new GeometryAttribute(50,QColor(0,0,255))));

               }
               else if (poGeometry != NULL
                        && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
               {
                   poGeometry->transformTo(poTarget);
                   OGRLineString * ls = (OGRLineString *) poGeometry;
                   _shapestoDraw.append(qMakePair(ls, new GeometryAttribute(1,QColor(255,0,0))));

               }
           }
       }
   }

   qDebug() << "loaded shapefile with" << _shapestoDraw.count() << "features";
   drawGeometries();
}

//LineString * ShapeLoader::convertFromOGRToGEOS(OGRLineString * ls) {
//    CoordinateSequence * coordinates = new CoordinateArraySequence();
//    for(int i = 0; i < ls->getNumPoints(); ++i) {
//        OGRPoint pt;
//        ls->getPoint(i, &pt);
//        Coordinate coord(pt.getX(), pt.getY());
//        coordinates->add(coord);
//    }
//    GeometryFactory globalFactory;
//    return globalFactory.createLineString(coordinates);
//}

//Point * ShapeLoader::convertFromOGRToGEOS(OGRPoint * pt) {
//    GeometryFactory globalFactory;
//    return globalFactory.createPoint(Coordinate(pt->getX(), pt->getY()));
//}

#ifndef SHAPELOADER_H
#define SHAPELOADER_H

#include "shapeloader_global.h"

#include <QObject>
#include <QString>
#include "iviewerlayer.h"
#include "csvparser.h"

#include "proj_api.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>

using namespace geos;
using namespace geos::geom;

class SHAPELOADERSHARED_EXPORT ShapeLoader: public QObject, public IViewerLayer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "shapeloader.json")
    Q_INTERFACES(IViewerLayer)
public:
    ShapeLoader();

    void setProject(Project * project);

    virtual QObject * getQObject() { return this; }

private:
    QPointF transfromCoordinates(double lat, double lon);

    QString _shapeFilename;
    QString _tripFilename;
    projPJ _pjIn;
    projPJ _pjOut;
    QPen _pen;
    QList<QGraphicsItem*> _groupItems;
    QMap<QString, QMap<int,QPointF>* > _shapesMap;
    QMap<QString, QList<QString>* > _shapesToRoutes;
    QMap<QString, QList<QString>* > _shapesToTrips;
};

#endif // SHAPELOADER_H

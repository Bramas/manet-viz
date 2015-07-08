#ifndef SHAPELOADER_H
#define SHAPELOADER_H

#include "shapeloader_global.h"

#include <QObject>
#include <QString>
#include <QColor>
#include "iviewerlayer.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <ogrsf_frmts.h>

using namespace geos;
using namespace geos::geom;

namespace Ui {
class Control;
}

class GeometryAttribute
{
public:
    GeometryAttribute(int size, QColor color) :
        _size(size), _color(color) {}
    int getSize() { return _size; }
    QColor getColor() { return _color; }

private:
    int _size;
    QColor _color;
};

class SHAPELOADERSHARED_EXPORT ShapeLoader: public QObject, public IViewerLayer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "shapeloader.json")
    Q_INTERFACES(IViewerLayer)
public:
    ShapeLoader();

    void setProject(Project * project);
    QMenu *createMenu() const;
    QWidget *createControlWidget() const;

    virtual QObject * getQObject() { return this; }

signals:
    void requestUpdate();

private:
    void drawGeometries();
    void loadGTFSShapes();
    void loadShapeFile();
    LineString * convertFromOGRToGEOS(OGRLineString * ls);
    Point * convertFromOGRToGEOS(OGRPoint * pt);

    Ui::Control *ui;
    QString _shapeFilename;
    QString _tripFilename;
    QPen _pen;
    QList<QGraphicsItem*> _groupItems;
    QMap<QString, QMap<int,QPointF>* > _shapesMap;
    QMap<QString, QList<QString>* > _shapesToRoutes;
    QMap<QString, QList<QString>* > _shapesToTrips;
    QList<QPair<Geometry*,GeometryAttribute*> > _shapestoDraw;

    bool _showShape;

private slots:
    void setShowShape(bool);
    void openShapeFile();

};

#endif // SHAPELOADER_H

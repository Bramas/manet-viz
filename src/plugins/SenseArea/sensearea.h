#ifndef SENSEAREA_H
#define SENSEAREA_H

#include "sensearea_global.h"

#include <QObject>
#include "iplugin.h"


class Area
{
public:
    Area():
        _center(QPointF()), _range(0.0), _id(idGenerator++) {}
    Area(QPointF center, int range):
        _center(center), _range(range), _id(idGenerator++) {}

    QPointF getCenter() const { return _center; }
    int getRange() const { return _range; }
    int getId() const { return _id; }
    void setCenter(double x, double y) { _center.setX(x); _center.setY(y); }
    void setCenter(QPointF center) { _center= center; }
    void setRange(int range) { _range = range; }
    bool contains(QPointF p) {
        double dx = _center.x() - p.x();
        double dy = _center.y() - p.y();
        return ((dx*dx) + (dy*dy)) < (_range*_range);
    }
    QString toString() {
        QString str;
        QTextStream(&str) << "Sense area #" << _id << " at (" << _center.x() << "," << _center.y() << ") with radius " << _range;
        return str;
    }

private:
    static int idGenerator;
    QPointF _center;
    int _range;
    int _id;
};

class GraphicsAreaItem : public QGraphicsItem
{
public:

    GraphicsAreaItem(Area * area) : _area(area), _initPos(area->getCenter()), _range(area->getRange()) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }
    GraphicsAreaItem() : _area(new Area()), _initPos(QPointF()), _range(0) {
        setFlags(QGraphicsItem::ItemIsSelectable|
                     QGraphicsItem::ItemIsMovable);
    }

    int getAreaId() { return _area->getId(); }

    QRectF boundingRect() const
    {
        QPointF offset = QPointF(_range,_range);
        return QRectF(_initPos-offset, _initPos+offset);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);

        // draw the communication range
        QPointF offset = QPointF(_range,_range);
        QRectF r = QRectF(_initPos-offset, _initPos+offset);
        QPen p;
        p.setWidth(1);
        p.setCosmetic(true);
        p.setColor(QColor(0,150,0));
        painter->setPen(p);
        painter->setBrush(QBrush(QColor(0,255,0,130)));
        painter->drawEllipse(r);
    }

    void setPen(const QPen &p)
    {
        _pen = p;
    }

    enum { Type = UserType + 2 };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

protected:
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
    {
        event->setAccepted(true);

        if(event->modifiers() == Qt::AltModifier) {
            _isResizing = false;
        } else if(event->modifiers() != Qt::ShiftModifier) {
            QGraphicsItem::mouseReleaseEvent(event);
            _area->setCenter(_area->getCenter() + (pos() - _startDrag));
        }
    }

    void mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
        event->setAccepted(true);

        if(event->button() == Qt::LeftButton) {
            if(event->modifiers() == Qt::AltModifier) {
                double radius = boundingRect().width() / 2.0;
                QPointF clikPos = event->scenePos();
                double dist = sqrt(pow(_area->getCenter().x()-clikPos.x(), 2) + pow(_area->getCenter().y()-clikPos.y(), 2));
                double frac = dist / radius;
                if(frac > 0.8 && frac <= 1.0) {
                    _isResizing = true;
                } else {
                    _isResizing = false;
                }
            } else {
                _startDrag = pos();
                QGraphicsItem::mousePressEvent(event);
            }
        } else if(event->button() == Qt::RightButton) {
            event->ignore();
        }
    }

    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
    {
        // user have moved the mouse, move the location of the box
        event->setAccepted(true);
        if(event->modifiers() == Qt::AltModifier && _isResizing){
            QPointF mousePos = event->scenePos();
            double dist = sqrt(pow(_area->getCenter().x()-mousePos.x(), 2) + pow(_area->getCenter().y()-mousePos.y(), 2));
            _range = dist;
            _area->setRange(_range);
            update(boundingRect());
        } else if(event->modifiers() != Qt::AltModifier) {
            QGraphicsItem::mouseMoveEvent(event);
        }
    }

private:
    Area * _area;
    QPointF _initPos;
    QPointF _startDrag;
    bool _isResizing;
    int _range;
    QPen _pen;
};


class SENSEAREASHARED_EXPORT SenseArea: public QObject, public IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "sensearea.json")
  Q_INTERFACES(IPlugin)

public:
    SenseArea();

    void setProject(Project * project);
    QObject * getQObject() { return this; }
    void addDependancy(QObject *plugin);
    QMap<int,Area*> getAreas() const { return _areas; }
    QMenu *createMenu() const;

private:
    QMap<int,Area*> _areas;
    QMap<int, GraphicsAreaItem *> _graphicsAreas;
    int _minContactDuration;
    int _defaultRange;
    QObject * _gridStatPlugin;

private slots:
    void onMousePressed(QGraphicsSceneMouseEvent*event);
    void setMinContactDuration(int);
    void openContactArea();
    void saveContactArea();

};

#endif // SENSEAREA_H

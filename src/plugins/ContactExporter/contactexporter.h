#ifndef CONTACTEXPORTER_H
#define CONTACTEXPORTER_H

#include <QObject>
#include "iplugin.h"

#include "../SenseArea/sensearea.h"


#include "contactexporter_global.h"


class ContactInfo
{
public:
    ContactInfo(int id1, int id2, mvtime t):
        _id1(id1), _id2(id2), _startTime(t), _endTime(t), _lastContactTime(-1) { }
    ContactInfo():
        _id1(-1), _id2(-1), _startTime(-1), _endTime(-1), _lastContactTime(-1) { }

    void setEndTime(mvtime endTime) { _endTime = endTime; }
    void setLastContactTime(mvtime time) { _lastContactTime = time; }
    mvtime getLastContactTime() { return _lastContactTime; }
    mvtime getDuration() const { return _endTime - _startTime; }
    mvtime getStartTime() const { return _startTime; }
    void addPosition(mvtime time, double x1, double y1, double x2, double y2) {
        _recordedPositions.insert(time, qMakePair(QPointF(x1,y1), QPointF(x2,y2)));
    }

    QPair<int,int> getNodes() const { return qMakePair(_id1, _id2); }
    QMap<mvtime, QPair<QPointF,QPointF> > getRecordedPositions() {
        return _recordedPositions;
    }

    QString toString() {
        QString str;
        QTextStream(&str) << _id1 << "<->" << _id2 << " [" << _startTime << " - " << _endTime << "]";
        return str;
    }

private:
    mvtime _startTime;
    mvtime _endTime;
    mvtime _lastContactTime;
    int _id1;
    int _id2;
    QMap<mvtime, QPair<QPointF,QPointF> > _recordedPositions;
};

class GridCell: public QRectF
{
public:
    GridCell():
        QRectF() {}
    GridCell(qreal x, qreal y, qreal w, qreal h):
        QRectF(x, y, w, h), _count(0) { }

    void setCount(int count) { _count = count; }
    int getCount() const { return _count; }
    void incCount() { _count++; }
    void incCount(int count) { _count += count; }
    void decCount() { _count--; }
    void decCount(int count) { _count -= count; }

private:
    int _count;
};

class ContactArea
{
public:
    ContactArea(Area * area, mvtime minContactDuration):
        _area(area), _lastContactTime(-1), _minContactDuration(minContactDuration) { }
    ContactArea():
        _area(new Area()), _lastContactTime(-1), _minContactDuration(0) { }

    bool contains(QPair<int,int> nodes) { return _contacts.contains(nodes); }
    bool contains(QPointF p) {
//        qDebug() << _area->toString() << "(" << p.x() << "," << p.y() << ") contains" << _area->contains(p);
        return _area->contains(p);
    }
    void incrementDuration(QPair<int,int> nodes, mvtime time) { _contacts.value(nodes)->setEndTime(time); }
    QVector<int> getContactDistribution() { return _contactDistribution; }
    QVector<int> getInterContactDisctribution () { return _interContactDistribution; }

    void beginContact(QPair<int,int> nodes, mvtime startTime) {
        ContactInfo * ci = new ContactInfo(nodes.first, nodes.second, startTime);
        if(_lastContactTime != -1) {
            // set the last contact time
            ci->setLastContactTime(_lastContactTime);
        }
        // record the contact
        _contacts.insert(nodes, ci);
    }

    void endContact(QPair<int,int> nodes, mvtime endTime) {
        ContactInfo * ci = _contacts.value(nodes);
        ci->setEndTime(endTime);
        mvtime contactDuration = ci->getDuration();

        if(ci->getLastContactTime() != -1 && contactDuration >= _minContactDuration) {
            // update the distribution of the intercontact durations
            mvtime interContactDuration = ci->getStartTime() - ci->getLastContactTime();
            int idx = (int) qFloor(interContactDuration / 10.0);
            if (idx >= _interContactDistribution.size()) {
                _interContactDistribution.resize(idx + 2); // resize the vector if the index is out of bounds
            }

            int curVal = _interContactDistribution.value(idx, -1);
            if(curVal == -1) {
                _interContactDistribution[idx] = 1;
            } else {
                _interContactDistribution[idx] = curVal + 1;
            }
        }

        // update the contact duration distribution
        int idx = (int) qFloor(contactDuration / 10.0);
        if (idx >= _contactDistribution.size()) {
            _contactDistribution.resize(idx + 2);
        }

        int curVal = _contactDistribution.value(idx, -1);
        if(curVal == -1) {
            _contactDistribution[idx] = 1;
        } else {
            _contactDistribution[idx] = curVal + 1;
        }

        // update the last contact time
        _lastContactTime = endTime;

        // remove the contact from the contact list
        _contacts.remove(nodes);
    }

    QString toString() {
        QString str;
        QTextStream(&str) << "Contact area " << _area->toString() << " with " << _contacts.size() << " contacts";
        return str;
    }

private:
    Area * _area;
    mvtime _lastContactTime;
    mvtime _minContactDuration;
    QVector<int> _interContactDistribution;
    QVector<int> _contactDistribution;
    QMap<QPair<int,int>, ContactInfo *> _contacts;
};


class CONTACTEXPORTERSHARED_EXPORT ContactExporter: public QObject, public IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "contactexporter.json")
  Q_INTERFACES(IPlugin)

public:
    ContactExporter();

    void addDependancy(QObject * plugin);
    QStringList requiredField() const { return QStringList(); }
    void setProject(Project * project);
    QWidget * createControlWidget() const;

    virtual QObject * getQObject() { return this; }

signals:
    void graphLoaded(bool);

public slots:
    void onGraphLoaded();
    void convert();
    void exportContactDistributions();
    void setMinContactDuration(int);

private:
    QObject * _gridStatPlugin;
    SenseArea * _senseAreaPlugin;
    mvtime _minContactDuration;
    mvtime _lastContactTime;
    int _cellSize;
    QMap<QPair<int,int>, ContactInfo *> _contacts;
    QHash<QPoint,GridCell *> _contactCount;
    QMap<int,ContactArea*> _contactAreas;
    QVector<int> _interContactDistribution;
    QVector<int> _contactDistribution;
};

#endif // CONTACTEXPORTER_H

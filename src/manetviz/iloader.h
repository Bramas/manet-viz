#ifndef ILOADER
#define ILOADER

class QObject;
class AbstractEvolvingGraph;
class Project;

#include <QPointF>
#include "/usr/local/include/proj_api.h"

class ILoader {

public:
    ILoader() {}
    virtual ~ILoader() {}

    virtual void setProject(Project * project) { _project = project; }

    virtual const AbstractEvolvingGraph *   constEvolvingGraph() const = 0;
    virtual AbstractEvolvingGraph *         evolvingGraph() const = 0;

    QPointF transformCoordinates(double lat, double lon) const {
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

    virtual QString getType() const = 0;

    virtual QString getPath() const = 0;

    virtual bool isLoaded() const = 0;

    virtual QObject * getQObject() = 0;

protected:
    Project * _project;
    projPJ _pjIn;
    projPJ _pjOut;

};

#endif // ILOADER


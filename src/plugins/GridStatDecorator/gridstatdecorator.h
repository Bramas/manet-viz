#ifndef GRIDSTATDECORATOR_H
#define GRIDSTATDECORATOR_H
#include <QObject>
#include "igraphdecorator.h"

#include "gridstatdecorator_global.h"

class GRIDSTATDECORATORSHARED_EXPORT GridStatDecorator: public QObject, public IGraphDecorator
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IGraphDecorator" FILE "gridstatdecorator.json")
    Q_INTERFACES(IGraphDecorator)
public:
    GridStatDecorator();
};

#endif // GRIDSTATDECORATOR_H

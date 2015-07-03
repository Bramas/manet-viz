#ifndef GRAPHCONVERTOR_H
#define GRAPHCONVERTOR_H

#include "graphconvertor_global.h"

#include <QObject>
#include "iviewerlayer.h"

class GRAPHCONVERTORSHARED_EXPORT GraphConvertor : public QObject, public IViewerLayer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IViewerLayer" FILE "graphconvertor.json")
    Q_INTERFACES(IViewerLayer)
public:
    GraphConvertor();

    QStringList requiredField() const { return QStringList(); }
    void setProject(Project * project);
    QWidget * createControlWidget() const;

    virtual QObject * getQObject() { return this; }
signals:
    void graphLoaded(bool);
public slots:
    void onGraphLoaded();

    void convert();
};

#endif // GRAPHCONVERTOR_H

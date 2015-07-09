#ifndef GRAPHCONVERTOR_H
#define GRAPHCONVERTOR_H

#include "graphconvertor_global.h"

#include <QObject>
#include "iplugin.h"

class GRAPHCONVERTORSHARED_EXPORT GraphConvertor : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "graphconvertor.json")
    Q_INTERFACES(IPlugin)
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

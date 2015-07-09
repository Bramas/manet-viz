#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QGraphicsScene>
#include <QMenuBar>

#include "types.h"

class Viewer;
class IGraph;
class IPlugin;
class ILoader;
class Project : public QObject
{
    Q_OBJECT
public:
    Project(Viewer *viewer, ILoader *loader, QMenuBar *menuBar);
    ~Project();

    Viewer * viewer() { return _viewer; }
    ILoader * loader() { return _loader; }

    //void setLoader(ILoader *loader) { _loader = loader; }
    //void setViewer(Viewer *viewer) { _viewer = viewer; }

    virtual void addLayer(IPlugin * layer, int priority = 100);

    const QList<IPlugin* > layers() const { return _layers.values(); }

    virtual IGraph * constructSnapshot(mvtime time) const;
public slots:
    void onPluginsChanged();
protected:
    QMap<int, IPlugin*> _layers;
    Viewer * _viewer;
    ILoader * _loader;
    QMenuBar * _menuBar;
};

#endif // PROJECT_H

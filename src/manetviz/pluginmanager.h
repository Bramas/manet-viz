#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QMetaObject>
#include <QReadWriteLock>
#include <QPluginLoader>
#include <QStringList>
#include <QString>
#include <QMap>
#include <QDebug>

class PluginManager : public QObject
{
    Q_OBJECT
public:
    static PluginManager instance;
    ~PluginManager();

    static QReadWriteLock *listLock();
    static QList<QObject *> allObjects();
    template <typename T> static QList<T *> getObjects()
    {
        QReadLocker lock(listLock());
        QList<T *> results;
        QList<QObject *> all = allObjects();
        foreach (QObject *obj, all) {
            T *result = qobject_cast<T *>(obj);
            if (result)
                results += result;
        }
        return results;
    }

    static void loadPlugins();
    static QStringList pluginPaths();
    static void setPluginPaths(const QStringList &paths);
    static QObject * getObjectByName(QString name){
        QReadLocker lock(listLock());
        QList<QObject *> all = allObjects();

        foreach (QObject *obj, all) {
            if (obj->objectName() == name)
                return obj;
        }
        return NULL;
    }

    enum PluginState { Invalid, Loaded, Initialized, Running, Stopped, Deleted};

signals:
    void pluginsChanged();

public slots:

private:
    explicit PluginManager(QObject *parent = 0);
    static QList<QPair<QString,QString> > loadFilenames();


    QList<QPair<QString, QObject *> > _allObjects;
    mutable QReadWriteLock _listLock;
    QStringList _pluginPath;
};

#endif // PLUGINMANAGER_H

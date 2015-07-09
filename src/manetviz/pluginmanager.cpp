#include "pluginmanager.h"

#include <QDir>
#include <QSet>
#include <QDebug>
#include <QJsonArray>
#include "iplugin.h"
#include "igraphlayout.h"

PluginManager PluginManager::instance;

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{

}

PluginManager::~PluginManager()
{

}

QReadWriteLock * PluginManager::listLock()
{
    return &instance._listLock;
}


QList<QObject *> PluginManager::allObjects()
{
    QList<QObject *> ret;
    foreach(auto o, instance._allObjects) {
        ret.append(o.second);
    }

    return ret;
}

void PluginManager::loadPlugins()
{
    QList<QPair<QString,QString> > pluginFilenames = loadFilenames();

    foreach (auto filename, pluginFilenames) {
        QPluginLoader loader(filename.second);
        if(!loader.load())
        {
            qWarning()<<"Unable to laod plugin: "<<filename.first;
            continue;
        }

        IPlugin * plugin = qobject_cast<IPlugin*>(loader.instance());
        if(plugin) // Load a plugin
        {
            plugin->getQObject()->setObjectName(filename.first);
            qDebug() << "Load IPlugin " << filename.first;
            QJsonObject meta = loader.metaData();
            QStringList dependancyList = meta.value("MetaData").toObject().value("dependencies").toVariant().toStringList();
            foreach(QString dep, dependancyList) {
                plugin->addDependancy(getObjectByName(dep));
            }
            instance._allObjects.append(qMakePair(filename.first, plugin->getQObject()));
        }

        IGraphLayout * layout = qobject_cast<IGraphLayout*>(loader.instance());
        if(layout) // Load a layout
        {
            layout->getQObject()->setObjectName(filename.first);
            qDebug() << "Load Layout " << filename.first;
            instance._allObjects.append(qMakePair(filename.first, layout->getQObject()));
        }

    }
}

QList<QPair<QString,QString> > PluginManager::loadFilenames()
{
    QSet<QString> loadedPlugins;
    QMap<QString, QString> pluginNames;
    QMap<QString,QStringList*> dependancyStack;
    QList<QPair<QString,QString>> filenames;

    foreach(const QString &path, pluginPaths())
    {
        QDir pluginsDir(path);
        foreach (const QString &filename, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
            QJsonObject meta = loader.metaData();
            QString name = meta.value("MetaData").toObject().value("name").toString();
            bool active = meta.value("MetaData").toObject().value("active").toVariant().toBool();

            if(!active || name.isEmpty() || pluginNames.contains(name))
            {
                if (!active) {
                    qDebug()<<"Plugin non active: " << name;
                }
                else if(name.isEmpty())
                {
                    qDebug()<<pluginsDir.absoluteFilePath(filename)<<" has empty plugin name: "<<name;
                }
                else if(pluginNames.contains(name))
                {
                    qDebug()<<"Duplicate plugin: " << name;
                }
                continue;
            }

            // Add the plugin in the dependancy stack
            dependancyStack.insert(name, new QStringList());
            pluginNames.insert(name,pluginsDir.absoluteFilePath(filename));

            // Add dependancies
            QStringList dependancyList = meta.value("MetaData").toObject().value("dependencies").toVariant().toStringList();
            foreach(QString dep, dependancyList) {
                if(!loadedPlugins.contains(dep)) {
                    dependancyStack.value(name)->push_back(dep);
                }
            }

            // finally add the plugin that satisfies all dependancies
            if(dependancyStack.value(name)->size() == 0) {
                // Add the plugin
                dependancyStack.remove(name);
                loadedPlugins.insert(name);
                filenames << qMakePair(name,pluginNames.value(name));
//                filenames << pluginNames.value(name);
            }
        }
    }

    // Check the dependancies for non-loaded plugins
    while (!dependancyStack.isEmpty())
    {
        // check if the current pluging satisfies the dependancies
        for(auto i = dependancyStack.begin(); i != dependancyStack.end();) {
            QString name = i.key();
            QStringList * depList = dependancyStack.value(name);
            QMutableListIterator<QString> j(*depList);
            while (j.hasNext()) {
                QString dep = j.next();
                if(pluginNames.contains(dep)) {
                    // delete the dependancy in the list
                    j.remove();
                }
            }
            if(depList->isEmpty()) {
                // delete the plugin from the stack (increment the iterator) and load the plugin
                i = dependancyStack.erase(i);
                loadedPlugins.insert(name);
                filenames << qMakePair(name,pluginNames.value(name));
//                filenames << pluginNames.value(name);
            } else {
                ++i;
            }
        }
    }

    return filenames;
}

QStringList PluginManager::pluginPaths()
{
    return instance._pluginPath;
}

void PluginManager::setPluginPaths(const QStringList &paths)
{
    instance._pluginPath = paths;
}

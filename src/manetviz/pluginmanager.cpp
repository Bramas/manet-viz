#include "pluginmanager.h"

#include <QDir>
#include <QSet>
#include <QDebug>

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
    return instance._allObjects;
}

void PluginManager::loadPlugins()
{
    QStringList pluginFilenames = loadFilenames();

    foreach (const QString & filename, pluginFilenames) {
        QPluginLoader loader(filename);
        if(!loader.load())
        {
            qWarning()<<"Unable to laod plugin: "<<filename;
            continue;
        }
        instance._allObjects << loader.instance();
    }
}

QStringList PluginManager::loadFilenames()
{
    QSet<QString> pluginNames;
    QStringList filenames;
    foreach(const QString &path, pluginPaths())
    {
        QDir pluginsDir(path);
        foreach (const QString &filename, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
            QJsonObject meta = loader.metaData();
            QString name = meta.value("MetaData").toObject().value("name").toString();
            if(name.isEmpty() || pluginNames.contains(name))
            {
                if(name.isEmpty())
                {
                    qDebug()<<pluginsDir.absoluteFilePath(filename)<<" has empty plugin name: "<<name;
                }
                else
                {
                    qDebug()<<"Duplicate plugin: "<<name;
                }
                continue;
            }
            pluginNames.insert(name);
            filenames << pluginsDir.absoluteFilePath(filename);
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

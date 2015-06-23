#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include "pluginmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir pluginsDir(a.applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    PluginManager::setPluginPaths(QStringList(pluginsDir.absolutePath()));
    PluginManager::loadPlugins();



    MainWindow w;
    w.show();

    return a.exec();
}

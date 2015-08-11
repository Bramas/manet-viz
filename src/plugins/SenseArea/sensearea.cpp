#include "sensearea.h"
#include "project.h"
#include <QFileDialog>

int Area::idGenerator = 1;

SenseArea::SenseArea()
{
    _minContactDuration = 0;
    _defaultRange = 100;
}

void SenseArea::setProject(Project *project)
{
    _project = project;
    connect(project->viewer(), SIGNAL(mousePressedEvent(QGraphicsSceneMouseEvent*)), this, SLOT(onMousePressed(QGraphicsSceneMouseEvent*)));
}

void SenseArea::addDependancy(QObject *plugin)
{
    if(plugin->objectName() == "GridStatDecorator")
    {
        qDebug() << "connected to plugin"<<plugin->objectName();
        _gridStatPlugin = plugin;
        connect(_gridStatPlugin, SIGNAL(minContactDurationChanged(int)), this, SLOT(setMinContactDuration(int)));
    }
}

QMenu *SenseArea::createMenu() const
{
    QMenu * menu = new QMenu();
    menu->setTitle("Contact Areas");
    connect(menu->addAction("Open Contact Area"), SIGNAL(triggered()), this, SLOT(openContactArea()));
    connect(menu->addAction("Save Contact Area"), SIGNAL(triggered()), this, SLOT(saveContactArea()));
    return menu;

}

void SenseArea::onMousePressed(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(event->modifiers() != Qt::AltModifier) {
            // create a new sensing area where the user clicked
            Area * area = new Area(event->scenePos(), _defaultRange);
            _areas.insert(area->getId(),area);
            GraphicsAreaItem * graphicsArea = new GraphicsAreaItem(area);
            _graphicsAreas.insert(area->getId(), graphicsArea);
            QPen p;
            p.setWidthF(4);
            graphicsArea->setPen(p);
            _project->viewer()->addItem(graphicsArea);
        }
    } else if (event->button() == Qt::RightButton) {
        // delete the static node from the map
        QGraphicsItem * itemToRemove = NULL;
        foreach(auto item, _project->viewer()->items(event->scenePos())) {
            if(item->type() == QGraphicsItem::UserType+2) {
                itemToRemove = item;
                break;
            }
        }

        if(itemToRemove) {
            // remove the item from the graphicsScene
            GraphicsAreaItem * graphicsArea = qgraphicsitem_cast<GraphicsAreaItem *>(itemToRemove);
            _areas.remove(graphicsArea->getAreaId());
            _graphicsAreas.remove(graphicsArea->getAreaId());
            _project->viewer()->removeItem(itemToRemove);
        }
    }
}

void SenseArea::setMinContactDuration(int value)
{
    _minContactDuration = value;
}

void SenseArea::openContactArea()
{
    QSettings settings;
    QString filename = QFileDialog::getOpenFileName(0,
                                                    "Open a contact area",
                                                    settings.value("defaultContactAreaPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    tr("Text file (*.txt)"));
    if(filename.isEmpty())
        return;

    // Save the filename path in the app settings
    settings.setValue("defaultContactAreaPath", QFileInfo(filename).absolutePath());

    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug() << "Unable to open the contact areas file "<<filename;
        return;
    }

    // Delete all areas
    qDeleteAll(_areas);
    for(auto it = _graphicsAreas.begin(); it != _graphicsAreas.end();) {
        _project->viewer()->removeItem(it.value());
        it = _graphicsAreas.erase(it);
    }

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = QString(in.readLine()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts).at(0);
        QStringList fields = line.split(";");
        Area * area = new Area(QPoint(fields.at(1).toDouble(), fields.at(2).toDouble()), fields.at(3).toInt());
        GraphicsAreaItem * graphicsArea = new GraphicsAreaItem(area);
        QPen p;
        p.setWidthF(4);
        graphicsArea->setPen(p);

        _project->viewer()->addItem(graphicsArea);
        _areas.insert(fields.at(0).toInt(), area);
        _graphicsAreas.insert(area->getId(), graphicsArea);
    }

    file.close();
}

void SenseArea::saveContactArea()
{
    QSettings settings;
    QString filename = QFileDialog::getSaveFileName(0,
                                                    tr("Save the current contact areas"),
                                                    settings.value("defaultContactAreaPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString(),
                                                    tr("Text file (*.txt)"));
    if(filename.isEmpty())
        return;

    // Save the filename path in the app settings
    settings.setValue("defaultContactAreaPath", QFileInfo(filename).absolutePath());

    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << "Unable to open the contact areas file "<<filename;
        return;
    }

    QTextStream out(&file);
    for(auto it = _areas.begin(); it != _areas.end(); ++it) {
        Area * area = it.value();
        out << area->getId() << ";" << area->getCenter().x() << ";" << area->getCenter().y() << ";" << area->getRange() << "\n";
    }

    file.close();
}

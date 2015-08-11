#include "contactexporter.h"
#include <QPushButton>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include "iloader.h"
#include "project.h"
#include "abstractevolvinggraph.h"
#include <ogrsf_frmts.h>
#include <QDateTime>

ContactExporter::ContactExporter()
{
    _minContactDuration = 0;
    _lastContactTime = -1;
}

void ContactExporter::addDependancy(QObject *plugin)
{
    if(plugin->objectName() == "GridStatDecorator")
    {
        qDebug() << "connected to plugin"<<plugin->objectName();
        _gridStatPlugin = plugin;
        connect(_gridStatPlugin, SIGNAL(minContactDurationChanged(int)), this, SLOT(setMinContactDuration(int)));
    }
    if(plugin->objectName() == "SenseArea")
    {
        qDebug() << "connected to plugin"<<plugin->objectName();
        _senseAreaPlugin = (SenseArea*) plugin;
    }
}

QWidget* ContactExporter::createControlWidget() const
{
    QPushButton * widget = new QPushButton("Export contacts");
    widget->setEnabled(false);
    connect(this, &ContactExporter::graphLoaded, widget, &QPushButton::setEnabled);
    connect(widget, &QPushButton::clicked, this, &ContactExporter::exportContactDistributions);
    if(_project->loader()->isLoaded())
    {
        widget->setEnabled(true);
    }
    return widget;
}

void ContactExporter::onGraphLoaded()
{
    emit graphLoaded(true);
}

void ContactExporter::setProject(Project * project)
{
    _project = project;
    connect(_project->loader()->getQObject(), SIGNAL(loaded()), this, SLOT(onGraphLoaded()));
}

void ContactExporter::exportContactDistributions()
{
    if(!_project->loader()->isLoaded())
    {
        qDebug() << "graph is not fully loaded ";
        return;
    }

    const AbstractEvolvingGraph * evg =  _project->loader()->constEvolvingGraph();

    QString filename = QFileDialog::getSaveFileName(0,
                                                    tr("Save the contact distributions"),
                                                    QString(),
                                                    tr("Text file (*.txt)"));
    if(filename.isEmpty())
        return;

    // Delete all complex data structures
    qDeleteAll(_contactAreas);
    _contactDistribution.clear();
    _interContactDistribution.clear();

    QFileInfo original(filename);
    QString filename_distr = original.canonicalPath() + QDir::separator() + original.baseName() + "_distr";
    if (!original.completeSuffix().isEmpty())
        filename_distr += "." + original.completeSuffix();

    QString filename_inter_distr = original.canonicalPath() + QDir::separator() + original.baseName() + "_inter_distr";
    if (!original.completeSuffix().isEmpty())
        filename_inter_distr += "." + original.completeSuffix();

    qDebug()<<"export contact distributions => " << filename_distr;
    qDebug()<<"export contact inter distributions => " << filename_inter_distr;

    QFile file_distr(filename_distr);
    if(!file_distr.open(QFile::WriteOnly))
    {
        qDebug() << "Unable to write in file "<<filename_distr;
        return;
    }

    QFile file_inter_distr(filename_inter_distr);
    if(!file_inter_distr.open(QFile::WriteOnly))
    {
        qDebug() << "Unable to write in file "<<filename_inter_distr;
        return;
    }

    // Create the contact areas
    qDebug() << "Number of sense areas" << _senseAreaPlugin->getAreas().keys();
    for(auto e : _senseAreaPlugin->getAreas().keys()) {
        Area * area = _senseAreaPlugin->getAreas().value(e);
        ContactArea * contactArea = new ContactArea(area,_minContactDuration);
        _contactAreas.insert(area->getId(), contactArea);
        qDebug() << area->toString();
        qDebug() << contactArea->toString();
    }

    qDebug() << "------\n" << _contactAreas.size() << "contact areas created";

    for(mvtime i = evg->beginTime(); i < evg->beginTime() + 10000; ++i)
    {
        QSet<QPair<int,int> > examinedContacts;
        IGraph * snapshot = _project->constructSnapshot(i);
//        qDebug() << "[" << i << "] Number of contacts " << _contacts.size() << " / last contact time " << _lastContactTime;
        foreach(const Node &n1, snapshot->nodes())
        {
            foreach(int n2, n1.neighbors())
            {
                if(n1.id() < n2)
                {
                    QPair<int,int> nodes = qMakePair(n1.id(),n2);
                    examinedContacts.insert(nodes);
                    QPointF p1(n1.properties().value(X).toDouble(), n1.properties().value(Y).toDouble());
                    QPointF p2(snapshot->nodes().value(n2).properties().value(X).toDouble(), snapshot->nodes().value(n2).properties().value(Y).toDouble());

                    ContactInfo * ci;
                    if(_contacts.contains(nodes)) {
                        ci = _contacts.value(nodes);
                        // increment the contact duration
                        ci->setEndTime(i);
                    } else {
                        ci = new ContactInfo(n1.id(), n2, i);
                        if(_lastContactTime != -1) {
                            // set the last contact time
                            ci->setLastContactTime(_lastContactTime);
                        }
                        // record the contact
                        _contacts.insert(nodes, ci);
                    }

                    // loop through all the contactAreas
                    for(auto it = _contactAreas.begin(); it != _contactAreas.end(); ++it) {
                        ContactArea * contactArea = it.value();
                        if(contactArea->contains(nodes)) {
                            // increment the contact duration
                            contactArea->incrementDuration(nodes, i);
                        } else {
                            if(contactArea->contains(p1) && contactArea->contains(p2)) {
                                // add the contact to the contact list
                                qDebug() << "\n[Hotspot"<<it.key()<<"] Add contact" << nodes.first << "<->" << nodes.second;
                                contactArea->beginContact(nodes, i);
                            }
                        }
                    }

                }
            }
        }

        if(!(i%1000))
        {
            qDebug()<<((i - evg->beginTime())/qreal(evg->endTime() - evg->beginTime()));
        }

        delete snapshot;

        // get all contacts that have not been examined
        if(_contacts.size() > 0) {
            // get all contacts to remove
            QSet<QPair<int,int> > contactsToRemove = _contacts.uniqueKeys().toSet().subtract(examinedContacts);
            if(contactsToRemove.size() > 0) {
                qDebug() << "[" << i << "] removing" << contactsToRemove.size() << "contacts";
            }

            for(auto it = contactsToRemove.begin(); it != contactsToRemove.end(); ++it) {
                QPair<int,int> nodes = *it;
                ContactInfo * ci = _contacts.value(nodes);
                ci->setEndTime(i);
                mvtime contactDuration = ci->getDuration();

                qDebug() << "\t[Removing]" << ci->toString() << "/ duration" << contactDuration << "/ min contact duration" << _minContactDuration << "/ last contact time" << ci->getLastContactTime();

                if(ci->getLastContactTime() != -1 && contactDuration >= _minContactDuration) {
                    // update the distribution of the intercontact durations
                    mvtime interContactDuration = ci->getStartTime() - ci->getLastContactTime();
                    int idx = (int) qFloor(interContactDuration / 10.0);
                    if (idx >= _interContactDistribution.size()) {
                        _interContactDistribution.resize(idx + 2); // resize the vector if the index is out of bounds
                    }

                    int curVal = _interContactDistribution.value(idx, -1);
                    qDebug() << "\t[Insert] in the inter contact duration" << interContactDuration << "/ index" << idx << "/ current value" << curVal;

                    if(curVal == -1) {
                        _interContactDistribution[idx] = 1;
                    } else {
                        _interContactDistribution[idx] = curVal + 1;
                    }
                }

                // update the contact duration distribution
                int idx = (int) qFloor(contactDuration / 10.0);
                if (idx >= _contactDistribution.size()) {
                    _contactDistribution.resize(idx + 2);
                }

                int curVal = _contactDistribution.value(idx, -1);
                if(curVal == -1) {
                    _contactDistribution[idx] = 1;
                } else {
                    _contactDistribution[idx] = curVal + 1;
                }

                // update the last contact time
                _lastContactTime = i;

                // remove the contact from the contact list
                _contacts.remove(nodes);

                // loop through all the contact areas
                for(auto jt = _contactAreas.begin(); jt != _contactAreas.end(); ++jt) {
                    ContactArea * contactArea = jt.value();
                    if(contactArea->contains(nodes)) {
                        qDebug() << "[Hotspot"<<jt.key()<<"] End contact" << nodes.first << "<->" << nodes.second ;
                        contactArea->endContact(nodes,i);
                    }
                }
            }
        }
    }

    qDebug() << "DONE";

    qDebug() << "[Writing] output distributions in files";
    qDebug() << "[INFO]" << _contactDistribution.size() << _interContactDistribution.size();

    QTextStream out_distr(&file_distr);
    QTextStream out_inter_distr(&file_inter_distr);
    for(int i = 0; i < _contactDistribution.size(); ++i) {
        out_distr << "all;" << i << ";" << _contactDistribution[i] << "\n";
    }

    for(int i = 0; i < _interContactDistribution.size(); ++i) {
        out_inter_distr << "all;" << i << ";" << _interContactDistribution[i] << "\n";
    }

    for(auto it = _contactAreas.begin(); it != _contactAreas.end(); ++it) {
        ContactArea * contactArea = it.value();
        qDebug() << contactArea->toString() << "/ contact distr" << contactArea->getContactDistribution().size() << "/ intercontact distr" << contactArea->getInterContactDisctribution().size();
        for(int i = 0; i < contactArea->getContactDistribution().size(); ++i) {
            out_distr << "hs" << it.key() << ";" << i << ";" << contactArea->getContactDistribution().at(i) << "\n";
        }
        for(int i = 0; i < contactArea->getInterContactDisctribution().size(); ++i) {
            out_inter_distr << "hs" << it.key() << ";" << i << ";" << contactArea->getInterContactDisctribution().at(i) << "\n";
        }
    }

    qDebug() << "[Writing] DONE";
}

void ContactExporter::convert()
{
    if(!_project->loader()->isLoaded())
    {
        qDebug() << "graph is not fully loaded ";
        return;
    }

    const AbstractEvolvingGraph * evg =  _project->loader()->constEvolvingGraph();

    QString filename = QFileDialog::getSaveFileName(0,tr("Save the GeoDatabase"),QString(), tr("ESRI File Geodatabase (*.gdb)"));
    if(filename.isEmpty())
        return;

    qDebug()<<"export contacts => "<<filename;
//    QFile file(filename);
//    if(!file.open(QFile::WriteOnly))
//    {
//        qDebug() << "Unable to write in file "<<filename;
//        return;
//    }

    const char *pszDriverName = "FileGDB";
    OGRSFDriver *poDriver;

    OGRRegisterAll();
    OGRDataSource *poDS;

    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
                    pszDriverName );
    if( poDriver == NULL )
    {
        printf( "%s driver not available.\n", pszDriverName );
        exit( 1 );
    }

    poDS = poDriver->CreateDataSource( filename.toLatin1().data(), NULL );
    if( poDS == NULL )
    {
        qWarning() << "Creation of output file failed.";
    }

    OGRLayer *poLayer;
    OGRSpatialReference *srs = new OGRSpatialReference();
    srs->importFromProj4(_project->loader()->getOutputProj());

    poLayer = poDS->CreateLayer( "contacts", srs, wkbPoint, NULL );
    if( poLayer == NULL )
    {
        qWarning() << "Layer creation failed.";
    }

    OGRFieldDefn oFieldTimeStamp( "Timestamp", OFTDateTime );

    oFieldTimeStamp.SetWidth(32);

    if( poLayer->CreateField( &oFieldTimeStamp ) != OGRERR_NONE )
    {
        qWarning() <<  "Creating TimeStamp field failed.";
    }

    OGRFieldDefn oFieldNodeId1( "NodeId1", OFTInteger );
    oFieldNodeId1.SetWidth(32);

    if( poLayer->CreateField( &oFieldNodeId1 ) != OGRERR_NONE )
    {
        qWarning() << "Creating NodeId1 field failed.";
    }

    OGRFieldDefn oFieldNodeId2( "NodeId2", OFTInteger );
    oFieldNodeId2.SetWidth(32);

    if( poLayer->CreateField( &oFieldNodeId2 ) != OGRERR_NONE )
    {
        qWarning() << "Creating NodeId2 field failed.";
    }

    OGRSpatialReference *poTarget = new OGRSpatialReference();
    poTarget->importFromProj4(_project->loader()->getOutputProj());

//    QTextStream out(&file);

    qDebug()<<evg->beginTime()<<" "<<evg->endTime();
    for(mvtime i = evg->beginTime(); i < evg->endTime(); ++i)
    {
        IGraph * snapshot = _project->constructSnapshot(i);
        QSet<QPair<int,int> > examinedContacts;
        foreach(const Node &n1, snapshot->nodes())
        {
            foreach(int n2, n1.neighbors())
            {
                if(n1.id() < n2)
                {
                    QPair<int,int> nodes = qMakePair(n1.id(),n2);
                    examinedContacts.insert(nodes);

                    ContactInfo * ci;
                    if(_contacts.contains(nodes)) {
                        ci = _contacts.value(nodes);
                        // increment the contact duration
                        ci->setEndTime(i);
                    } else {
                        ci = new ContactInfo(n1.id(), n2, i);
                        _contacts.insert(nodes, ci);
                    }
                    double x1 = n1.properties().value(X).toDouble();
                    double y1 = n1.properties().value(Y).toDouble();
                    double x2 = snapshot->nodes().value(n2).properties().value(X).toDouble();
                    double y2 = snapshot->nodes().value(n2).properties().value(Y).toDouble();
                    ci->addPosition(i, x1, y1, x2, y2);
                }
            }
        }

        // get all contacts that have not been examined
        if(_contacts.size() > 0) {
            QSet<QPair<int,int> > contactsToRemove = _contacts.uniqueKeys().toSet().subtract(examinedContacts);
            for(auto it = contactsToRemove.begin(); it != contactsToRemove.end(); ++it) {
                ContactInfo * ci = _contacts.value(*it);
                if(ci->getDuration() >= _minContactDuration) {
//                    qDebug() << "contacts between" << ci->getNodes().first <<"and"<<ci->getNodes().second << "lasted"<<ci->getDuration();
                    QMap<mvtime, QPair<QPointF,QPointF> > positions = ci->getRecordedPositions();
                    for(auto it = positions.begin(); it != positions.end(); ++it) {
                        OGRPoint p1  = OGRPoint(it.value().first.x(), it.value().first.y());
                        OGRPoint p2  = OGRPoint(it.value().second.x(), it.value().second.y());
                        OGRPoint mid = OGRPoint((p1.getX() + p2.getX()) / 2.0, (p1.getY() + p2.getY()) / 2.0);
                        OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
                        mvtime time = it.key();
                        QDateTime timestamp;
                        timestamp.setTime_t(time);


                        poFeature->SetField( "Timestamp",
                                             timestamp.date().year(),
                                             timestamp.date().month(),
                                             timestamp.date().day(),
                                             timestamp.time().hour(),
                                             timestamp.time().minute(),
                                             timestamp.time().second(),
                                             0);
                        poFeature->SetField( "NodeId1", ci->getNodes().first );
                        poFeature->SetField( "NodeId2", ci->getNodes().second );
                        poFeature->SetGeometry( &mid );

                        if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                        {
                           qWarning() << "Failed to create feature in shapefile.";
                        }
                        OGRFeature::DestroyFeature( poFeature );
//                        out << it.key() << ";" << ci->getNodes().first << ";" << ci->getNodes().second << ";" << mid.x() << ";" << mid.y() << ";" << p1.x() << ";" << p1.y() << ";" << p2.x() << ";" << p2.y() << "\n";
                    }
                }
                _contacts.remove(*it);
            }
        }

        if(!(i%1000))
        {
            qDebug()<<((i - evg->beginTime())/qreal(evg->endTime() - evg->beginTime()));
        }
        delete snapshot;
    }
    qDebug()<<"DONE";
    OGRDataSource::DestroyDataSource( poDS );
    //_project->constructSnapshot(_time);
}

void ContactExporter::setMinContactDuration(int value)
{
    _minContactDuration = value;
}


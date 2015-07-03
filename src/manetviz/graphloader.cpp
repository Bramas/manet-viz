#include "graphloader.h"
#include <QFile>
#include <QTextStream>
#include <ctime>

#include "abstractevolvinggraph.h"
#include "evolvinggraph.h"

GraphLoader::GraphLoader(QString filename, QRegExp lineRegex, QList<QString> headers, QString timeFormat, QString projIn, QString projOut) :
    _filename(filename),
    _lineRegex(lineRegex),
    _timeFormat(timeFormat),
    _headers(headers)
{
    _forceStop = false;
    _loadProgress = 0;
    _evolvingGraph = new EvolvingGraph();
    _evolvingGraph->setLoader(this);
    if(!projIn.isEmpty() && !projOut.isEmpty()) {
        _pjIn  = pj_init_plus(projIn.toStdString().c_str());
        _pjOut = pj_init_plus(projOut.toStdString().c_str());
    }
}

GraphLoader::GraphLoader(const GraphLoader &other) :
    QObject(0),
    _filename(other._filename),
    _lineRegex(other._lineRegex),
    _timeFormat(other._timeFormat),
    _headers(other._headers)
{
    _forceStop = false;
    _loadProgress = 0;
    _evolvingGraph = new EvolvingGraph();
    _evolvingGraph->setLoader(this);
    _pjIn = other._pjIn;
    _pjOut = other._pjOut;
}

GraphLoader::~GraphLoader()
{
}

/*!
 * \brief GraphLoader::constEvolvingGraph
 * \return a const reference to the partially loaded evolving graph
 */
const AbstractEvolvingGraph * GraphLoader::constEvolvingGraph() const
{
    return _evolvingGraph;
}

/*!
 * \brief GraphLoader::evolvingGraph return the loaded graph if the file is fully loaded (check this with isFinished()). Otherwise return 0;
 * \return a pointer to the loaded graph
 */
AbstractEvolvingGraph * GraphLoader::evolvingGraph() const
{
    if(!isLoaded())
    {
        return 0;
    }
    return _evolvingGraph;
}


bool GraphLoader::load()
{
    QFile * file = new QFile(_filename);
    if(!file->open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    for(int i = 0; i< 1000; ++i)
    {
        if(!file->atEnd())
            processLine(QString(file->readLine()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts).at(0));
    }

    _loadProgress = 1.0 - file->bytesAvailable() / (qreal)file->size();
    _loadResult = QtConcurrent::run(this, &GraphLoader::concurrentLoad, file);
    return true;
}

bool GraphLoader::concurrentLoad(QFile * file)
{
    int limit = 20;
    while(!file->atEnd())
    {
        if(--limit < 0)
        {
            break;
        }
        for(int i = 0; i< 10000; ++i)
        {
            if(file->atEnd())
                break;
            processLine(QString(file->readLine()).split(QRegExp("[\r\n]"), QString::SkipEmptyParts).at(0));
        }
        _loadProgress = 1.0 - file->bytesAvailable() / (qreal)file->size();
        emit onLoadProgressChanged(_loadProgress);
        if(_forceStop)
        {
            return false;
        }
    }
    _loadProgress = 1.0;
    emit onLoadProgressChanged(_loadProgress);
    file->close();
    file->deleteLater();
    _isFinished = true;
    emit loaded();
    return true;
}

void GraphLoader::cancelLoadAndWait()
{
    if(!_loadResult.isFinished())
    {
        _forceStop = true;
        _loadResult.waitForFinished();
    }
}

QDateTime GraphLoader::toDateTime(QString dateTime, QString format)
{
    QDateTime dt;
    if(format == "T") {
        dt = QDateTime::fromTime_t(dateTime.toUInt());
    } else if(format == "t") {
        dt = QDateTime::fromMSecsSinceEpoch(dateTime.toLongLong());
    } else {
        dt = QDateTime::fromString(dateTime, format);
    }
    if(dt.isValid()) {
        return dt;
    } else {
        return QDateTime();
    }
}

void GraphLoader::processLine(QString line)
{
    if(-1 == _lineRegex.indexIn(line))
    {
        qWarning() << tr("Regex does not match line: ") << line;
        return;
    }
    QStringList capturedText = _lineRegex.capturedTexts();

    // The number of captured text + the whole matched text must equals the number of headers
    if(capturedText.size() != _headers.size() + 1)
    {
        qWarning() << tr("The number of captured text does not match the number of headers");
        return;
    }

    int id = 0;
    double lat = 0, lon = 0, x = 0, y = 0;
    QString time;
    for(int i = 0; i < _headers.size(); ++i)
    {
        QString header = _headers.at(i);
        if(header == "Id") {
            id = capturedText.at(i + 1).toInt();
        } else if(header == "Time") {
            time = capturedText.at(i + 1);
        } else if(header == X) {
            lat = capturedText.at(i + 1).toDouble();

        } else if(header == Y){
            lon = capturedText.at(i + 1).toDouble();
        }
    }

    QDateTime dt = toDateTime(time, _timeFormat);
    mvtime mvt = dt.toTime_t();
    if(!dt.isValid()) {
        qWarning() << tr("The time ") << time << tr(" does not match the given time format ") << _timeFormat;
        return;
    }

    // Transformation of the lat/lon coordinates to projected coordinates
    if(_pjIn && _pjOut) {
        x = lon * DEG_TO_RAD;
        y = lat * DEG_TO_RAD;
        pj_transform(_pjIn, _pjOut, 1, 1, &x, &y, NULL);
    } else {
        x = lon;
        y = lat;
    }

    QHash<QString, QVariant> props;
    props.insert(X, x);
    props.insert(Y, y);
    _evolvingGraph->addNode(id, mvt, props);
}

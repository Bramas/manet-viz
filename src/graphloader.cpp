#include "graphloader.h"
#include <QFile>
#include <QTextStream>
#include <ctime>

#include "abstractevolvinggraph.h"
#include "geometricgraph.h"

GraphLoader::GraphLoader(QString filename, QRegExp lineRegex, QList<TraceHeader> headers, QString timeFormat) :
    _filename(filename),
    _lineRegex(lineRegex),
    _timeFormat(timeFormat),
    _headers(headers)
{
    _forceStop = false;
    _loadProgress = 0;
    _evolvingGraph = new GeometricGraph();
}

GraphLoader::GraphLoader(const GraphLoader &other) :
    _filename(other._filename),
    _lineRegex(other._lineRegex),
    _timeFormat(other._timeFormat),
    _headers(other._headers)
{
    _forceStop = false;
    _loadProgress = 0;
    _evolvingGraph = new GeometricGraph();
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
    if(!isFinished())
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
    for(int i = 0; i< 100; ++i)
    {
        if(!file->atEnd())
            processLine(QString(file->readLine()));
    }

    _loadProgress = 1.0 - file->bytesAvailable() / (qreal)file->size();
    _loadResult = QtConcurrent::run(this, &GraphLoader::concurrentLoad, file);
    return true;
}

bool GraphLoader::concurrentLoad(QFile * file)
{
    while(!file->atEnd())
    {
        for(int i = 0; i< 10000; ++i)
        {
            if(!file->atEnd())
                processLine(QString(file->readLine()));
        }
        _loadProgress = 1.0 - file->bytesAvailable() / (qreal)file->size();
        emit onLoadProgressChanged(_loadProgress);
        if(_forceStop)
        {
            return false;
        }
    }

    file->close();
    file->deleteLater();
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
    qreal x = 0, y = 0;
    QString time;
    for(int i = 0; i < _headers.size(); ++i)
    {
        switch(_headers.at(i))
        {
        case IdHeader:
            id = capturedText.at(i + 1).toInt();
            break;
        case  XHeader:
            x = capturedText.at(i + 1).toDouble();
            break;
        case  YHeader:
            y = capturedText.at(i + 1).toDouble();
            break;
        case  TimeHeader:
            time = capturedText.at(i + 1);
            break;
        default:
            break;
        }
    }

    QDateTime dt = toDateTime(time, _timeFormat);
    mvtime mvt = dt.toTime_t();
    if(!dt.isValid()) {
        qWarning() << tr("The time ") << time << tr(" is not matching the given time format ") << _timeFormat;
        return;
    }

    QPointF p(x, y);
    dynamic_cast<GeometricGraph*>(_evolvingGraph)->addNodePosition(id, mvt, p);
}

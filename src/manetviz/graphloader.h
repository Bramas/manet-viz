#ifndef GRAPHLOADER_H
#define GRAPHLOADER_H

#include <QObject>
#include <QList>
#include <QtConcurrent>
#include <QFile>


#include "types.h"

class AbstractEvolvingGraph;

class GraphLoader : public QObject
{
    Q_OBJECT
public:
    explicit GraphLoader(QString filename, QRegExp lineRegex, QList<QString> headers, QString timeFormat);
    GraphLoader(const GraphLoader &other);
    ~GraphLoader();

    const AbstractEvolvingGraph *   constEvolvingGraph() const;
    AbstractEvolvingGraph *         evolvingGraph() const;

    bool    isFinished() const { return _isFinished; }

    qreal loadProgress() const { return _loadProgress; }
    void cancelLoadAndWait();

signals:
    void onLoadProgressChanged(qreal);
public slots:
    bool load();

private:
    void processLine(QString line);
    bool concurrentLoad(QFile * file);
    QDateTime toDateTime(QString dateTime, QString format);
    QString _filename;
    QRegExp _lineRegex;
    QString _timeFormat;
    QList<QString> _headers;

    QFuture<bool> _loadResult;
    qreal _loadProgress;
    bool _forceStop;

    AbstractEvolvingGraph * _evolvingGraph;

    bool _isFinished;
};

#endif // GRAPHLOADER_H

#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QDockWidget>
#include <QTimer>
#include <QElapsedTimer>

#include "types.h"

namespace Ui {
class ControlWidget;
}

class Viewer;
class Project;

class ControlWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent, Project *project);
    ~ControlWidget();

    void setViewer(Viewer * viewer);

public slots:
    void setLoadProgress(qreal);

signals:
    void timeChanged(mvtime);

private slots:
    void emitTimeChanged();
    void onPlayTimerTimeout();
    void play();
    void stop();
    void togglePlay();

private:
    Ui::ControlWidget *ui;
    QTimer _playTimer;
    QElapsedTimer _playTimerElapsed;
    qreal _cumulatedTime;
    bool _isPlaying;
    Viewer * _viewer;
    Project * _project;
};

#endif // CONTROLWIDGET_H

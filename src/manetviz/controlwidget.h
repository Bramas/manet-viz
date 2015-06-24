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

class ControlWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = 0);
    ~ControlWidget();

    void setViewer(Viewer * viewer);

public slots:
    void setLoadProgress(qreal);

signals:
    void timeChanged(mvtime);
    void communicationRangeChanged(qreal);
    void contactDisplayChanged(bool);
    void rangeDisplayChanged(bool);

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
};

#endif // CONTROLWIDGET_H

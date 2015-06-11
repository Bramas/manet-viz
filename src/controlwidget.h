#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QDockWidget>
#include <QTimer>
#include <QElapsedTimer>

#include "types.h"

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = 0);
    ~ControlWidget();

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
};

#endif // CONTROLWIDGET_H

#ifndef SIMULATION_H
#define SIMULATION_H

#include "simulation_global.h"
#include <QObject>
#include "iplugin.h"

namespace Ui {
    class Control;
}

class SIMULATIONSHARED_EXPORT Simulation: public QObject, public IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "simulation.json")
  Q_INTERFACES(IPlugin)

public:
    Simulation();

    void setProject(Project * project);
    QObject * getQObject() { return this; }
    QWidget *createControlWidget() const;

signals:
    void timeChanged(mvtime);

private:
    Ui::Control *ui;
    QTimer _playTimer;
    int _updateCount;
    qreal _updateDuration;
    QElapsedTimer _playTimerElapsed;
    bool _isPlaying;

    void play();
    void stop();

private slots:
    void togglePlay();
    void emitTimeChanged();
    void onPlayTimerTimeout();
    void onSpeedSliderChange();
    void modelUpdated(mvtime);

};

#endif // SIMULATION_H

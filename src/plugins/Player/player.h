#ifndef PLAYER_H
#define PLAYER_H

#include "player_global.h"

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include "iplugin.h"

namespace Ui {
    class Control;
}

class PLAYERSHARED_EXPORT Player: public QObject, public IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "player.json")
  Q_INTERFACES(IPlugin)

public:
    Player();
    QObject * getQObject() { return this; }
    void setProject(Project * project);
    QWidget *createControlWidget() const;

private:
    Ui::Control *ui;
    bool _isPlaying;
    QTimer _playTimer;
    QElapsedTimer _playTimerElapsed;
    qreal _cumulatedTime;

signals:
    void timeChanged(mvtime);

private slots:
    void togglePlay();
    void onPlayTimerTimeout();
    void play();
    void stop();
    void emitTimeChanged();

};

#endif // PLAYER_H

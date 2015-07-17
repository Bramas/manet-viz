#include "player.h"
#include "project.h"

#include "ui_control.h"

Player::Player():
    ui(new Ui::Control),
    _cumulatedTime(0),
    _isPlaying(false)
{
}

QWidget *Player::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);

    ui->spinBoxSpeed->setValue(60);
    ui->spinBoxTime->setMaximum(99999999);

    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(togglePlay()));
    connect(&_playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimerTimeout()));
    connect(ui->spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(emitTimeChanged()));

    return control;
}

void Player::emitTimeChanged()
{
    mvtime t = (mvtime)ui->spinBoxTime->value();
    emit timeChanged(t);
}

void Player::togglePlay()
{
    if(_isPlaying)
    {
        stop();
    }
    else
    {
        play();
    }
}

void Player::setProject(Project *project)
{
    _project = project;
    connect(this, SIGNAL(timeChanged(mvtime)), _project->viewer(), SLOT(setTime(mvtime)));
}

void Player::play()
{
    _isPlaying = true;
    ui->spinBoxTime->setEnabled(false);
    ui->pushButtonPlay->setText(tr("Stop"));
    _playTimerElapsed.start();
    _playTimer.start(1);
}
void Player::stop()
{
    _isPlaying = false;
    ui->spinBoxTime->setEnabled(true);
    ui->pushButtonPlay->setText(tr("Play"));
    _playTimer.stop();
}
void Player::onPlayTimerTimeout()
{
    _cumulatedTime += ui->spinBoxSpeed->value()*_playTimerElapsed.elapsed()/100.0;
    _playTimerElapsed.start();
    if(floor(_cumulatedTime) >= 1.0)
    {
        int add = floor(_cumulatedTime);
        _cumulatedTime -= add;
        ui->spinBoxTime->setValue(ui->spinBoxTime->value()+add);
    }
}

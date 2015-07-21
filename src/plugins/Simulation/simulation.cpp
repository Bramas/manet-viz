#include "simulation.h"
#include "ui_control.h"

#include "project.h"


Simulation::Simulation():
    ui(new Ui::Control),
    _isPlaying(false)
{

}

void Simulation::setProject(Project *project)
{
    _project = project;
    connect(this, SIGNAL(timeChanged(mvtime)), _project->viewer(), SLOT(setTime(mvtime)));
    connect(_project->viewer(), SIGNAL(updated(mvtime)), this, SLOT(modelUpdated(mvtime)));
}

QWidget *Simulation::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);

    // initialize all ui elements
    ui->labelSpeed->setText(QString::number(1) + "s");
    ui->horizontalSliderSpeed->setValue(1);
    ui->spinBoxTime->setMaximum(99999999);
    ui->labelSimSpeed->setText("0 f/s");

    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(togglePlay()));
    connect(ui->spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(emitTimeChanged()));
    connect(&_playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimerTimeout()));
    connect(ui->horizontalSliderSpeed, SIGNAL(valueChanged(int)), this, SLOT(onSpeedSliderChange()));

    return control;
}

void Simulation::play()
{
    _isPlaying = true;
    _updateCount = 0;
    _updateDuration = 0.0;
    _playTimer.start(1000);
    ui->spinBoxTime->setEnabled(false);
    ui->pushButtonPlay->setText(tr("Stop"));
    _playTimerElapsed.start();
    emitTimeChanged();
}

void Simulation::stop()
{
    _isPlaying = false;
    _playTimer.stop();
    ui->spinBoxTime->setEnabled(true);
    ui->pushButtonPlay->setText(tr("Play"));
}

void Simulation::togglePlay()
{
    if(_isPlaying)
        stop();
    else
        play();
}

void Simulation::emitTimeChanged()
{
    mvtime t = (mvtime)ui->spinBoxTime->value();
    emit timeChanged(t);
}

void Simulation::onPlayTimerTimeout()
{
    ui->labelSimSpeed->setText(QString::number(1/(_updateDuration / _updateCount))+" f/s");
    _updateDuration = 0.0;
    _updateCount = 0.0;
}

void Simulation::onSpeedSliderChange()
{
    ui->labelSpeed->setText(QString::number(ui->horizontalSliderSpeed->value()) + "s");
}

void Simulation::modelUpdated(mvtime time)
{
    if(_isPlaying) {
        _updateDuration += _playTimerElapsed.elapsed()/1000.0;
        _updateCount++;
        _playTimerElapsed.start();
        ui->spinBoxTime->setValue(ui->spinBoxTime->value()+ui->horizontalSliderSpeed->value());
    }
}


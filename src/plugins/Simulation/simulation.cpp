#include "simulation.h"
#include "ui_control.h"

#include "project.h"


Simulation::Simulation():
    ui(new Ui::Control)
{

}

void Simulation::setProject(Project *project)
{
    _project = project;
    connect(this, SIGNAL(timeChanged(mvtime)), _project->viewer(), SLOT(setTime(mvtime)));
}

QWidget *Simulation::createControlWidget() const
{
    QWidget * control = new QWidget();
    ui->setupUi(control);

    // initialize all ui elements
    ui->labelSpeed->setText(QString::number(1) + "s");
    ui->horizontalSliderSpeed->setValue(1);
    ui->spinBoxTime->setMaximum(99999999);

    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(togglePlay()));
    connect(ui->spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(emitTimeChanged()));
    connect(&_playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimerTimeout()));
    connect(ui->horizontalSliderSpeed, SIGNAL(valueChanged(int)), this, SLOT(onSpeedSliderChange()));

    return control;
}

void Simulation::play()
{
    _isPlaying = true;
    ui->spinBoxTime->setEnabled(false);
    ui->pushButtonPlay->setText(tr("Stop"));
    _playTimer.start(1);
}

void Simulation::stop()
{
    _isPlaying = false;
    ui->spinBoxTime->setEnabled(true);
    ui->pushButtonPlay->setText(tr("Play"));
    _playTimer.stop();
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
    ui->spinBoxTime->setValue(ui->spinBoxTime->value()+ui->horizontalSliderSpeed->value());
}

void Simulation::onSpeedSliderChange()
{
    ui->labelSpeed->setText(QString::number(ui->horizontalSliderSpeed->value()) + "s");
}


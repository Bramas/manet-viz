#include "controlwidget.h"
#include "ui_controlwidget.h"

ControlWidget::ControlWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ControlWidget),
    _cumulatedTime(0),
    _isPlaying(false)
{
    ui->setupUi(this);

    ui->spinBoxTime->setMaximum(99999999);
    ui->spinBoxSpeed->setValue(60);
    connect(ui->spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(emitTimeChanged()));
    connect(&_playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimerTimeout()));
    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(togglePlay()));
    connect(ui->checkBoxContacts, &QCheckBox::stateChanged, [this]
        {
            emit contactDisplayChanged(ui->checkBoxContacts->isChecked());
        });
    connect(ui->checkBoxRange, &QCheckBox::stateChanged, [this]
        {
            emit rangeDisplayChanged(ui->checkBoxRange->isChecked());
        });
    connect(ui->sliderRange, &QSlider::valueChanged, [this](int v)
        {
            emit communicationRangeChanged((qreal)v);
        });
    _playTimerElapsed.start();
}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::emitTimeChanged()
{
    mvtime t = (mvtime)ui->spinBoxTime->value();
    emit timeChanged(t);
}
void ControlWidget::togglePlay()
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

void ControlWidget::play()
{
    _isPlaying = true;
    ui->spinBoxTime->setEnabled(false);
    ui->pushButtonPlay->setText(tr("Stop"));
    _playTimerElapsed.start();
    _playTimer.start(1);
}
void ControlWidget::stop()
{
    _isPlaying = false;
    ui->spinBoxTime->setEnabled(true);
    ui->pushButtonPlay->setText(tr("Play"));
    _playTimer.stop();
}
void ControlWidget::onPlayTimerTimeout()
{
    _cumulatedTime += ui->spinBoxSpeed->value()*_playTimerElapsed.elapsed()/100.0;
    _playTimerElapsed.start();
    if(floor(_cumulatedTime) >= 1.0)
    {
        int add = floor(_cumulatedTime);
        _cumulatedTime -= add;
        ui->spinBoxTime->setValue(ui->spinBoxTime->value()+add);
        emitTimeChanged();
    }
}

void ControlWidget::setLoadProgress(qreal f)
{
    ui->progressBarImport->setValue(f*1000);
}

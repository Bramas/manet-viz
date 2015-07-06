#include "controlwidget.h"
#include "ui_controlwidget.h"
#include "viewer.h"
#include "iviewerlayer.h"
#include "igraphdecorator.h"
#include "project.h"

#include <QSpacerItem>

ControlWidget::ControlWidget(QWidget *parent, Project * project) :
    QDockWidget(parent),
    ui(new Ui::ControlWidget),
    _cumulatedTime(0),
    _isPlaying(false)
{
    _project = project;
    ui->setupUi(this);
    ui->dockWidgetContents->setLayout(ui->verticalLayout);

    _viewer = 0;

    ui->spinBoxTime->setMaximum(99999999);
    ui->spinBoxSpeed->setValue(60);
    connect(ui->spinBoxTime, SIGNAL(valueChanged(int)), this, SLOT(emitTimeChanged()));
    connect(&_playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimerTimeout()));
    connect(ui->pushButtonPlay, SIGNAL(clicked(bool)), this, SLOT(togglePlay()));
    /*connect(ui->checkBoxContacts, &QCheckBox::stateChanged, [this]
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
        });*/
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
    }
}

void ControlWidget::setLoadProgress(qreal f)
{
    ui->progressBarImport->setValue(f*1000);
}

void ControlWidget::setViewer(Viewer *viewer)
{
    _viewer = viewer;
    connect(this, SIGNAL(timeChanged(mvtime)), _viewer, SLOT(setTime(mvtime)));

    foreach(IViewerLayer * layer, _project->layers())
    {
        QWidget * w = layer->createControlWidget();
        if(w)
        {
            ui->verticalLayout->addWidget(w);
            qDebug()<<"add control for "<<layer->getQObject()->objectName();
        }
    }
    //add vertical Spacer
    ui->verticalLayout->addSpacerItem(new QSpacerItem(20,20,QSizePolicy::Minimum, QSizePolicy::Expanding));
}

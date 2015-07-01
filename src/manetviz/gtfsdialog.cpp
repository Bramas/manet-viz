#include "gtfsdialog.h"
#include "ui_gtfsdialog.h"

#include <QPushButton>

GTFSDialog::GTFSDialog(QString pathname, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTFSDialog),
    _pathname(pathname)
{
    ui->setupUi(this);

    // Set default values
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->lineEditTrace->setText((new QFileInfo(pathname))->fileName());

    QSettings settings;

    _projIns = settings.value("savedProjInsGTFS", QStringList()).toStringList();
    if(_projIns.count() > 0) {
        ui->comboBoxInputProj->addItems(_projIns);
    }
    _projIn = ui->comboBoxInputProj->currentText();

    _projOuts = settings.value("savedProjOutsGTFS", QStringList()).toStringList();
    if(_projOuts.count() > 0) {
        ui->comboBoxOutputProj->addItems(_projOuts);
    }
    _projOut = ui->comboBoxOutputProj->currentText();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(ui->comboBoxInputProj,  SIGNAL(editTextChanged(QString)), this, SLOT(projInEdited(QString)));
    connect(ui->comboBoxOutputProj, SIGNAL(editTextChanged(QString)), this, SLOT(projOutEdited(QString)));

    projInEdited(_projIn);
    projOutEdited(_projOut);
}

GTFSDialog::~GTFSDialog()
{
    delete ui;
}

void GTFSDialog::projInEdited(QString text)
{
    _projIn = text;
    if(!text.isEmpty()) {
        _isProjInValid = toggleBoldFont(ui->comboBoxInputProj, isValidProj(text));
    } else {
        _isProjInValid = true;
    }
    checkConsistency();
}

void GTFSDialog::projOutEdited(QString text)
{
    _projOut = text;
    if(!text.isEmpty()) {
        _isProjOutValid = toggleBoldFont(ui->comboBoxOutputProj, isValidProj(text));
    } else {
        _isProjOutValid = true;
    }
    checkConsistency();
}

bool GTFSDialog::isValidProj(QString proj)
{
    projPJ pj = pj_init_plus(proj.toStdString().c_str());
    bool ret = true;
    if(!pj) {
        ret = false;
    }
    pj_free(pj);
    return ret;
}

bool GTFSDialog::toggleBoldFont(QComboBox * combo, bool isValid) {
    QFont prevFont(combo->font()); // Get previous font
    if(isValid) {
        prevFont.setBold(false);
        combo->setFont(prevFont);
    } else {
        prevFont.setBold(true);
        combo->setFont(prevFont);
    }
    return isValid;
}

void GTFSDialog::checkConsistency()
{
    // Consistency check
    bool flag = false;

    // Check with the time format
    if((_projIn.isEmpty() && !_projOut.isEmpty()) || (!_projIn.isEmpty() && _projOut.isEmpty())) {
        flag = true;
    }
    if(!_isProjInValid || !_isProjOutValid) {
        flag = true;
    }

    // Disable the "import" button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(flag);
}

void GTFSDialog::onAccepted()
{
    // Save the current input projection in the settings
    // if it is already there, put on top
    // if there are more than 5 items, delete the last one (least recent one)

    if(!ui->comboBoxInputProj->currentText().isEmpty()){
        QString projIn = ui->comboBoxInputProj->currentText();
        int idx = _projIns.indexOf(projIn);
        if(idx != -1) {
            _projIns.removeAt(idx);
        }
        _projIns.push_front(projIn);
        if(_projIns.count() > 5) {
            _projIns.removeLast();
        }
    }

    // Do the same for the output projection
    if(!ui->comboBoxOutputProj->currentText().isEmpty()){
        QString projOut = ui->comboBoxOutputProj->currentText();
        int idx = _projOuts.indexOf(projOut);
        if(idx != -1) {
            _projOuts.removeAt(idx);
        }
        _projOuts.push_front(projOut);
        if(_projOuts.count() > 5) {
            _projOuts.removeLast();
        }
    }

    QSettings settings;
    settings.setValue("savedProjInsGTFS", _projIns);
    settings.setValue("savedProjOutsGTFS", _projOuts);
    QDialog::accept();
}

GTFSLoader GTFSDialog::createGTFSLoader()
{
    return GTFSLoader(_pathname, _projIn, _projOut);
}

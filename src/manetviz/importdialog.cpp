#include "importdialog.h"
#include "ui_importdialog.h"
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QListView>
#include <QStandardItemModel>
#include <QCloseEvent>
#include <QCompleter>

#include "ComboBoxDelegate.h"
#include "csvparser.h"
#include "types.h"
#include "graphloader.h"

ImportDialog::ImportDialog(QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog),
    _filename(filename)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    _inputModel = new QStandardItemModel();
    _outputModel = new QStandardItemModel();
    ui->tableViewOutput->setModel(_outputModel);
    ui->listViewInput->setModel(_inputModel);

    // Set default values
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->lineEditTrace->setText((new QFileInfo(filename))->fileName());
    ui->checkBoxHeading->setChecked(false);
    ui->checkBoxMobility->setChecked(true);

    // Add the stored regexps in the combolist
    QSettings settings;
    _regExps = settings.value("savedRegExps", QStringList()).toStringList();
    if(_regExps.count() > 0) {
        ui->comboBoxRegEx->addItems(_regExps);
    }
    _regEx = ui->comboBoxRegEx->currentText();

    _timeFormats = settings.value("savedTimeFormats", QStringList()).toStringList();
    if(_timeFormats.count() > 0) {
        ui->comboBoxTimeFormat->addItems(_timeFormats);
    }
    _timeFormat = ui->comboBoxTimeFormat->currentText();

    _projIns = settings.value("savedProjIns", QStringList()).toStringList();
    if(_projIns.count() > 0) {
        ui->comboBoxInputProj->addItems(_projIns);
    }
    _projIn = ui->comboBoxInputProj->currentText();

    _projOuts = settings.value("savedProjOuts", QStringList()).toStringList();
    if(_projOuts.count() > 0) {
        ui->comboBoxOutputProj->addItems(_projOuts);
    }
    _projOut = ui->comboBoxOutputProj->currentText();

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->checkBoxContact, SIGNAL(stateChanged(int)), this, SLOT(contactChanged()));
    connect(ui->checkBoxMobility, SIGNAL(stateChanged(int)), this, SLOT(mobilityChanged()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(ui->comboBoxRegEx, SIGNAL(editTextChanged(QString)), this, SLOT(regExEdited(QString)));
    connect(ui->comboBoxTimeFormat, SIGNAL(editTextChanged(QString)), this, SLOT(timeFormatEdited(QString)));
    connect(ui->comboBoxInputProj, SIGNAL(editTextChanged(QString)), this, SLOT(projInEdited(QString)));
    connect(ui->comboBoxOutputProj, SIGNAL(editTextChanged(QString)), this, SLOT(projOutEdited(QString)));
    connect(_outputModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(headerChanged(QStandardItem *)));

    // default for test purpose
    //ui->lineEditRegEx->setText("(\\d+);([^;]+);POINT\\(([^ ]+) ([^ ]+)\\)");

    processSampleTrace(10);
    processInputTable();
    processOutputTable();
}

void ImportDialog::processSampleTrace(int nbLines)
{
    QFile * file = new QFile(_filename);
    if(!file->open(QFile::ReadOnly | QFile::Text))
    {
        return;
    }

    for(int i = 0; i < nbLines; ++i)
    {
        if(!file->atEnd()) {
            _sampleTrace.append(QString(file->readLine()).split(QRegExp("[\r\n]"),QString::SkipEmptyParts).at(0));
        } else {
            break;
        }
    }
}

void ImportDialog::processInputTable()
{
    foreach(QString line, _sampleTrace) {
        QStandardItem * item = new QStandardItem(line);
        _inputModel->appendRow(item);
    }
}

void ImportDialog::processOutputTable()
{
    // Re-initilize the attributes
    _outputModel->clear();
    _headers.clear();
    _TraceHeaders.clear();
    _timeCol = -1;
    _isLoading = true;

    CSVParser parser = CSVParser();

    ComboBoxDelegate * delegate = new ComboBoxDelegate();
    QStringList defaultHeaders;
    defaultHeaders << "Id" << "Time" << "X" << "Y";
    int idx = 0;
    if (_hasHeading) {
        ui->tableViewOutput->horizontalHeader()->setVisible(true);
        QStringList fields;
        QString line = _sampleTrace[idx++];
        parser.parseRegEx(line, fields, _regEx);
        int j = 0;
        foreach(const QString &field, fields)
        {
            QStandardItem * item = new QStandardItem(field);
            QStandardItem * emptyItem = new QStandardItem();
            emptyItem->setBackground(QBrush(QColor(237,237,237)));
            _headers.append(field);
            _outputModel->setHorizontalHeaderItem(j,item);
            _outputModel->setItem(0,j,emptyItem);
            j++;
        }
    } else {
        ui->tableViewOutput->horizontalHeader()->setVisible(false);
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields, _regEx);
        for(int i = 0; i<fields.count(); ++i)
        {
            QString field = "";
            if(idx < defaultHeaders.size())
            {
                field = defaultHeaders.at(i);
            }
            QStandardItem * item = new QStandardItem(field);
            item->setData(field, Qt::EditRole);
            _outputModel->setItem(0,i,item);
        }
    }
    ui->tableViewOutput->setItemDelegateForRow(0,delegate);
    int i = 1;
    for(; idx<_sampleTrace.count(); ++idx) {
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields, _regEx);

        int j = 0;
        foreach(QString field, fields) {
            QStandardItem * item = new QStandardItem(field);
            _outputModel->setItem(i,j,item);
            j++;
        }
        i++;
    }

    _isLoading = false;
    checkConsistency();
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::headingChanged()
{
    _hasHeading = ui->checkBoxHeading->isChecked();
    processOutputTable();
    checkConsistency();
}
void ImportDialog::contactChanged()
{
    _isContactTrace = ui->checkBoxContact->isChecked();
    processOutputTable();
    checkConsistency();
}
void ImportDialog::mobilityChanged()
{
    _isMobilityTrace = ui->checkBoxMobility->isChecked();
    processOutputTable();
    checkConsistency();
}

void ImportDialog::headerChanged(QStandardItem * item)
{
    int row = item->row();
    int column = item->column();

    if(row == 0) {
        QString header = _outputModel->item(0,column)->data(Qt::EditRole).toString();
        if(!header.isEmpty()) {
            // Not default empty value for header
            _TraceHeaders[column] = header;
            if(header == "Time") {
                // Update the time columnw used in the time format validity test
                _timeCol = column;
            } else {
                // check whether there is another time header in the current header
                int idx = _TraceHeaders.values().indexOf("Time");
                if(idx != -1) {
                    // Update the time column to the first index found
                    _timeCol = idx;
                } else {
                    // Clear the time column
                    _timeCol = -1;
                }
            }
        }
    }

    checkConsistency();
}

void ImportDialog::regExEdited(QString text)
{
    _regEx = text;
    if(!text.isEmpty()) {
        _isRegExValid = toggleBoldFont(ui->comboBoxRegEx, isValidRegEx(text));
    } else {
        _isRegExValid = true;
    }

    processOutputTable();
    checkConsistency();
}

void ImportDialog::projInEdited(QString text)
{
    _projIn = text;
    if(!text.isEmpty()) {
        _isProjInValid = toggleBoldFont(ui->comboBoxInputProj, isValidProj(text));
        qDebug() << "ProjIn is not empty - " << _isProjInValid;
    } else {
        _isProjInValid = true;
        qDebug() << "ProjIn is empty - " << _isProjInValid;
    }
    checkConsistency();
}

void ImportDialog::projOutEdited(QString text)
{
    _projOut = text;
    if(!text.isEmpty()) {
        _isProjOutValid = toggleBoldFont(ui->comboBoxOutputProj, isValidProj(text));
        qDebug() << "ProjOut is not empty - " << _isProjOutValid;
    } else {
        _isProjOutValid = true;
        qDebug() << "ProjOut is not empty - " << _isProjOutValid;
    }
    checkConsistency();
}

void ImportDialog::timeFormatEdited(QString text)
{
    _timeFormat = text;
    if(!text.isEmpty()) {
        _isTimeFormatValid = toggleBoldFont(ui->comboBoxTimeFormat, isTimeFormatValid(_timeFormat));
    } else {
        _isTimeFormatValid = false;
    }
    checkConsistency();
}

bool ImportDialog::isValidProj(QString proj)
{
    projPJ pj = pj_init_plus(proj.toStdString().c_str());
    bool ret = true;
    if(!pj) {
        ret = false;
    }
    pj_free(pj);
    return ret;
}

bool ImportDialog::toggleBoldFont(QComboBox * combo, bool isValid) {
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

bool ImportDialog::isTimeFormatValid(QString format)
{
    if(_timeCol != -1) {
        // Check whehter the time format is valid
        QDateTime dt;
        QString timeSample = _outputModel->item(2,_timeCol)->data(Qt::EditRole).toString();
        qDebug() << timeSample << format;
        if(format == "T") {
            dt = QDateTime::fromTime_t(timeSample.toUInt());
        } else if(format == "t") {
            dt = QDateTime::fromMSecsSinceEpoch(timeSample.toLongLong());
        } else {
            dt = QDateTime::fromString(timeSample, format);
        }
        return dt.isValid();
    } else {
        return false;
    }
}

bool ImportDialog::isValidRegEx(QString regex)
{
    return QRegExp(regex).isValid();
}


void ImportDialog::checkConsistency()
{
    // Consistency check
    bool flag = false;
    // discard if the output table is still loading
    if(_isLoading) {
        flag = true;
    } else {
        qDebug() << "Time: " << _isTimeFormatValid << "ProjIn" << _isProjInValid << "ProjOut" << _isProjOutValid;
        // Check with the time format
        if((_projIn.isEmpty() && !_projOut.isEmpty()) || (!_projIn.isEmpty() && _projOut.isEmpty())) {
            flag = true;
        }
        if(!_isProjInValid || !_isProjOutValid || !_isTimeFormatValid) {
            flag = true;
        }
        // Check whether the fields are well filled
        if(!(_TraceHeaders.values().count("Id") == 1 && _TraceHeaders.values().count("Time") == 1)) {
            flag = true;
        }
        if(_isMobilityTrace && !(_TraceHeaders.values().count("X") == 1 && _TraceHeaders.values().count("Y") == 1)) {
            flag = true;
        }
        if(_isContactTrace && !(_TraceHeaders.values().count("X2") == 1) && _TraceHeaders.values().count("TimeStart") == 1 && _TraceHeaders.values().count("TimeEnd") == 1) {
            flag = true;
        }
    }

    // Disable the "import" button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(flag);
}

void ImportDialog::onAccepted()
{
    // Save the current regex in the settings
    // if it is already there, put on top
    // if there are more than 5 items, delete the last one (least recent one)
    if(!ui->comboBoxRegEx->currentText().isEmpty()){
        QString regEx = ui->comboBoxRegEx->currentText();
        int idx = _regExps.indexOf(regEx);
        if(idx != -1) {
            _regExps.removeAt(idx);
        }
        _regExps.push_front(ui->comboBoxRegEx->currentText());
        if(_regExps.count() > 5) {
            _regExps.removeLast();
        }
    }

    // Do the same for the time format
    if(!ui->comboBoxTimeFormat->currentText().isEmpty()){
        QString timeFormat = ui->comboBoxTimeFormat->currentText();
        int idx = _timeFormats.indexOf(timeFormat);
        if(idx != -1) {
            _timeFormats.removeAt(idx);
        }
        _timeFormats.push_front(ui->comboBoxTimeFormat->currentText());
        if(_timeFormats.count() > 5) {
            _timeFormats.removeLast();
        }
    }

    QSettings settings;
    settings.setValue("savedRegExps", _regExps);
    settings.setValue("savedTimeFormats", _timeFormats);
    settings.setValue("savedProjIns", _projIns);
    settings.setValue("savedProjOuts", _projOuts);
}

GraphLoader ImportDialog::createGraphLoader()
{
    QRegExp regEx(ui->comboBoxRegEx->currentText());
    return GraphLoader(_filename, regEx, _TraceHeaders.values(), _timeFormat, _projIn, _projOut);
}

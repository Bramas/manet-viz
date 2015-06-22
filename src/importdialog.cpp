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

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->checkBoxContact, SIGNAL(stateChanged(int)), this, SLOT(contactChanged()));
    connect(ui->checkBoxMobility, SIGNAL(stateChanged(int)), this, SLOT(mobilityChanged()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(ui->comboBoxRegEx, SIGNAL(editTextChanged(QString)), this, SLOT(regExEdited(QString)));
    connect(ui->comboBoxTimeFormat, SIGNAL(editTextChanged(QString)), this, SLOT(timeFormatEdited(QString)));
    connect(_outputModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(headerChanged(QStandardItem *)));

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
    _timeSample.clear();

    CSVParser parser(_regEx);

    ComboBoxDelegate * delegate = new ComboBoxDelegate();

    int idx = 0;
    if (_hasHeading) {
        ui->tableViewOutput->horizontalHeader()->setVisible(true);
        QStringList fields;
        QString line = _sampleTrace[idx++];
        parser.parseRegEx(line, fields);
        int j = 0;
        foreach (QString field, fields) {
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
        parser.parseRegEx(line, fields);
        for(int i = 0; i<fields.count(); ++i) {
            QStandardItem * emptyItem = new QStandardItem();
            emptyItem->setBackground(QBrush(QColor(237,237,237)));
            _outputModel->setItem(0,i,emptyItem);
        }
    }
    ui->tableViewOutput->setItemDelegateForRow(0,delegate);
    int i = 1;
    for(; idx<_sampleTrace.count(); ++idx) {
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields);

        int j = 0;
        foreach(QString field, fields) {
            QStandardItem * item = new QStandardItem(field);
            _outputModel->setItem(i,j,item);
            j++;
        }
        i++;
    }
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

TraceHeader ImportDialog::convertToTraceHeader(QString header)
{
    if(header == "Id") return IdHeader;
    else if(header == "Time") return TimeHeader;
    else if(header == "X") return XHeader;
    else if(header == "Y") return YHeader;
    else if(header == "TimeStart") return TimeStartHeader;
    else if(header == "TimeEnd") return TimeEndHeader;
    else if(header == "Id2") return Id2Header;
    else if(header == "Property") return PropertyHeader;
    else return NoneHeader;
}

void ImportDialog::headerChanged(QStandardItem * item)
{
    int row = item->row();
    int column = item->column();

    if(row == 0) {
        QString headerText = _outputModel->item(0,column)->data(Qt::EditRole).toString();
        if(!headerText.isEmpty()) {
            // Not default empty value for header
            TraceHeader header = convertToTraceHeader(headerText);
            _TraceHeaders[column] = header;
            if(header == TimeHeader) {
                // Update the time sample used in the time format validity test
                _timeSample = _outputModel->item(2,column)->data(Qt::EditRole).toString();
            } else {
                // check whether there is another time header in the current header
                int idx = _TraceHeaders.values().indexOf(TimeHeader);
                if(idx != -1) {
                    // Update the time sample to the first index found
                    _timeSample = _outputModel->item(2,idx)->data(Qt::EditRole).toString();
                } else {
                    // Clear the time sample
                    _timeSample.clear();
                }
            }
        }
    }

    checkConsistency();
}

void ImportDialog::regExEdited(QString text)
{
    _regEx = text;
    QFont prevFont(ui->comboBoxRegEx->font()); // Get previous font
    if(isValidRegEx(text)) {
        prevFont.setBold(false);
        ui->comboBoxRegEx->setFont(prevFont);
    } else {
        prevFont.setBold(true);
        ui->comboBoxRegEx->setFont(prevFont);
    }

    processOutputTable();
    checkConsistency();
}

bool ImportDialog::isTimeFormatValid(QString format)
{
    if(!_timeSample.isEmpty()) {
        // Check whehter the time format is valid
        QDateTime dt;
        if(format == "T") {
            dt = QDateTime::fromTime_t(_timeSample.toUInt());
        } else if(format == "t") {
            dt = QDateTime::fromMSecsSinceEpoch(_timeSample.toLongLong());
        } else {
            dt = QDateTime::fromString(_timeSample, format);
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
    // Check with the time format
    if(_timeFormat.isEmpty()) {
        flag = true;
    } else {
        bool isValid = isTimeFormatValid(_timeFormat);
        QFont prevFont(ui->comboBoxTimeFormat->font()); // Get previous font
        if(isValid) {
            prevFont.setBold(false);
            ui->comboBoxTimeFormat->setFont(prevFont);
        } else {
            prevFont.setBold(true);
            ui->comboBoxTimeFormat->setFont(prevFont);
        }
        flag = !isValid;
    }
    // Check whether the fields are well filled
    if(!(_TraceHeaders.values().count(IdHeader) == 1 && _TraceHeaders.values().count(TimeHeader) == 1)) {
        flag = true;
    }
    if(_isMobilityTrace && !(_TraceHeaders.values().count(XHeader) == 1 && _TraceHeaders.values().count(YHeader) == 1)) {
        flag = true;
    }
    if(_isContactTrace && !(_TraceHeaders.values().count(Id2Header) == 1) && _TraceHeaders.values().count(TimeStartHeader) == 1 && _TraceHeaders.values().count(TimeEndHeader) == 1) {
        flag = true;
    }

    // Disable the "import" button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(flag);
}

void ImportDialog::timeFormatEdited(QString text)
{
    _timeFormat = text;
    checkConsistency();
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
}

GraphLoader ImportDialog::createGraphLoader()
{
    QRegExp regEx(ui->comboBoxRegEx->currentText());
    return GraphLoader(_filename, regEx, _TraceHeaders.values(), _timeFormat);
}

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

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->checkBoxContact, SIGNAL(stateChanged(int)), this, SLOT(contactChanged()));
    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onAccepted()));
    connect(ui->comboBoxRegEx, SIGNAL(editTextChanged(QString)), this, SLOT(regExEdited(QString)));
    connect(_outputModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(checkConsistency(QStandardItem *)));

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
            _sampleTrace.append(QString(file->readLine()).split(QRegExp("[\r\n]"),QString::SkipEmptyParts));
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
            _headers.append(field);
            _outputModel->setHorizontalHeaderItem(j,item);
            _outputModel->setItem(0,j,new QStandardItem());
            j++;
        }
    } else {
        ui->tableViewOutput->horizontalHeader()->setVisible(false);
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields);
        for(int i = 0; i<fields.count(); ++i)
        {
            _outputModel->setItem(0,i,new QStandardItem());
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
}
void ImportDialog::contactChanged()
{
    _isContactTrace = ui->checkBoxContact->isChecked();
    processOutputTable();
}
void ImportDialog::mobilityChanged()
{
    _isMobilityTrace = ui->checkBoxMobility->isChecked();
    processOutputTable();
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

void ImportDialog::checkConsistency(QStandardItem * item)
{
    int row = item->row();
    int column = item->column();

    if(row == 0) {
        _TraceHeaders[column] = convertToTraceHeader(_outputModel->item(0,column)->data(Qt::EditRole).toString());
    }

    // Consistency check
    bool flag = false;
    if(!(_TraceHeaders.values().contains(IdHeader) && _TraceHeaders.values().contains(TimeHeader))) {
        flag = true;
    }
    if(_isMobilityTrace && !(_TraceHeaders.values().contains(XHeader) && _TraceHeaders.values().contains(YHeader))) {
        flag = true;
    }
    if(_isContactTrace && !(_TraceHeaders.values().contains(Id2Header))) {
        flag = true;
    }

    // Disable the "import" button
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(flag);
}

void ImportDialog::onAccepted()
{
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

    QSettings settings;
    settings.setValue("savedRegExps", _regExps);
}

void ImportDialog::regExEdited(QString text)
{
    _regEx = text;
    processOutputTable();
}

GraphLoader ImportDialog::createGraphLoader()
{
    QRegExp regEx(ui->comboBoxRegEx->currentText());
    return GraphLoader(_filename, regEx, _TraceHeaders.values());
}

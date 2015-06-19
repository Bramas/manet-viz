#include "importdialog.h"
#include "ui_importdialog.h"
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QListView>
#include <QStandardItemModel>
#include <QCloseEvent>

#include "csvparser.h"
#include "types.h"

ImportDialog::ImportDialog(QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog),
    _filename(filename)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    _inputModel = new QStandardItemModel();
    _outputModel = new QStandardItemModel();

    // Set default values
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    ui->lineEditTrace->setText((new QFileInfo(filename))->fileName());
    ui->checkBoxHeading->setChecked(false);
    ui->comboBoxQuote->setCurrentIndex(1);
    ui->comboBoxEscape->setCurrentIndex(0);
    ui->comboBoxSep->setCurrentIndex(2);

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->comboBoxSep, SIGNAL(activated(int)), this, SLOT(sepChanged(int)));
    connect(ui->comboBoxEscape, SIGNAL(activated(int)), this, SLOT(escapeChanged(int)));
    connect(ui->comboBoxQuote, SIGNAL(activated(int)), this, SLOT(quoteChanged(int)));
    connect(ui->comboBoxID, SIGNAL(activated(int)), this, SLOT(idChanged(int)));
    connect(ui->comboBoxTime, SIGNAL(activated(int)), this, SLOT(timeChanged(int)));
    connect(ui->comboBoxX, SIGNAL(activated(int)), this, SLOT(xChanged(int)));
    connect(ui->comboBoxY, SIGNAL(activated(int)), this, SLOT(yChanged(int)));

    processSampleTrace(10); // TODO: Guess the default variables
    processInputTable();
    processOutputTable();

    //    ui->listViewInput->setModel(model = new QStringListModel(this););
}

QChar ImportDialog::guessSep(QString line)
{
    QString commonSep = ";, \t";
    int maxValue = -1;
    QChar maxSep;
    foreach(QChar sep, commonSep) {
        int c = line.count(sep);
        if(c > maxValue) {
            maxValue = c;
            maxSep = sep;
        }
    }
    return maxSep;
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

    ui->listViewInput->setModel(_inputModel);
    ui->listViewInput->reset();
    ui->listViewInput->show();
}

void ImportDialog::processOutputTable()
{
    // Re-initilize the attributes
    _outputModel->clear();
    _headers.clear();
    ui->comboBoxID->clear();
    ui->comboBoxTime->clear();
    ui->comboBoxX->clear();
    ui->comboBoxY->clear();

    CSVParser parser(_quote, _sep, _escape);

    int idx = 0;
    if (_heading) {
        QStringList fields;
        QString line = _sampleTrace[idx++];
        parser.parseString(line, fields);
        int j = 0;
        foreach (QString field, fields) {
            QStandardItem * item = new QStandardItem(field);
            _headers.append(field);
            _outputModel->setHorizontalHeaderItem(j++,item);
        }
    } else {
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseString(line, fields);
        for(int i = 0; i<fields.count(); ++i)
        {
            _headers.append(QString(tr("Column %0")).arg(i));
        }
    }

    // Add the headers to the comboboxes
    ui->comboBoxID->addItems(_headers);
    ui->comboBoxTime->addItems(_headers);
    ui->comboBoxX->addItems(_headers);
    ui->comboBoxY->addItems(_headers);

    qDebug() << _idCol << " - " << _timeCol << " - " << _xCol << " - " << _yCol ;

    if(_idCol != -1) {
        _outputModel->setHorizontalHeaderItem(_idCol, new QStandardItem("ID"));
        ui->comboBoxID->setCurrentIndex(_idCol);
    }
    if(_timeCol != -1) {
        _outputModel->setHorizontalHeaderItem(_timeCol, new QStandardItem("TIME"));
        ui->comboBoxTime->setCurrentIndex(_timeCol);
    }
    if(_xCol != -1){
        _outputModel->setHorizontalHeaderItem(_xCol, new QStandardItem("X"));
        ui->comboBoxX->setCurrentIndex(_xCol);
    }
    if(_yCol != -1){
        _outputModel->setHorizontalHeaderItem(_yCol, new QStandardItem("Y"));
        ui->comboBoxY->setCurrentIndex(_yCol);
    }

    int i = 0;
    for(; idx<_sampleTrace.count(); ++idx) {
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseString(line, fields);

        int j = 0;
        foreach(QString field, fields) {
            QStandardItem * item = new QStandardItem(field);
            _outputModel->setItem(i,j,item);
            j++;
        }
        i++;
    }

    ui->tableViewOutput->setModel(_outputModel);
    ui->tableViewOutput->reset();
    ui->tableViewOutput->show();
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::headingChanged()
{
    _heading = this->ui->checkBoxHeading->isChecked();
    processOutputTable();
}

QString ImportDialog::processSepComboBox(int index)
{
    switch(index) {
        case 0: return " ";
        case 1: return ",";
        case 2: return ";";
        case 3: return "\t";
        default: return ";";
    }
}

QString ImportDialog::processEscapeComboBox(int index)
{
    switch(index) {
    case 0: return "\\";
    case 1: return "\"";
    default: return "\\";
    }
}

QString ImportDialog::processQuoteComboBox(int index)
{
    switch(index) {
        case 0: return "'";
        case 1: return "\"";
        case 2: return "";
        default: return "";
    }
}

void ImportDialog::sepChanged(int index)
{
    _sep = processSepComboBox(index);
    processOutputTable();
}

void ImportDialog::escapeChanged(int index)
{
    _escape = processEscapeComboBox(index);
    processOutputTable();
}

void ImportDialog::quoteChanged(int index)
{
    _quote = processQuoteComboBox(index);
    processOutputTable();
}

void ImportDialog::idChanged(int index)
{
    qDebug() << "We have - " << _idCol;
    if(_idCol != -1)
        _outputModel->setHorizontalHeaderItem(_idCol, new QStandardItem(QString::number(_idCol)));
    _idCol = index;
    _outputModel->setHorizontalHeaderItem(_idCol, new QStandardItem("ID"));
}

void ImportDialog::timeChanged(int index)
{
    if(_timeCol != -1)
        _outputModel->setHorizontalHeaderItem(_timeCol, new QStandardItem(QString::number(_timeCol)));
    _timeCol = index;
    _outputModel->setHorizontalHeaderItem(_timeCol, new QStandardItem("TIME"));
}

void ImportDialog::xChanged(int index)
{
    if(_xCol != -1)
        _outputModel->setHorizontalHeaderItem(_xCol, new QStandardItem(QString::number(_xCol)));
    _xCol = index;
    _outputModel->setHorizontalHeaderItem(_xCol, new QStandardItem("X"));
}

void ImportDialog::yChanged(int index)
{
    if(_yCol != -1)
        _outputModel->setHorizontalHeaderItem(_yCol, new QStandardItem(QString::number(_yCol)));
    _yCol = index;
    _outputModel->setHorizontalHeaderItem(_yCol, new QStandardItem("Y"));
}

void ImportDialog::accept()
{
    // Consistency check

    QList<TraceHeader> res;
    // GraphLoader(filename, _regex, QList<TraceHeader>);
    QDialog::accept();
}

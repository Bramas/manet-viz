#include "importdialog.h"
#include "ui_importdialog.h"
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QListView>
#include <QStandardItemModel>
#include "csvparser.h"

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
    ui->comboBoxEscape->setCurrentIndex(1);
    ui->comboBoxSep->setCurrentIndex(2);

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->comboBoxSep, SIGNAL(currentIndexChanged(int)), this, SLOT(sepChanged(int)));
    connect(ui->comboBoxEscape, SIGNAL(currentIndexChanged(int)), this, SLOT(escapeChanged(int)));
    connect(ui->comboBoxQuote, SIGNAL(currentIndexChanged(int)), this, SLOT(quoteChanged(int)));

    processSampleTrace(10); // TODO: Guess the default variables
    processInputTable();
    processOutputTable();

    //    ui->listViewInput->setModel(model = new QStringListModel(this););
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
    CSVParser parser(_quote, _sep, _escape);
    int i = 0;
//    if (_heading) {
//        i++;
//        file->readLine();
//    }

    foreach(QString line, _sampleTrace) {
        QList<QString> fields;
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



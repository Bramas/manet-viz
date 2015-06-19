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

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->lineEditRegEx, SIGNAL(textEdited(QString)), this, SLOT(regExEdited(QString)));

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
    // Re-initilize the attributes
    _outputModel->clear();
    _headers.clear();

    CSVParser parser(_regEx);

    int idx = 0;
    if (_heading) {
        QStringList fields;
        QString line = _sampleTrace[idx++];
        parser.parseRegEx(line, fields);
        int j = 0;
        foreach (QString field, fields) {
            QStandardItem * item = new QStandardItem(field);
            _headers.append(field);
            _outputModel->setHorizontalHeaderItem(j++,item);
        }
    } else {
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields);
        for(int i = 0; i<fields.count(); ++i)
        {
            _headers.append(QString(tr("Column %0")).arg(i));
        }
    }

    int i = 0;
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
void ImportDialog::regExEdited(QString text) {
    _regEx = text;
    processOutputTable();
}

void ImportDialog::accept()
{
    // Consistency check

    QList<TraceHeader> res;
    // GraphLoader(filename, _regex, QList<TraceHeader>);
    QDialog::accept();
}

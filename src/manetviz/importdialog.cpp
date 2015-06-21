#include "importdialog.h"
#include "ui_importdialog.h"
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QListView>
#include <QStandardItemModel>
#include <QCloseEvent>

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
    ui->lineEditTrace->setText((new QFileInfo(filename))->fileName());
    ui->checkBoxHeading->setChecked(false);

    connect(ui->checkBoxHeading, SIGNAL(stateChanged(int)), this, SLOT(headingChanged()));
    connect(ui->lineEditRegEx, SIGNAL(textEdited(QString)), this, SLOT(regExEdited(QString)));

    ui->lineEditRegEx->setText("(\\d+);([^;]+);POINT\\(([^ ]+) ([^ ]+)\\)");

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
}

void ImportDialog::processOutputTable()
{
    // Re-initilize the attributes
    _outputModel->clear();
    _headers.clear();    

    CSVParser parser(_regEx);

    ComboBoxDelegate * delegate = new ComboBoxDelegate();
    QStringList defaultHeaders;
    defaultHeaders << "Id" << "Time" << "X" << "Y";
    int idx = 0;
    if (_heading) {
        ui->tableViewOutput->horizontalHeader()->setVisible(true);
        QStringList fields;
        QString line = _sampleTrace[idx++];
        parser.parseRegEx(line, fields);
        int j = 0;
        foreach(const QString &field, fields)
        {
            QStandardItem * item = new QStandardItem(field);
            _headers.append(field);
            _outputModel->setHorizontalHeaderItem(j++,item);
            _outputModel->setItem(0,j,item);
        }
    } else {
        ui->tableViewOutput->horizontalHeader()->setVisible(false);
        QStringList fields;
        QString line = _sampleTrace[idx];
        parser.parseRegEx(line, fields);
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
    _heading = this->ui->checkBoxHeading->isChecked();
    processOutputTable();
}
void ImportDialog::regExEdited(QString text) {
    _regEx = text;
    processOutputTable();
}

TraceHeader ImportDialog::convertToTraceHeader(QString header)
{
    if(header == "Id") return IdHeader;
    if(header == "Time") return TimeHeader;
    if(header == "X") return XHeader;
    if(header == "Y") return YHeader;
    if(header == "TimeStart") return TimeStartHeader;
    if(header == "TimeEnd") return TimeEndHeader;
    if(header == "Id2") return Id2Header;
    if(header == "Property") return PropertyHeader;
}

GraphLoader ImportDialog::createGraphLoader()
{
    // Consistency check
    QList<TraceHeader> headers;
    for(int i=0; i<_outputModel->columnCount();++i)
    {
        headers.append(convertToTraceHeader(_outputModel->item(0,i)->data(Qt::EditRole).toString()));
    }

    return GraphLoader(_filename, QRegExp(_regEx), headers);
}

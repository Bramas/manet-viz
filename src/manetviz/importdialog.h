#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

#include "types.h"
#include "graphloader.h"

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QString filename, QWidget *parent = 0);
    ~ImportDialog();
    GraphLoader createGraphLoader();

private:
    Ui::ImportDialog *ui;
    QString _filename = "";
    QString _regEx = "^$";
    bool _heading = false;
    QStandardItemModel * _inputModel;
    QStandardItemModel * _outputModel;
    QStringList _sampleTrace;
    QStringList _headers;

    void processSampleTrace(int);
    void processOutputTable();
    void processInputTable();
    TraceHeader convertToTraceHeader(QString header);

private slots:
    void headingChanged();
    void regExEdited(QString);
};

#endif // IMPORTDIALOG_H

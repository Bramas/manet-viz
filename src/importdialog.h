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
    QString _regEx;
    QStringList _regExps;
    bool _hasHeading = false;
    bool _isMobilityTrace = true;
    bool _isContactTrace = false;
    QMap<int,TraceHeader> _TraceHeaders;
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
    void contactChanged();
    void mobilityChanged();
    void onAccepted();
    void regExEdited(QString);
    void checkConsistency(QStandardItem *);
};

#endif // IMPORTDIALOG_H

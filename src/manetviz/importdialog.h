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
    QString _regEx = "";
    QString _timeFormat = "";
    int _timeCol = -1;
    bool _isLoading;
    QStringList _regExps;
    QStringList _timeFormats;
    bool _hasHeading = false;
    bool _isMobilityTrace = true;
    bool _isContactTrace = false;
    QMap<int,QString> _TraceHeaders;
    QStandardItemModel * _inputModel;
    QStandardItemModel * _outputModel;
    QStringList _sampleTrace;
    QStringList _headers;

    void processSampleTrace(int);
    void processOutputTable();
    void processInputTable();
    bool isTimeFormatValid(QString format);
    bool isValidRegEx(QString regex);
    void checkConsistency();

private slots:
    void headingChanged();
    void contactChanged();
    void mobilityChanged();
    void onAccepted();
    void regExEdited(QString);
    void timeFormatEdited(QString);
    void headerChanged(QStandardItem *);
};

#endif // IMPORTDIALOG_H

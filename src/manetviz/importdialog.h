#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QComboBox>

#include "/usr/local/include/proj_api.h"
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
    QString _projIn = "";
    QString _projOut = "";
    int _timeCol = -1;
    bool _isLoading;
    bool _isTimeFormatValid = false, _isProjInValid = true, _isProjOutValid = true, _isRegExValid = false;
    QStringList _regExps;
    QStringList _timeFormats;
    QStringList _projIns, _projOuts;
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
    bool isValidProj(QString proj);
    void checkConsistency();
    bool toggleBoldFont(QComboBox * combo, bool isValid);

private slots:
    void headingChanged();
    void contactChanged();
    void mobilityChanged();
    void onAccepted();
    void regExEdited(QString);
    void timeFormatEdited(QString);
    void projInEdited(QString);
    void projOutEdited(QString);
    void headerChanged(QStandardItem *);
};

#endif // IMPORTDIALOG_H

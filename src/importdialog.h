#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QString filename, QWidget *parent = 0);
    ~ImportDialog();

public slots:
    void accept();

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
    QString processSepComboBox(int);
    QString processEscapeComboBox(int);
    QString processQuoteComboBox(int);
    QChar guessSep(QString line);

private slots:
    void headingChanged();
    void regExEdited(QString);
};

#endif // IMPORTDIALOG_H

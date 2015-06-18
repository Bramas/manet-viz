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

private:
    Ui::ImportDialog *ui;
    QString _filename;
    QString _sep = ";";
    bool _heading = false;
    QString _escape = "\\";
    QString _quote = "\"";
    QStandardItemModel * _inputModel;
    QStandardItemModel * _outputModel;
    QList<QString> _sampleTrace;

    void processSampleTrace(int);
    void processOutputTable();
    void processInputTable();
    QString processSepComboBox(int);
    QString processEscapeComboBox(int);
    QString processQuoteComboBox(int);

private slots:
    void headingChanged();
    void sepChanged(int);
    void escapeChanged(int);
    void quoteChanged(int);
};

#endif // IMPORTDIALOG_H

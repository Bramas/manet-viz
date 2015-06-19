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
    QString _filename;
    QString _sep = ";";
    bool _heading = false;
    QString _escape = "\\";
    QString _quote = "\"";
    int _idCol   = -1;
    int _timeCol = -1;
    int _xCol    = -1;
    int _yCol    = -1;
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
    void sepChanged(int);
    void escapeChanged(int);
    void quoteChanged(int);
    void idChanged(int);
    void timeChanged(int);
    void xChanged(int);
    void yChanged(int);
};

#endif // IMPORTDIALOG_H

#ifndef GTFSDIALOG_H
#define GTFSDIALOG_H

#include <QDialog>
#include <QComboBox>

#include "types.h"
#include "gtfsloader.h"
#include "/usr/local/include/proj_api.h"

namespace Ui {
class GTFSDialog;
}

class GTFSDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTFSDialog(QString pathname, QWidget *parent = 0);
    ~GTFSDialog();
    GTFSLoader createGTFSLoader();

private:
    Ui::GTFSDialog *ui;
    QString _pathname;
    QString _projIn = "";
    QString _projOut = "";
    QStringList _projIns, _projOuts;
    bool _isProjInValid = true, _isProjOutValid = true;

    bool isValidProj(QString proj);
    void checkConsistency();
    bool toggleBoldFont(QComboBox * combo, bool isValid);

private slots:
    void onAccepted();
    void projInEdited(QString);
    void projOutEdited(QString);

};

#endif // GTFSDIALOG_H

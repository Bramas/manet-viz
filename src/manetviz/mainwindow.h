#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class GraphLoader;
class GTFSLoader;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();
    void openGTFS();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    GraphLoader * _graphLoader;
    GTFSLoader * _gtfsLoader;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class GraphLoader;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    GraphLoader * _graphLoader;
};

#endif // MAINWINDOW_H

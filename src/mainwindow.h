#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class AbstractEvolvingGraph;

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

    AbstractEvolvingGraph * _evolvingGraph;
};

#endif // MAINWINDOW_H

#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QDockWidget>

#include "types.h"

namespace Ui {
class ControlWidget;
}

class Viewer;
class Project;

class ControlWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent, Project *project);
    ~ControlWidget();

    void setViewer(Viewer * viewer);

public slots:
    void setLoadProgress(qreal);

private:
    Ui::ControlWidget *ui;
    Viewer * _viewer;
    Project * _project;
};

#endif // CONTROLWIDGET_H

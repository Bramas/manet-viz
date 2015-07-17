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
    void increaseZoom();
    void decreaseZoom();
    void resetZoom();

private:
    Ui::ControlWidget *ui;
    Viewer * _viewer;
    Project * _project;

    QPointF getWeightedCenter();
};

#endif // CONTROLWIDGET_H

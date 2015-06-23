#include "graphlayer.h"
#include "viewer.h"
#include "ui_graphlayercontrol.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QImage>

class GraphicsNodeItem : public QGraphicsItem
{
public:

    GraphicsNodeItem(QPointF position) : _position(position) { }
    QRectF boundingRect() const
    {
        return QRectF(_position.x()-0.00001, _position.y()-0.00001,0.00002,0.00002);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QTransform t = painter->transform().inverted();
        QPointF point = painter->transform().map(_position);
        QPointF offset(_pen.widthF()/2.0,_pen.widthF()/2.0);
        QRectF r(t.map(point - offset), t.map(point+offset));
        painter->setPen(Qt::NoPen);
        painter->setBrush(_pen.brush());
        painter->drawEllipse(r);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _position;
};

class GraphicsEdgeItem : public QGraphicsItem
{
public:

    GraphicsEdgeItem(QPointF p1, QPointF p2) : _p1(p1), _p2(p2) { }
    QRectF boundingRect() const
    {
        QPointF topLeft(qMin(_p1.x(), _p2.x()), qMin(_p1.y(), _p2.y()));
        QPointF bottomRight(qMax(_p1.x(), _p2.x()), qMax(_p1.y(), _p2.y()));
        return QRectF(topLeft, bottomRight);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QLineF line(_p1, _p2);
        painter->setPen(_pen);
        painter->drawLine(line);
    }
    void setPen(const QPen &p)
    {
        _pen = p;
    }

private:
    QPen _pen;
    QPointF _p1;
    QPointF _p2;
};

GraphLayer::GraphLayer()
{
    _communicationRange = 2;
    _displayContacts = false;
    _displayRange = false;
}

void GraphLayer::paint(IGraph * graph)
{
    //Graph g = _evolvingGraph->footprint(_evolvingGraph->beginTime() + graph);//_parent->time());

    _scene->removeItem(items);
    delete items;
    items = new QGraphicsItemGroup();


    QPen p(QColor(255,0,0));
    p.setWidth(1);
    p.setCosmetic(true);
    if(_displayContacts) {
        foreach(const Node& n1, graph->nodes())
        {
            foreach(const Node& n2, n1.neighbors())
            {
                GraphicsEdgeItem * line = new GraphicsEdgeItem(n1.position(), (n2.position()));
                line->setPen(p);
                items->addToGroup((QGraphicsItem*) line);
            }
        }
    }
    p.setColor(QColor(0,0,0));
    p.setWidthF(4);
    foreach(const Node& n, graph->nodes())
    {
        GraphicsNodeItem * i = new GraphicsNodeItem(n.position());
        i->setPen(p);

        items->addToGroup((QGraphicsItem*)i);

        if(_displayRange) { // TODO: add range
            QPen p2 = p;
            p2.setWidth(0);
            qreal range = _communicationRange / 1000.0;
            QPointF offset(range/2.0, range/2.0);
            QRectF rect(n.position() - offset, n.position() + offset);
            QGraphicsEllipseItem * ellipse = new QGraphicsEllipseItem(rect);
            ellipse->setPen(p2);
            items->addToGroup(ellipse);
        }

    }

    _scene->addItem(items);
}

void GraphLayer::setGraphicsScene(QGraphicsScene *scene)
{
    _scene = scene;
    items = new QGraphicsItemGroup();
    _scene->addItem(items);
}

void GraphLayer::setDisplayRange(bool state)
{
    _displayRange = state;
    emit requestUpdate();
}

void GraphLayer::setDisplayContact(bool state)
{
    _displayContacts = state;
    emit requestUpdate();
}
void GraphLayer::setCommunicationRange(int range)
{
    _communicationRange = range;
    emit requestUpdate();
}

QWidget* GraphLayer::createControlWidget() const
{
    QWidget *control = new QWidget();
    Ui::GraphLayerControl  *ui = new Ui::GraphLayerControl();
    ui->setupUi(control);

    connect(ui->transmissionRangeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCommunicationRange(int)));
    connect(ui->displayRangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayRange(bool)));
    connect(ui->displayContactCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplayContact(bool)));


    return control;
}



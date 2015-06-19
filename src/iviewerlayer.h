#ifndef ABSTRACTLAYER_H
#define ABSTRACTLAYER_H


class QPainter;
class Viewer;


class IViewerLayer
{
public:
    IViewerLayer(Viewer * parent)  {
        _parent = (parent);
    }
    virtual ~IViewerLayer() {}
    virtual void paint(QPainter *) = 0;

protected:
    Viewer * _parent;
};

Q_DECLARE_INTERFACE(IViewerLayer, "org.manet-viz.IViewerLayer")

#endif // ABSTRACTLAYER_H

#ifndef ABSTRACTLAYER_H
#define ABSTRACTLAYER_H


class QPainter;
class Viewer;


class AbstractLayer
{
public:
    AbstractLayer(Viewer * parent)  {
        _parent = (parent);
    }
    virtual void paint(QPainter *) = 0;

protected:
    Viewer * _parent;
};

#endif // ABSTRACTLAYER_H

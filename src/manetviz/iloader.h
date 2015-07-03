#ifndef ILOADER
#define ILOADER

class QObject;
class AbstractEvolvingGraph;

class ILoader {

public:
    ILoader() {}
    virtual ~ILoader() {}

    virtual const AbstractEvolvingGraph *   constEvolvingGraph() const = 0;
    virtual AbstractEvolvingGraph *         evolvingGraph() const = 0;

    virtual bool isLoaded() const = 0;

    virtual QObject * getQObject() = 0;

};

#endif // ILOADER


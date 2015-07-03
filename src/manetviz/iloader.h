#ifndef ILOADER
#define ILOADER

class QObject;
class AbstractEvolvingGraph;
class Project;

class ILoader {

public:
    ILoader() {}
    virtual ~ILoader() {}

    virtual void setProject(Project * project) { _project = project; }

    virtual const AbstractEvolvingGraph *   constEvolvingGraph() const = 0;
    virtual AbstractEvolvingGraph *         evolvingGraph() const = 0;

    virtual bool isLoaded() const = 0;

    virtual QObject * getQObject() = 0;

protected:
    Project * _project;

};

#endif // ILOADER


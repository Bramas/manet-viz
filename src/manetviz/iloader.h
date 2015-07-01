#ifndef ILOADER
#define ILOADER

class AbstractEvolvingGraph;

class ILoader {

public:
    ILoader() {}
    virtual ~ILoader() {}

protected:
    AbstractEvolvingGraph * _evolvingGraph;
};

#endif // ILOADER


#ifndef TYPES_H
#define TYPES_H
#include <QtCore>
#include <climits>

typedef quint64 mvtime;

const mvtime MaxTime = std::numeric_limits<mvtime>::max();

typedef enum TraceHeader {
    IdHeader,
    TimeHeader,
    XHeader,
    YHeader,
    TimeStartHeader,
    TimeEndHeader,
    Id2Header,
    PropertyHeader,
    NoneHeader
} TraceHeader;

#endif // TYPES_H


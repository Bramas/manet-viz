#ifndef TYPES_H
#define TYPES_H
#include <QtCore>

typedef quint64 mvtime;

typedef enum TraceHeader {
    IdHeader,
    TimeHeader,
    XHeader,
    YHeader,
    TimeStartHeader,
    TimeEndHeader,
    Id2Header,
    PropertyHeader
} TraceHeader;

#endif // TYPES_H


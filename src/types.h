#ifndef TYPES_H
#define TYPES_H
#include <QtCore>

typedef quint64 mvtime;

typedef enum TraceHeader {
    Id,
    Time,
    X,
    Y,
    TimeStart,
    TimeEnd,
    Id2,
    Property
} TraceHeader;

#endif // TYPES_H


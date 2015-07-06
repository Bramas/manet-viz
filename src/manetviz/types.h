#ifndef TYPES_H
#define TYPES_H
#include <QtCore>
#include <climits>

typedef quint64 mvtime;

const mvtime MaxTime = std::numeric_limits<mvtime>::max();
const double MaxDist = std::numeric_limits<double>::max();

const QString X = "X";
const QString Y = "Y";
const QString Time = "Time";
const QString Id = "Id";

#endif // TYPES_H

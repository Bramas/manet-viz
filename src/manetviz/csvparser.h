#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QStringList>


class CSVParser
{
public:
    CSVParser();

    int parseString(QString &buffer, QStringList &fields, QString delimChars, QString quoteChars, QString escapeChars);
    int parseRegEx(QString &buffer, QStringList &fields, QString regEx);
    int parseCSV(QString filename, QVector<QMap<QString,QString>> &fields, QString delim, QString quote="\"", QString escape="\\");
};

#endif // CSVPARSER_H

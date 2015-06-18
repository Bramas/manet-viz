#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QList>


class CSVParser
{
public:
    CSVParser(QString mQuoteChar, QString mDelimChars, QString mEscapeChar);
    int parseString(QString &buffer, QList<QString> &fields);
    QList<QString> parseLine();
private:
    QString _filname;
    QString _mDelimChars;
    QString _mQuoteChar;
    QString _mEscapeChar;
};

#endif // CSVPARSER_H

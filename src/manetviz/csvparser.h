#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QStringList>


class CSVParser
{
public:
    CSVParser(QString mQuoteChar, QString mDelimChars, QString mEscapeChar);
    CSVParser(QString regEx);
    int parseString(QString &buffer, QStringList &fields);
    int parseRegEx(QString &buffer, QStringList &fields);
private:
    QString _filname;
    QString _mDelimChars;
    QString _mQuoteChar;
    QString _mEscapeChar;
    QString _regEx;
};

#endif // CSVPARSER_H

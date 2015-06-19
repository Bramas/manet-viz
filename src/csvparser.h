#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QString>
#include <QStringList>


class CSVParser
{
public:
    CSVParser(QString mQuoteChar, QString mDelimChars, QString mEscapeChar);
    int parseString(QString &buffer, QStringList &fields);
private:
    QString _filname;
    QString _mDelimChars;
    QString _mQuoteChar;
    QString _mEscapeChar;
};

#endif // CSVPARSER_H

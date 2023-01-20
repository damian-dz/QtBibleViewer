#ifndef PARSERS_H
#define PARSERS_H

#include "ParsedVerseLocation.h"

class Parsers
{
public:
    static QList<qbv::ParsedVerseLocation> ToVerseLocations(const QString &str);

private:
    static int CountNumbers(const QString &str);
    static bool CanBeBookName(const QString &str, int offset);
    static QList<int> ToNumbers(const QString &str);
    static QList<qbv::ParsedVerseLocation> ToSubLocations(const QString &lastBookName, const QList<int> &numbers);
};

#endif // PARSERS_H

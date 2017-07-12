#ifndef AUXILIARYMETHODS_H
#define AUXILIARYMETHODS_H

#include <QRegExp>
#include <QString>

class AuxiliaryMethods
{
public:
    static bool startsWithNonLatin(const QString &word);
    static QString singleWordQueryString(const QString &word, const QString &columns);
};

#endif // AUXILIARYMETHODS_H

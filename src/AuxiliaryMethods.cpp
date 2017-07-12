#include "hdr/AuxiliaryMethods.h"

bool AuxiliaryMethods::startsWithNonLatin(const QString &word)
{
    QRegExp rgxChar("[a-zA-z0-9<>]");
    return !rgxChar.exactMatch(word.left(1));
}

QString AuxiliaryMethods::singleWordQueryString(const QString &word, const QString &columns)
{
    QString lowercase = word.toLower();
    bool hasSpecial = false;
    hasSpecial = startsWithNonLatin(lowercase);
    if (!hasSpecial)
        return QString("SELECT " + columns + " FROM Bible"
                       " WHERE Scripture LIKE '%" + lowercase + "%'");
    else {
        QString uppercase = lowercase[0].toUpper() +
                lowercase.right(lowercase.length() - 1);
        return QString("SELECT " + columns + " FROM Bible"
                       " WHERE Scripture LIKE '%" + lowercase + "%'" +
                       " OR Scripture LIKE '%" + uppercase + "%'");
    }
}

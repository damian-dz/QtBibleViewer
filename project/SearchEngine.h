#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "SearchOptions.h"

class SearchEngine
{
    typedef bool (&AllAny)(const QString &text, const QList<QRegularExpression> &words);

public:
    static bool containsAllWords(const QString &text, const QList<QRegularExpression> &words);
    static bool containsAnyWord(const QString &text, const QList<QRegularExpression> &words);
    static QRegExp iterateRecords(QSqlQuery &query, const QStringList &words,
                               QRegularExpression::PatternOption sensitivity, bool wholeWords, bool containsAll,
                               QStringList &results, QStringList &refs, const QStringList &bookNames);
    static QRegExp iterateRecords(QSqlQuery &query, const QString &text,
                               QRegularExpression::PatternOption sensitivity, bool wholeWords, bool hasStrong,
                               QStringList &results, QStringList &refs, const QStringList &bookNames);
    static QString multipleWordQueryString(const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj);
    static QStringList search(QString text, SearchOptions options, const QSqlDatabase &module, bool hasStrong, QStringList &refs,
                              const QStringList &bookNames);

};

#endif // SEARCHENGINE_H

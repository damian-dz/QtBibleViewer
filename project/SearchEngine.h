#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "Location.h"
#include "SearchOptions.h"

class SearchEngine
{
    typedef bool (&AllAny)(const QString &text, const QList<QRegularExpression> &words);

public:
    static bool containsAllWords(const QString &text, const QList<QRegularExpression> &words);
    static bool containsAnyWord(const QString &text, const QList<QRegularExpression> &words);
    static QRegularExpression iterateRecords(QSqlQuery &query, const QStringList &words,QRegularExpression::PatternOption sensitivity, bool wholeWords, bool containsAll,
                               QStringList &results, QStringList &refs, const QStringList &bookNames);
    static QRegularExpression iterateRecords(QSqlQuery &query, const QString &text,
                               QRegularExpression::PatternOption sensitivity, bool wholeWords, bool hasStrong,
                               QStringList &results, QStringList &refs, const QStringList &bookNames);

    static void escape(QStringList &words);
    static void multipleWordQuery(QSqlQuery &query, const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj, int book1, int book2);
    static QString multipleWordQueryString(const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj);
    static QRegularExpression search(QString text, SearchOptions options, QSqlDatabase &module, bool hasStrong, QStringList &refs,
                              const QStringList &bookNames, QStringList &results);

    static int FindBestMatchIndex(QString pattern, QStringList values);
    static qbv::Location parseLocationStr(QString raw,
                                          const QStringList& bookNames,
                                          const QStringList& shortBookNames);


private:
    static double CalculateSimilarity(const QString &source, const QString &target, bool caseSensitive = true);
    static int ComputeLevenshteinDistance(const QString &source, const QString &target);


};

#endif // SEARCHENGINE_H

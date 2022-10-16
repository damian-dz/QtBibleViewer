#ifndef DBBIBLE_H
#define DBBIBLE_H

#include "AbstractDb.h"
#include "Formatting.h"
#include "Location.h"
#include "PassageWithNotes.h"
#include "PassageWithRef.h"
#include "SearchOptions.h"

namespace qbv {

class DbBible : public AbstractDb
{
public:
    bool HasOT() const;
    bool HasNT() const;
    bool HasStrong() const;

    QString Info() const;
    QString ShortName() const;

    QStringList Passage(Location loc) const;
    PassageWithNotes PassageWithNotesAndMissingVerses(Location loc) const;

    QRegularExpression Search(const QString &text, SearchOptions options);
    QRegularExpression SearchByPhrase(const QString &phrase, SearchOptions options);
    QRegularExpression SearchByStrong(const QString &number, SearchOptions options);
    QList<qbv::PassageWithLocation> GetLastSearchResults() const;
    QList<qbv::PassageWithLocation> GetLastSearchResults(int pos, int count = -1) const;
    int GetNumLastSearchResults() const;

private:
    QString MultipleWordCommand(const QStringList &words, const QString &conjunction);
    void LimitRange(SearchOptions options, QString &command);
    void RegexStrings(QString &rgxStr, QString &highlightRgxStr);
    void FilterRawResults(const QRegularExpression &rgx, QSqlQuery &query);

    QList<qbv::PassageWithLocation> m_lastSearchResults;
};

}

#endif // DBBIBLE_H

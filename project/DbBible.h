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

    QList<PassageWithLocation> Search(const QString &text, SearchOptions options);
    QList<qbv::PassageWithLocation> SearchByPhrase(const QString &phrase, SearchOptions options);
    QList<qbv::PassageWithLocation> SearchByStrong(const QString &number, SearchOptions options);

    qbv::PassageWithLocation GetRandomPassage(SearchOptions options);
   // QList<qbv::PassageWithLocation> GetLastSearchResults() const;
  //  QList<qbv::PassageWithLocation> GetLastSearchResults(int pos, int count = -1) const;
   // int GetNumLastSearchResults() const;

private:
    QString MultipleWordCommand(const QStringList &words, const QString &conjunction);
    void LimitRange(SearchOptions options, QString &command);
    void RegexStrings(QString &rgxStr, QString &highlightRgxStr);
    QList<PassageWithLocation> GetFilteredResults(const QRegularExpression &rgx, QSqlQuery &query);

   // QList<qbv::PassageWithLocation> m_lastSearchResults;
};

}

#endif // DBBIBLE_H

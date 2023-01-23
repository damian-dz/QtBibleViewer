#ifndef DBBIBLE_H
#define DBBIBLE_H

#include "AbstractDb.h"
#include "Formatting.h"
#include "Location.h"
#include "Parsers.h"
#include "PassageWithNotes.h"
#include "PassageWithRef.h"
#include "SearchOptions.h"
#include "VerseLocation.h"
#include "Typedefs.h"
#include "BookStringMapping.h"
#include "SearchEngine.h"


namespace qbv {

class DbBible : public AbstractDb
{
public:

   // typedef qbv::VerseLocation (*MyFunc)(qbv::ParsedVerseLocation);
   // MyFunc m_pToVerseLocation;
    DbBible(const QList<BookStringMapping> &bookNameMappings, const QList<BookStringMapping> &shortBookNameMappings);


    bool HasOT() const;
    bool HasNT() const;
    bool HasStrong() const;

    QString Info() const;
    QString GetShortName() const;

    QStringList GetScriptures(int verseId, int endVerseId) const;
    QStringList GetScriptures(Location loc) const;
    QStringList GetScripturesWithMissing(Location loc) const;
    QString GetScripture(Location loc) const;
    QString GetScripture(VerseLocation loc) const;

    QList<PassageWithLocation> Search(const QString &text, SearchOptions options);
    QList<qbv::PassageWithLocation> SearchByPhrase(const QString &phrase, SearchOptions options);
    QList<qbv::PassageWithLocation> SearchByStrong(const QString &number, SearchOptions options);
    QList<qbv::PassageWithLocation> SearchByVerses(const QString &verses);



    qbv::PassageWithLocation GetRandomPassage(SearchOptions options);
   // QList<qbv::PassageWithLocation> GetLastSearchResults() const;
  //  QList<qbv::PassageWithLocation> GetLastSearchResults(int pos, int count = -1) const;
   // int GetNumLastSearchResults() const;
    VerseLocation ToVerseLocation(ParsedVerseLocation loc);
private:
   // DatabaseService &m_databaseService;
    const QList<BookStringMapping> &m_bookNameMappings;
    const QList<BookStringMapping> &m_shortBookNameMappings;

    MyFunc m_pToVerseLocation;

    QString MultipleWordCommand(const QStringList &words, SearchOptions options);
    void LimitRange(SearchOptions options, QString &command);
    void RegexStrings(QString &rgxStr, QString &highlightRgxStr);
    QList<PassageWithLocation> GetFilteredResults(const QRegularExpression &rgx, QSqlQuery &query);

   // QList<qbv::PassageWithLocation> m_lastSearchResults;
};

}

#endif // DBBIBLE_H

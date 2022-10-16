#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include "precomp.h"

#include "AppConfig.h"
#include "DbBible.h"
#include "DbVerseData.h"
#include "DbNotes.h"

namespace qbv {

class DatabaseService : public QObject
{
public:
    DatabaseService(const QString &dataDir, AppConfig &config);
    ~DatabaseService();

    void AddDbBible(DbBible *dbBible);
    void SwapDbBibles(int first, int second);
    void RemoveDbBible(int idx);
    void CloseAll();

    void LoadBookNames();
    void LoadDbBibles();

    QString DirBibles() const;

    QStringList BookNames() const;
    QStringList ShortBookNames() const;

    QString BookName(int idx) const;
    QString BookNameForNumber(int number) const;

    QString BibleShortName(int idx) const;
    QStringList BibleShortNames() const;
    bool HasStrong(int idx) const;

    int NumBibles() const;

    int NumChapters(int book) const;
    int NumVerses(int book, int chapter) const;
    int ChapterId(int book, int chapter) const;
    int VerseId(int book, int chapter, int verse) const;

    int ChapterIdForLocation(qbv::Location loc) const;
    qbv::Location LocationForChapterId(int id) const;
    QString PassageIdForLocation(qbv::Location loc);

    void SetActiveIdx(int idx);

    QStringList Passage(int idx, Location loc);
    qbv::PassageWithNotes PassageWithNotesAndMissingVerses(int idx, Location loc);
    QList<PassageWithLocation> Search(int idx, const QString &phrase, SearchOptions options);
   // QList<qbv::PassageWithLocation> GetLastSearchResults(int idx) const;
   // QList<qbv::PassageWithLocation> GetLastSearchResults(int idx, int pos, int count = -1) const;
  //  int GetNumLastSearchResults(int idx) const;

    void CreateUserDir();
    void OpenUserNotesDb();

    void AddToNotes(Location loc);
    QList<qbv::Location> NotesLocations();
    QString Note(qbv::Location loc);
    void SaveNote(const QString &note, qbv::Location loc);

private:
    const QString *m_dataDir;
    AppConfig *m_pConfig;

    const QString m_dirCommon;
    const QString m_dirBibles;
    const QString m_dirUser;

    QList<DbBible *> m_dbBibles;
    DbVerseData m_dbVerseData;
    DbNotes m_dbNotes;

    QStringList m_bookNames;
    QStringList m_shortBookNames;

    int m_activeIdx = 0;

    QStringList BibleFilePaths();

    void PopulateBookNames();
    void PopulateShortBookNames();
};

}
#endif // DATABASESERVICE_H

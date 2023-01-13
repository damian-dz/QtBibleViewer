#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include "AppConfig.h"
#include "DbBible.h"
#include "DbVerseData.h"
#include "DbNotes.h"
#include "DbXRef.h"

namespace qbv {

class DatabaseService : public QObject
{
    Q_OBJECT
public:
    DatabaseService(const QString &dataDir, AppConfig &config);
    ~DatabaseService();

    void AddDbBible(qbv::DbBible *dbBible);
    void SwapDbBibles(int first, int second);
    void RemoveDbBible(int idx);
    void CloseAll();

    void LoadBookNames();
    void LoadDbBibles();

    QString DirBibles() const;

    QStringList BookNames();
    QStringList ShortBookNames();

    QString BookName(int idx) const;
    QString BookNameForNumber(int number) const;

    QString BibleShortName(int idx) const;
    QStringList BibleShortNames() const;
    bool HasStrong(int idx) const;

    int NumBibles() const;

    int NumChapters(int book) const;
    int GetNumVerses(int book, int chapter) const;
    int ChapterId(int book, int chapter) const;
    int VerseId(int book, int chapter, int verse) const;

    int ChapterIdForLocation(qbv::Location loc) const;
    qbv::Location LocationForChapterId(int id) const;
    QString PassageIdForLocation(qbv::Location loc);
    int IndexForBibleShortName(const QString &name);

    void SetActiveIdx(int idx);

    QStringList GetScriptures(int idx, int verseId, int endVerseId) const;
    QStringList GetScriptures(int idx, qbv::Location loc);
    QStringList GetScripturesWithMissing(int idx, qbv::Location loc);
    QStringList GetScriptures(qbv::Location loc) const;
    QList<qbv::PassageWithLocation> Search(int idx, const QString &phrase, SearchOptions options);
    qbv::PassageWithLocation GetRandomPassage(int idx, SearchOptions options);

    QList<Location> GetXRefLocations(Location loc) const;

    void CreateUserDir();
    void OpenUserNotesDb();
    void OpenXRefDb();

    void AddToNotes(qbv::Location loc);
    QList<qbv::Location> NotesLocations();
    QString Note(qbv::Location loc);
    void SaveNote(const QString &note, qbv::Location loc);


    void PopulateBookNames();
    void PopulateShortBookNames();

private:
    const QString *m_dataDir;
    AppConfig *m_pConfig;

    const QString m_dirCommon;
    const QString m_dirBibles;
    const QString m_dirUser;

    QList<qbv::DbBible *> m_dbBibles;
    qbv::DbVerseData m_dbVerseData;
    qbv::DbNotes m_dbNotes;
    qbv::DbXRef m_dbXRef;

    QStringList m_bookNames;
    QStringList m_shortBookNames;

    int m_activeIdx = 0;

    QStringList BibleFilePaths();

};

}
#endif // DATABASESERVICE_H

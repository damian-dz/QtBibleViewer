#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include "precomp.h"

#include "AppConfig.h"
#include "DbBible.h"
#include "DbVerseData.h"

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

    QStringList BookNames() const;
    QStringList ShortBookNames() const;

    QString BookName(int idx) const;
    QString BookNameForNumber(int number) const;

    QString BibleShortName(int idx) const;

    int NumBibles() const;

    int NumChapters(int book) const;
    int NumVerses(int book, int chapter) const;
    int ChapterId(int book, int chapter) const;
    int VerseId(int book, int chapter, int verse) const;

    int ChapterIdForLocation(qbv::Location loc) const;
    qbv::Location LocationForChapterId(int id) const;

    void SetActiveIdx(int idx);

    qbv::PassageWithNotes PassageWithNotesAndMissingVerses(int idx, Location loc);

private:
    const QString *m_pDataDir;
    AppConfig *m_pConfig;

    const QString m_dirCommon;
    const QString m_dirBibles;

    QList<DbBible *> m_dbBibles;
    DbVerseData m_dbVerseData;

    QStringList m_bookNames;
    QStringList m_shortBookNames;

    int m_activeIdx = 0;

    QStringList BibleFilePaths();

    void PopulateBookNames();
    void PopulateShortBookNames();
};

}
#endif // DATABASESERVICE_H

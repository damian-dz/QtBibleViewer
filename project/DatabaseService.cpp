#include "DatabaseService.h"

#include <stdexcept>

namespace qbv {

DatabaseService::DatabaseService(const QString &dataDir, AppConfig &config) :
    m_dataDir(&dataDir),
    m_pConfig(&config),
    m_dirCommon(dataDir + "Common"),
    m_dirBibles(dataDir + "Bibles"),
    m_dirUser(dataDir + "User")
{
    m_dbVerseData.Open(m_dirCommon + "/verse_data.qbv");

    LoadBookNames();
    LoadDbBibles();
}

DatabaseService::~DatabaseService()
{
    CloseAll();
    for (DbBible *dbBible : m_dbBibles) {
        delete dbBible;
    }
}

void DatabaseService::AddDbBible(DbBible *dbBible)
{
    m_dbBibles.append(dbBible);
}

void DatabaseService::SwapDbBibles(int first, int second)
{
    m_dbBibles.swapItemsAt(first, second);
}

void DatabaseService::RemoveDbBible(int idx)
{
    m_dbBibles[idx]->Close();
    delete m_dbBibles[idx];
    m_dbBibles.removeAt(idx);
}

void DatabaseService::CloseAll()
{
    for (DbBible *dbBible : m_dbBibles) {
        dbBible->Close();
    }
    m_dbVerseData.Close();
    m_dbNotes.Close();
}

void DatabaseService::LoadBookNames()
{
    PopulateBookNames();
    PopulateShortBookNames();
}

void DatabaseService::LoadDbBibles()
{
    if (!QDir(m_dirBibles).exists()) {
        throw std::invalid_argument("The Bible directory does not exist.");
    }
    QStringList paths = BibleFilePaths();
    for (const QString &path : paths) {
        if (!m_pConfig->module_data.paths.contains(path) &&
                !m_pConfig->module_data.removed_paths.contains(path) &&
                QFileInfo(path).exists()) {
            m_pConfig->module_data.paths << path;
        }
    }
    if (m_pConfig->module_data.paths.isEmpty()) {
        throw std::invalid_argument("No Bible modules found.");
    } else {
        for (int i = 0; i < m_pConfig->module_data.paths.count(); ++i) {
            if (!QFileInfo(m_pConfig->module_data.paths[i]).exists()) {
                m_pConfig->module_data.paths.removeAt(i--);
            }
        }
    }

    for (const QString &filePath: m_pConfig->module_data.paths) {
        qDebug() << filePath;
        DbBible *dbBible = new DbBible;
        dbBible->Open(filePath);
        AddDbBible(dbBible);
    }
}

QString DatabaseService::DirBibles() const
{
    return m_dirBibles;
}

QStringList DatabaseService::BibleFilePaths()
{
    QDir dir(m_dirBibles);
    QStringList filters("*.qbv");
    dir.setNameFilters(filters);
    QFileInfoList moduleList = dir.entryInfoList();
    QStringList results;
    for (const QFileInfo &file : moduleList) {
        results << file.absoluteFilePath();
    }
    return results;
}

QStringList DatabaseService::BookNames() const
{
    return m_bookNames;
}

QStringList DatabaseService::ShortBookNames() const
{
    return m_shortBookNames;
}

QString DatabaseService::BookName(int idx) const
{
    return m_bookNames[idx];
}

QString DatabaseService::BookNameForNumber(int number) const
{
    return m_bookNames[number - 1];
}

QString DatabaseService::BibleShortName(int idx) const
{
    return m_dbBibles[idx]->GetShortName();
}

QStringList DatabaseService::BibleShortNames() const
{
    QStringList results;
    for (const qbv::DbBible *item : m_dbBibles) {
        results.append(item->GetShortName());
    }
    return results;
}

bool DatabaseService::HasStrong(int idx) const
{
    return m_dbBibles[idx]->HasStrong();
}

int DatabaseService::NumBibles() const
{
    return m_dbBibles.count();
}

int DatabaseService::NumChapters(int book) const
{
    return m_dbVerseData.NumChapters(book);
}

int DatabaseService::NumVerses(int book, int chapter) const
{
    return m_dbVerseData.NumVerses(book, chapter);
}

int DatabaseService::ChapterId(int book, int chapter) const
{
    return  m_dbVerseData.ChapterId(book, chapter);
}

int DatabaseService::VerseId(int book, int chapter, int verse) const
{
    return  m_dbVerseData.VerseId(book, chapter, verse);
}

int DatabaseService::ChapterIdForLocation(Location loc) const
{
    return m_dbVerseData.ChapterIdForLocation(loc);
}

Location DatabaseService::LocationForChapterId(int id) const
{
    return m_dbVerseData.LocationForChapterId(id);
}

QString DatabaseService::PassageIdForLocation(Location loc)
{
    QString result = BookNameForNumber(loc.book) + " " + QString::number(loc.chapter) +
            ":" + QString::number(loc.verse1);
    if (loc.verse1 != loc.verse2) {
        result += "-" % QString::number(loc.verse2);
    }
    return result;
}

int DatabaseService::IndexForBibleShortName(const QString &name)
{
    int result = -1;
    for (int i = 0; i < m_dbBibles.count(); ++i) {
        if (m_dbBibles[i]->GetShortName() == name) {
            result = i;
            break;
        }
    }
    return result;
}

void DatabaseService::SetActiveIdx(int idx)
{
    m_activeIdx = idx;
}

QStringList DatabaseService::GetScriptures(int idx, Location loc)
{
    return m_dbBibles[idx]->GetScriptures(loc);
}

QStringList DatabaseService::GetScripturesWithMissing(int idx, Location loc)
{
    return m_dbBibles[idx]->GetScripturesWithMissing(loc);
}

QStringList DatabaseService::GetScriptures(Location loc) const
{
    QStringList results;
    for (const qbv::DbBible *item : m_dbBibles) {
        QString scripture = item->GetScripture(loc);
        results.append(scripture);
    }
    return results;
}

QList<PassageWithLocation> DatabaseService::Search(int idx, const QString &phrase, SearchOptions options)
{
    return m_dbBibles[idx]->Search(phrase, options);
}

PassageWithLocation DatabaseService::GetRandomPassage(int idx, SearchOptions options)
{
    return m_dbBibles[idx]->GetRandomPassage(options);
}

void DatabaseService::CreateUserDir()
{
    if (!QDir(m_dirUser).exists()) {
        QDir(*m_dataDir).mkdir("User");
    }
}

void DatabaseService::OpenUserNotesDb()
{
    m_dbNotes.Open(m_dirUser + "/notes.qbv");
    m_dbNotes.Init();
}

void DatabaseService::AddToNotes(Location loc)
{
    m_dbNotes.Add(loc);
}

QList<Location> DatabaseService::NotesLocations()
{
    return m_dbNotes.Locations();
}

QString DatabaseService::Note(Location loc)
{
    return m_dbNotes.Note(loc);
}

void DatabaseService::SaveNote(const QString &note, Location loc)
{
    m_dbNotes.Save(note, loc);
}

void DatabaseService::PopulateBookNames()
{
    if (!m_bookNames.isEmpty()) {
        m_bookNames.clear();
    }
    m_bookNames << tr("Genesis")
                << tr("Exodus")
                << tr("Leviticus")
                << tr("Numbers")
                << tr("Deuteronomy")
                << tr("Joshua")
                << tr("Judges")
                << tr("Ruth")
                << tr("1 Samuel")
                << tr("2 Samuel")
                << tr("1 Kings")
                << tr("2 Kings")
                << tr("1 Chronicles")
                << tr("2 Chronicles")
                << tr("Ezra")
                << tr("Nehemiah")
                << tr("Esther")
                << tr("Job")
                << tr("Psalms")
                << tr("Proverbs")
                << tr("Ecclesiastes")
                << tr("Song of Solomon")
                << tr("Isaiah")
                << tr("Jeremiah")
                << tr("Lamentations")
                << tr("Ezekiel")
                << tr("Daniel")
                << tr("Hosea")
                << tr("Joel")
                << tr("Amos")
                << tr("Obadiah")
                << tr("Jonah")
                << tr("Micah")
                << tr("Nahum")
                << tr("Habakkuk")
                << tr("Zephaniah")
                << tr("Haggai")
                << tr("Zechariah")
                << tr("Malachi")
                << tr("Matthew")
                << tr("Mark")
                << tr("Luke")
                << tr("John")
                << tr("Acts")
                << tr("Romans")
                << tr("1 Corinthians")
                << tr("2 Corinthians")
                << tr("Galatians")
                << tr("Ephesians")
                << tr("Philippians")
                << tr("Colossians")
                << tr("1 Thessalonians")
                << tr("2 Thessalonians")
                << tr("1 Timothy")
                << tr("2 Timothy")
                << tr("Titus")
                << tr("Philemon")
                << tr("Hebrews")
                << tr("James")
                << tr("1 Peter")
                << tr("2 Peter")
                << tr("1 John")
                << tr("2 John")
                << tr("3 John")
                << tr("Jude")
                << tr("Revelation");
}

void DatabaseService::PopulateShortBookNames()
{
    if (!m_shortBookNames.isEmpty()) {
        m_shortBookNames.clear();
    }
    m_shortBookNames << tr("Gen")
                     << tr("Exo")
                     << tr("Lev")
                     << tr("Num")
                     << tr("Deu")
                     << tr("Jos")
                     << tr("Jdg")
                     << tr("Rut")
                     << tr("1Sa")
                     << tr("2Sa")
                     << tr("1Ki")
                     << tr("2Ki")
                     << tr("1Ch")
                     << tr("2Ch")
                     << tr("Ezr")
                     << tr("Neh")
                     << tr("Est")
                     << tr("Job")
                     << tr("Psa")
                     << tr("Pro")
                     << tr("Ecc")
                     << tr("Sol")
                     << tr("Isa")
                     << tr("Jer")
                     << tr("Lam")
                     << tr("Eze")
                     << tr("Dan")
                     << tr("Hos")
                     << tr("Joe")
                     << tr("Amo")
                     << tr("Oba")
                     << tr("Jon")
                     << tr("Mic")
                     << tr("Nah")
                     << tr("Hab")
                     << tr("Zep")
                     << tr("Hag")
                     << tr("Zec")
                     << tr("Mal")
                     << tr("Mat")
                     << tr("Mar")
                     << tr("Luk")
                     << tr("Joh")
                     << tr("Act")
                     << tr("Rom")
                     << tr("1Co")
                     << tr("2Co")
                     << tr("Gal")
                     << tr("Eph")
                     << tr("Phi")
                     << tr("Col")
                     << tr("1Th")
                     << tr("2Th")
                     << tr("1Ti")
                     << tr("2Ti")
                     << tr("Tit")
                     << tr("Phm")
                     << tr("Heb")
                     << tr("Jam")
                     << tr("1Pe")
                     << tr("2Pe")
                     << tr("1Jo")
                     << tr("2Jo")
                     << tr("3Jo")
                     << tr("Jud")
                     << tr("Rev");
}

}

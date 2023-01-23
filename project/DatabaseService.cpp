#include "DatabaseService.h"

#include <stdexcept>

namespace qbv {

DatabaseService::DatabaseService(const QString &dataDir, AppConfig &config) :
    m_dataDir(&dataDir),
    m_pConfig(&config),
    m_dirCommon(dataDir + "/Common"),
    m_dirBibles(dataDir + "/Bibles"),
    m_dirUser(dataDir + "/User")
{
    m_dbVerseData.Open(m_dirCommon + "/verse_data.qbv");

    LoadBookNames();
    LoadDbBibles();
    InitBookKeys();
    LoadBookNameMappings();
    OpenXRefDb();
}

DatabaseService::~DatabaseService()
{
    CloseAll();
    for (qbv::DbBible *dbBible : m_dbBibles) {
        delete dbBible;
    }
}

void DatabaseService::AddDbBible(qbv::DbBible *dbBible)
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
    for (qbv::DbBible *dbBible : m_dbBibles) {
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

void DatabaseService::LoadBookNameMappings()
{
    PopulateBookNameMappings(m_bookNameMappings, m_bookNames, m_bookNameKeys, "book_names_alt");
    PopulateBookNameMappings(m_shortBookNameMappings, m_shortBookNames, m_shortBookNameKeys, "short_book_names_alt");
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
                QFileInfo::exists(path)) {
            m_pConfig->module_data.paths << path;
        }
    }
    if (m_pConfig->module_data.paths.isEmpty()) {
        throw std::invalid_argument("No Bible modules found.");
    } else {
        for (int i = 0; i < m_pConfig->module_data.paths.count(); ++i) {
            if (!QFileInfo::exists(m_pConfig->module_data.paths[i])) {
                m_pConfig->module_data.paths.removeAt(i--);
            }
        }
    }

    for (const QString &filePath: m_pConfig->module_data.paths) {
        qDebug() << filePath;
        qbv::DbBible *dbBible = new qbv::DbBible(m_bookNameMappings, m_shortBookNameMappings);
        //dbBible->SetToVerseLocation(&DatabaseService::ToVerseLocation);
       // dbBible->SetDatabaseService(this);
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


QStringList DatabaseService::BookNames()
{
    return m_bookNames;
}

QStringList DatabaseService::ShortBookNames()
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

int DatabaseService::GetNumVerses(int book, int chapter) const
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

int DatabaseService::ChapterIdForLocation(qbv::Location loc) const
{
    return m_dbVerseData.ChapterIdForLocation(loc);
}

qbv::Location DatabaseService::LocationForChapterId(int id) const
{
    return m_dbVerseData.LocationForChapterId(id);
}

QString DatabaseService::PassageIdForLocation(qbv::Location loc)
{
    QString result = BookNameForNumber(loc.book) + " " + QString::number(loc.chapter) +
        ":" + QString::number(loc.verse);
    if (loc.verse != loc.endVerse) {
        result += "-" % QString::number(loc.endVerse);
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

QStringList DatabaseService::GetScriptures(int idx, int verseId, int endVerseId) const
{
    return m_dbBibles[idx]->GetScriptures(verseId, endVerseId);
}

QStringList DatabaseService::GetScriptures(int idx, qbv::Location loc)
{
    return m_dbBibles[idx]->GetScriptures(loc);
}

QStringList DatabaseService::GetScripturesWithMissing(int idx, qbv::Location loc)
{
    return m_dbBibles[idx]->GetScripturesWithMissing(loc);
}

QStringList DatabaseService::GetScriptures(qbv::Location loc) const
{
    QStringList results;
    for (const qbv::DbBible *item : m_dbBibles) {
        QString scripture = item->GetScripture(loc);
        results.append(scripture);
    }
    return results;
}

QList<qbv::PassageWithLocation> DatabaseService::Search(int idx, const QString &phrase, SearchOptions options)
{
    return m_dbBibles[idx]->Search(phrase, options);
}

qbv::PassageWithLocation DatabaseService::GetRandomPassage(int idx, SearchOptions options)
{
    return m_dbBibles[idx]->GetRandomPassage(options);
}

QList<Location> DatabaseService::GetXRefLocations(Location loc) const
{
    return m_dbXRef.GetLocations(loc);
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

void DatabaseService::OpenXRefDb()
{
    if (!m_dbXRef.IsOpen())
        m_dbXRef.Open(m_dirCommon + "/xref.qbv");
}

void DatabaseService::AddToNotes(qbv::Location loc)
{
    m_dbNotes.Add(loc);
}

QList<qbv::Location> DatabaseService::NotesLocations()
{
    return m_dbNotes.Locations();
}

QString DatabaseService::Note(qbv::Location loc)
{
    return m_dbNotes.Note(loc);
}

void DatabaseService::SaveNote(const QString &note, qbv::Location loc)
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


void DatabaseService::PopulateBookNameMappings(QList<BookStringMapping> &mappings, const QStringList &names,
    const QStringList &nameKeys, const QString &fileName)
{
    if (!mappings.isEmpty()) {
        mappings.clear();
    }

    QJsonObject json = LoadJsonObject(fileName);

    int length = names.length();
    for (int i = 0; i < length; ++i) {
        mappings.append({ names[i], i + 1 });
        QJsonArray altNames = json[nameKeys[i]].toArray();
        for (const QJsonValue &altName : altNames) {
            mappings.append({ altName.toString(), i + 1 });
        }
    }
}



void DatabaseService::InitBookKeys()
{
    m_bookNameKeys = {
        "Genesis",
        "Exodus",
        "Leviticus",
        "Numbers",
        "Deuteronomy",
        "Joshua",
        "Judges",
        "Ruth",
        "1Samuel",
        "2Samuel",
        "1Kings",
        "2Kings",
        "1Chronicles",
        "2Chronicles",
        "Ezra",
        "Nehemiah",
        "Esther",
        "Job",
        "Psalms",
        "Proverbs",
        "Ecclesiastes",
        "SongOfSolomon",
        "Isaiah",
        "Jeremiah",
        "Lamentations",
        "Ezekiel",
        "Daniel",
        "Hosea",
        "Joel",
        "Amos",
        "Obadiah",
        "Jonah",
        "Micah",
        "Nahum",
        "Habakkuk",
        "Zephaniah",
        "Haggai",
        "Zechariah",
        "Malachi",
        "Matthew",
        "Mark",
        "Luke",
        "John",
        "Acts",
        "Romans",
        "1Corinthians",
        "2Corinthians",
        "Galatians",
        "Ephesians",
        "Philippians",
        "Colossians",
        "1Thessalonians",
        "2Thessalonians",
        "1Timothy",
        "2Timothy",
        "Titus",
        "Philemon",
        "Hebrews",
        "James",
        "1Peter",
        "2Peter",
        "1John",
        "2John",
        "3John",
        "Jude",
        "Revelation"
    };

    m_shortBookNameKeys = {
        "Gn",
        "Ex",
        "Lv",
        "Nm",
        "Dt",
        "Jo",
        "Jgs",
        "Ru",
        "1Sm",
        "2Sm",
        "1Kgs",
        "2Kgs",
        "1Chr",
        "2Chr",
        "Ezr",
        "Neh",
        "Est",
        "Jb",
        "Ps",
        "Prv",
        "Eccl",
        "Sg",
        "Is",
        "Jer",
        "Lam",
        "Ez",
        "Dn",
        "Hos",
        "Jl",
        "Am",
        "Ob",
        "Jon",
        "Mi",
        "Na",
        "Hb",
        "Zep",
        "Hg",
        "Zec",
        "Mal",
        "Mt",
        "Mk",
        "Lk",
        "Jn",
        "Act",
        "Rom",
        "1Cor",
        "2Cor",
        "Gal",
        "Eph",
        "Phil",
        "Col",
        "1Thes",
        "2Thes",
        "1Tm",
        "2Tm",
        "Ti",
        "Phlm",
        "Heb",
        "Jas",
        "1Pt",
        "2Pt",
        "1Jn",
        "2Jn",
        "3Jn",
        "Jud",
        "Rv"
    };
}

QJsonObject DatabaseService::LoadJsonObject(const QString &fileName)
{
    QString lang = m_pConfig->general.language.toLower();
    QString filePath = QString(":/str/str_res/%1_%2.json").arg(lang, fileName);

    QFile jsonFile(filePath);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject json = doc.object();
    return json;
}

}

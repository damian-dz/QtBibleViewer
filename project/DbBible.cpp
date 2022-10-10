#include "DbBible.h"

namespace qbv {

bool DbBible::HasOT() const
{
    bool result = false;
    QSqlQuery query(m_db);
    if (query.exec("SELECT Scripture FROM Bible WHERE Book=40 AND Chapter=1 AND Verse =1") && query.next()) {
        result = true;
    }
    return result;
}

bool DbBible::HasNT() const
{
    bool result = false;
    QSqlQuery query(m_db);
    if (query.exec("SELECT Scripture FROM Bible WHERE Book=1 AND Chapter=1 AND Verse=1") && query.next()) {
        result = true;
    }
    return result;
}

bool DbBible::HasStrong() const
{
    bool result = false;
    QSqlQuery query(m_db);
    int book = HasOT() ? 1 : 40;
    if (query.exec(QString("SELECT Scripture FROM Bible WHERE Book=%1 AND Chapter=1 AND Verse=1").arg(book))
            && query.next()) {
        result = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
    }
    return result;
}

QString DbBible::Info() const
{
    return "File Path: " + FilePath() + "\n"
           "Short Name: " + ShortName() + "\n"
           "Has Old Testament: " + (HasOT() ? "Yes" : "No" ) + "\n"
           "Has New Testament: " + (HasNT() ? "Yes" : "No" ) + "\n"
           "Has Strong: " + (HasStrong() ? "Yes" : "No" ) + "\n";
}

QString DbBible::ShortName() const
{
    QString result = "";
    QSqlQuery query(m_db);
    if (query.exec("SELECT Abbreviation FROM Info") && query.next()) {
        result = query.record().value(0).toString();
    }
    return result;
}

/*!
 * \brief Searches the database for verses from the specified location and removes the associated notes.
 * \param loc — location of the passage
 * \return a list of verses without notes
 */
QStringList DbBible::Passage(Location loc) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT Scripture From Bible WHERE Book=? AND Chapter=? AND Verse>=? AND Verse<=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse1, QSql::Out);
    query.addBindValue(loc.verse2, QSql::Out);
    QStringList results;
    if (query.exec()) {
        while (query.next()) {
            QString scripture = query.record().value(0).toString();
            Formatting::FormatTextAndRemoveNotes(scripture);
            results << scripture;
        }
    }
    return results;
}

PassageWithNotes DbBible::PassageWithNotesAndMissingVerses(Location loc) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT Verse, Scripture From Bible WHERE Book=? AND Chapter=? AND Verse>=? AND Verse<=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse1, QSql::Out);
    query.addBindValue(loc.verse2, QSql::Out);
    int verseNumber = loc.verse1;
    int finalVerse = loc.verse2;
    int verse = loc.verse1;
    PassageWithNotes results;
    if (query.exec()) {
        while (query.next() && verseNumber <= finalVerse) {
            QSqlRecord record = query.record();
            verse = record.value(0).toInt();
            results.verses << verse;
            if (verseNumber != verse) {
                for (int i = 0; i < verse - verseNumber; ++i) {
                    results.formatted << QString();
                    results.unformatted << QString();
                    ++verseNumber;
                }
            }
            QString scripture = record.value(1).toString();
            results.unformatted << scripture;
            Formatting::FormatTextAndAddNotes(scripture, results.notes);
            results.formatted << scripture;
            ++verseNumber;
        }
    }
    if (verse < finalVerse) {
        for (int i = 0; i < finalVerse - verse; ++i) {
            results.formatted << QString();
            results.unformatted << QString();
        }
    }
    return results;
}

}


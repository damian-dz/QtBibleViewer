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
    bool hasStrong = HasStrong();
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
            Formatting::FormatTextAndAddNotes(scripture, results.notes, hasStrong);
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

void DbBible::Search(const QString &phrase, SearchOptions options)
{
    QStringList words = phrase.split(" ");
    QString conj = options.searchMode == SearchMode::anyWords ? "OR" : "AND";

    QString command = MultipleWordCommand(words, conj);
    if (options.bookFrom > 1) {
        command += " AND Book>=" + QString::number(options.bookFrom);
    }
    if (options.bookTo < 66) {
        command += " AND Book<=" + QString::number(options.bookTo);
    }

    QString rgxStr = "";
    QString highlightRgxStr = "";

    if (options.searchMode == SearchMode::exactPhrase) {
        rgxStr = options.wholeWordsOnly ? QString("\\b%1\\b").arg(phrase) : phrase;
        highlightRgxStr = options.wholeWordsOnly ?
            QString("(<i>)?\\b%1\\b(</i.*?>)?").arg(words.join("(</i>)? (<i>)?")) :
            QString("(<i>)?%1(</i>)?").arg(words.join("(</i>)? (<i>)?"));
    } else if (options.searchMode == SearchMode::allWords) {
        rgxStr = options.wholeWordsOnly ?
            QString("(?=.*\\b%1\\b)").arg(words.join("\\b)(?=.*\\b")) :
            QString("(?=.*%1)").arg(words.join(")(?=.*"));
        highlightRgxStr = options.wholeWordsOnly ?
            QString("\\b%1\\b").arg(words.join("\\b|\\b")) :
            words.join("|");
    } else if (options.searchMode == SearchMode::anyWords) {
        rgxStr = options.wholeWordsOnly ?
            QString("\\b%1\\b)").arg(words.join("\\b|\\b")) :
            words.join("|");
        highlightRgxStr = rgxStr;
    } else {
        m_lastResults.clear();
    }
}

QString DbBible::MultipleWordCommand(const QStringList &words, const QString &conjunction)
{
    QString command;
    if (words.count() > 0) {
        command = "SELECT * FROM Bible WHERE Scripture LIKE '%" + words[0] + "%'";
        for (int i = 1; i < words.count(); i++) {
            command += " " + conjunction + " Scripture LIKE '%" + words[i] + "%'";
        }
    }
    return command;
}

void DbBible::RegexStrings(QString &rgxStr, QString &highlightRgxStr)
{

}

}


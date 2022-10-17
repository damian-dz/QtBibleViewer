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
           "Short Name: " + GetShortName() + "\n"
           "Has Old Testament: " + (HasOT() ? "Yes" : "No" ) + "\n"
           "Has New Testament: " + (HasNT() ? "Yes" : "No" ) + "\n"
           "Has Strong: " + (HasStrong() ? "Yes" : "No" ) + "\n";
}

QString DbBible::GetShortName() const
{
    QString result = "";
    QSqlQuery query(m_db);
    if (query.exec("SELECT Abbreviation FROM Info") && query.next()) {
        result = query.record().value(0).toString();
    }
    return result;
}

/*!
 * \brief Searches the database for a scripture from the specified location.
 * \param loc — location of the passage
 * \return a string containing the scripture or a null string if it is not found
 */
QString DbBible::GetScripture(Location loc) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT Scripture From Bible WHERE Book=? AND Chapter=? AND Verse=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse1, QSql::Out);
    QString result;
    if (query.exec() && query.next()) {
        result = query.record().value(0).toString();
    }
    return result;
}

/*!
 * \brief Searches the database for scriptures from the specified location.
 * \param loc — location of the scriptures
 * \return a list containing the scriptures or an empty list if none is found
 */
QStringList DbBible::GetScriptures(Location loc) const
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
            results.append(scripture);
        }
    }
    return results;
}

/*!
 * \brief Searches the database for scriptures from the specified location.
 *        If a verse is missing, an empty string is inserted.
 * \param loc — location of the scriptures
 * \return a list of scriptures
 */
QStringList DbBible::GetScripturesWithMissing(Location loc) const
{
    QStringList results;
    QSqlQuery query(m_db);
    query.prepare("SELECT Verse, Scripture From Bible WHERE Book=? AND Chapter=? AND Verse>=? AND Verse<=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse1, QSql::Out);
    query.addBindValue(loc.verse2, QSql::Out);
    int verseNumber = loc.verse1;
    int finalVerse = loc.verse2;
    int verse = loc.verse1;
    if (query.exec()) {
        while (query.next() && verseNumber <= finalVerse) {
            QSqlRecord record = query.record();
            verse = record.value(0).toInt();
            if (verseNumber != verse) {
                for (int i = 0; i < verse - verseNumber; ++i) {
                    results.append(QString());
                    ++verseNumber;
                }
            }
            QString scripture = record.value(1).toString();
            results.append(scripture);
            ++verseNumber;
        }
    }
    if (verse < finalVerse) {
        for (int i = 0; i < finalVerse - verse; ++i) {
            results.append(QString());
        }
    }

    return results;
}



 QList<qbv::PassageWithLocation> DbBible::Search(const QString &text, SearchOptions options)
{
    if (options.searchMode == SearchMode::byStrong) {
        return SearchByStrong(text, options);
    } else {
        return SearchByPhrase(text, options);
    }
}

 QList<qbv::PassageWithLocation> DbBible::SearchByStrong(const QString &number, SearchOptions options)
{
    QList<qbv::PassageWithLocation> results;
    QString pattern = QString("<W%1>").arg(number.toUpper());
    QString command = "SELECT Book, Chapter, Verse, Scripture FROM Bible WHERE Scripture LIKE '%" + pattern + "%'";
    LimitRange(options, command);
    QSqlQuery query(m_db);
    if (query.exec(command)) {
        while (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verse = record.value(2).toInt();
            QString scripture = record.value(3).toString();
            qbv::Location loc(book, chapter, verse, verse);
            results.append({ scripture, loc });
        }
    }
    return results;
 }

PassageWithLocation DbBible::GetRandomPassage(SearchOptions options)
{
    PassageWithLocation result;
    QSqlQuery query(m_db);
    query.prepare("SELECT Book, Chapter, Verse, Scripture FROM Bible WHERE Book>=? AND Book<=? ORDER BY RANDOM() LIMIT 1");
    query.addBindValue(options.bookFrom, QSql::Out);
    query.addBindValue(options.bookTo, QSql::Out);
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        int book = record.value(0).toInt();
        int chapter = record.value(1).toInt();
        int verse = record.value(2).toInt();
        QString scripture = record.value(3).toString();
        qbv::Location loc(book, chapter, verse, verse);
        result.passage = scripture;
        result.location = loc;
    }
    return result;
}

QList<qbv::PassageWithLocation> DbBible::SearchByPhrase(const QString &phrase, SearchOptions options)
{
    QStringList words = phrase.split(" ");
    QString conj = options.searchMode == SearchMode::anyWords ? "OR" : "AND";

    QString command = MultipleWordCommand(words, conj);
    LimitRange(options, command);

    QString rgxStr = "";

    if (options.searchMode == SearchMode::exactPhrase) {
        rgxStr = options.wholeWordsOnly ? QString("\\b%1\\b").arg(phrase) : phrase;
    } else if (options.searchMode == SearchMode::allWords) {
        rgxStr = options.wholeWordsOnly ?
            QString("(?=.*\\b%1\\b)").arg(words.join("\\b)(?=.*\\b")) :
            QString("(?=.*%1)").arg(words.join(")(?=.*"));
    } else if (options.searchMode == SearchMode::anyWords) {
        rgxStr = options.wholeWordsOnly ?
            QString("\\b%1\\b)").arg(words.join("\\b|\\b")) : words.join("|");
    }

    //m_lastSearchResults.clear();

    QRegularExpression::PatternOption sensitivity = options.caseSensitive ?
                QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
    QRegularExpression rgx(rgxStr, sensitivity | QRegularExpression::UseUnicodePropertiesOption);

    QSqlQuery query(m_db);
    QList<qbv::PassageWithLocation> results;

    if (query.exec(command)) {
        results = GetFilteredResults(rgx, query);
    }

    return results;
}

//QList<PassageWithLocation> DbBible::GetLastSearchResults() const
//{
//    return m_lastSearchResults;
//}

//QList<PassageWithLocation> DbBible::GetLastSearchResults(int pos, int count) const
//{
//    return  m_lastSearchResults.mid(pos, count);
//}

//int DbBible::GetNumLastSearchResults() const
//{
//    return m_lastSearchResults.count();
//}

QList<qbv::PassageWithLocation> DbBible::GetFilteredResults(const QRegularExpression &rgx, QSqlQuery &query)
{
    QList<qbv::PassageWithLocation> results;
    QString strong = HasStrong() ? "|<X?W[HG][0-9]{1,4}x?>" : "";
    QRegularExpression patterns("{TN}.*{tn}|{H}.*{h}|{[A-Za-z]{2}}|<.{1,2}>" + strong);
    while (query.next()) {
        QSqlRecord record = query.record();
        QString inputText = record.value(3).toString();
        QString outputText(inputText);
        inputText.remove(patterns);
        if (inputText.contains(rgx)) {
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verse = record.value(2).toInt();
            qbv::Location loc(book, chapter, verse, verse);
            results.append({ outputText, loc });
        }
    }
    return results;
}

QString DbBible::MultipleWordCommand(const QStringList &words, const QString &conjunction)
{
    QString command;
    if (words.count() > 0) {
        command = "SELECT Book, Chapter, Verse, Scripture FROM Bible WHERE Scripture LIKE '%" + words[0] + "%'";
        for (int i = 1; i < words.count(); i++) {
            command += " " + conjunction + " Scripture LIKE '%" + words[i] + "%'";
        }
    }
    return command;
}

void DbBible::LimitRange(SearchOptions options, QString &command)
{
    if (options.bookFrom > 1) {
        command += " AND Book>=" + QString::number(options.bookFrom);
    }
    if (options.bookTo < 66) {
        command += " AND Book<=" + QString::number(options.bookTo);
    }
}

void DbBible::RegexStrings(QString &rgxStr, QString &highlightRgxStr)
{

}

}


#include "DbVerseData.h"

namespace qbv {

int DbVerseData::ChapterId(int book, int chapter) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT Id FROM ChapterMap WHERE Book=? AND Chapter=?");
    query.addBindValue(book, QSql::Out);
    query.addBindValue(chapter, QSql::Out);
    int result = -1;
    if (query.exec() && query.next()) {
        result = query.record().value(0).toInt();
    }
    return result;
}

int DbVerseData::VerseId(int book, int chapter, int verse) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT Id FROM ChapterMap WHERE Book=? AND Chapter=? AND Verse=?");
    query.addBindValue(book, QSql::Out);
    query.addBindValue(chapter, QSql::Out);
    query.addBindValue(verse, QSql::Out);
    int result = -1;
    if (query.exec() && query.next()) {
        result = query.record().value(0).toInt();
    }
    return result;
}

int DbVerseData::NumChapters(int book) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT MAX(Chapter) FROM ChapterMap WHERE Book=?");
    query.addBindValue(book, QSql::Out);
    int result = -1;
    if (query.exec() && query.next()) {
        result = query.record().value(0).toInt();
    }
    return result;
}

int DbVerseData::NumVerses(int book, int chapter) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT NumVerses FROM ChapterMap WHERE Book=? AND Chapter=?");
    query.addBindValue(book, QSql::Out);
    query.addBindValue(chapter, QSql::Out);
    int result = -1;
    if (query.exec() && query.next()) {
        result = query.record().value(0).toInt();
    }
    return result;
}

}

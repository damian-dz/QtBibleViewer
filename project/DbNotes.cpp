#include "DbNotes.h"

namespace qbv {

void DbNotes::Init()
{
    if (m_db.open()) {
        QSqlQuery query(m_db);
        query.exec("CREATE TABLE IF NOT EXISTS Notes "
                   "(Book INT, Chapter INT, Verse1 INT, Verse2 INT, Note TEXT, Created TEXT, LastModified TEXT)");
        query.exec("CREATE UNIQUE INDEX IF NOT EXISTS note_key ON Notes "
                   "(Book ASC, Chapter ASC, Verse1 ASC, Verse2 ASC)");
    }
}

QList<Location> DbNotes::Locations()
{
    QList<Location> results;
    QSqlQuery query(m_db);
    query.exec("SELECT Book, Chapter, Verse1, Verse2 FROM Notes");
    while (query.next()) {
        QSqlRecord record = query.record();
        int book = record.value(0).toInt();
        int chapter = record.value(1).toInt();
        int verse1 = record.value(2).toInt();
        int verse2 = record.value(3).toInt();
        results.append(qbv::Location { book, chapter, verse1, verse2 } );

    }
    return results;
}

void DbNotes::Add(Location loc)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Notes (Book, Chapter, Verse1, Verse2, Created) VALUES (?, ?, ?, ?, DATETIME('now'))");
    query.addBindValue(loc.book);
    query.addBindValue(loc.chapter);
    query.addBindValue(loc.verse1);
    query.addBindValue(loc.verse2);
    query.exec();
}

void DbNotes::Save(const QString &note, Location loc)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Notes SET Note=?, LastModified=DATETIME('now') "
                  "WHERE Book=? AND Chapter=? AND Verse1=? AND Verse2=?");
    query.addBindValue(note);
    query.addBindValue(loc.book);
    query.addBindValue(loc.chapter);
    query.addBindValue(loc.verse1);
    query.addBindValue(loc.verse2);
    query.exec();
}

QString DbNotes::Note(Location loc)
{
    QString result;
    QSqlQuery query(m_db);
    query.prepare("SELECT Note FROM Notes WHERE Book=? AND Chapter=? AND Verse1=? AND Verse2=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse1, QSql::Out);
    query.addBindValue(loc.verse2, QSql::Out);
    if (query.exec() && query.next()) {
        result = query.record().value(0).toString();
    }
    return result;
}

}

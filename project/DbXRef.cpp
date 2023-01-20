#include "DbXRef.h"

namespace qbv {

void DbXRef::Init()
{

}

QList<Location> DbXRef::GetLocations(Location loc) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT XRefs From CrossReferences WHERE Book=? AND Chapter=? AND Verse=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse, QSql::Out);

    QString refs;
    if (query.exec() && query.next())
        refs = query.record().value(0).toString();

    QList<Location> locations;

    if (refs.isNull() || refs.isEmpty())
        return locations;
    QStringList refsSplit = refs.split(',');
    for (const QString &refSplit : refsSplit) {
        if (refSplit.contains('-')) {
            QStringList startEnd = refSplit.split('-');
            QStringList start = startEnd[0].split('.');
            int book = start[0].toInt();
            int chapter = start[1].toInt();
            int verse = start[2].toInt();
            QStringList end = startEnd[1].split('.');
            int endBook = end[0].toInt();
            int endChapter = end[1].toInt();
            int endVerse = end[2].toInt();
            locations.append(Location(book, endBook, chapter, endChapter, verse, endVerse));
        } else {
            QStringList start = refSplit.split('.');
            int book = start[0].toInt();
            int chapter = start[1].toInt();
            int verse = start[2].toInt();
            locations.append(Location(book, chapter, verse));
        }
    }

    return locations;
}

}

#ifndef DBBIBLE_H
#define DBBIBLE_H

#include "AbstractDb.h"
#include "Formatting.h"
#include "Location.h"
#include "PassageWithNotes.h"
#include "SearchOptions.h"

namespace qbv {

class DbBible : public AbstractDb
{
public:
    bool HasOT() const;
    bool HasNT() const;
    bool HasStrong() const;

    QString Info() const;
    QString ShortName() const;

    QStringList Passage(Location loc) const;
    PassageWithNotes PassageWithNotesAndMissingVerses(Location loc) const;

    void Search(const QString &phrase, SearchOptions options);

private:
    QString MultipleWordCommand(const QStringList &words, const QString &conjunction);
    void RegexStrings(QString &rgxStr, QString &highlightRgxStr);

    QStringList m_lastResults;
};

}

#endif // DBBIBLE_H

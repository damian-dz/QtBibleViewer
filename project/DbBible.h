#ifndef DBBIBLE_H
#define DBBIBLE_H

#include "AbstractDb.h"
#include "Formatting.h"
#include "Location.h"
#include "PassageWithNotes.h"

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
};

}

#endif // DBBIBLE_H

#ifndef DBNOTES_H
#define DBNOTES_H

#include "AbstractDb.h"
#include "Location.h"

namespace qbv {

class DbNotes : public AbstractDb
{
public:
    void Init();

    QList<Location> Locations();
    void Add(Location loc);
    void Save(const QString &note, Location loc);
    QString Note(Location loc);
};

}

#endif // DBNOTES_H

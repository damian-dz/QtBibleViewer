#ifndef DBXREF_H
#define DBXREF_H

#include "AbstractDb.h"
#include "Location.h"

namespace qbv {

class DbXRef : public AbstractDb
{
public:
    void Init();

    QList<Location> GetLocations(Location loc) const;
};

}

#endif // DBXREF_H

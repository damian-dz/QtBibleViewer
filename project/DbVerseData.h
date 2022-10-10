#ifndef DBVERSEDATA_H
#define DBVERSEDATA_H

#include "AbstractDb.h"

namespace qbv {

class DbVerseData : public AbstractDb
{
public:
    int ChapterId(int book, int chapter) const;
    int VerseId(int book, int chapter, int verse) const;

    int NumChapters(int book) const;
    int NumVerses(int book, int chapter) const;
};

}

#endif // DBVERSEDATA_H

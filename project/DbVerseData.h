#ifndef DBVERSEDATA_H
#define DBVERSEDATA_H

#include "AbstractDb.h"
#include "Location.h"

namespace qbv {

class DbVerseData : public AbstractDb
{
public:
    int ChapterId(int book, int chapter) const;
    int VerseId(int book, int chapter, int verse) const;

    int NumChapters(int book) const;
    int NumVerses(int book, int chapter) const;

    int ChapterIdForLocation(qbv::Location loc) const;
    qbv::Location LocationForChapterId(int id) const;
};

}

#endif // DBVERSEDATA_H

#ifndef TABBEDLOCATION_H
#define TABBEDLOCATION_H

#include "Location.h"

namespace qbv {

/*!
 * \brief Provides a structure for storing verse locations along with the appropriate tab.
 */
struct TabbedLocation : public Location
{
    int tab;

    TabbedLocation() :
        tab(-1)
    {

    }

    TabbedLocation(int tab, int book, int chapter, int verseFrom, int verseTo) :
        Location(book, chapter, verseFrom, verseTo),
        tab(tab)
    {

    }

    TabbedLocation(const TabbedLocation &other) :
        Location(other.book, other.chapter, other.verse1, other.verse2),
        tab(other.tab)
    {

    }

    bool IsLocationEqual(const TabbedLocation &other)
    {
        return (book == other.book &&
                chapter == other.chapter &&
                verse1 == other.verse1 &&
                verse2 == other.verse2);
    }

    bool IsLocationValid()
    {
        return (book > 0 && chapter > 0 && verse1 > 0 && verse2 > 0);
    }

    bool IsSameAs(const TabbedLocation &other)
    {
        return (tab == other.tab &&
                book == other.book &&
                chapter == other.chapter &&
                verse1 == other.verse1 &&
                verse2 == other.verse2);
    }

    TabbedLocation operator =(const TabbedLocation &other)
    {
        tab = other.tab;
        book = other.book;
        chapter = other.chapter;
        verse1 = other.verse1;
        verse2 = other.verse2;
        return *this;
    }

    bool operator ==(const TabbedLocation &other)
    {
        return this->IsSameAs(other);
    }

    bool operator !=(const TabbedLocation &other)
    {
        return !this->IsSameAs(other);
    }
};
}

#endif // TABBEDLOCATION_H

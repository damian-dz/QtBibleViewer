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
        Location(other.book, other.chapter, other.verse, other.endVerse),
        tab(other.tab)
    {

    }

    bool IsLocationEqual(const TabbedLocation &other)
    {
        return (book == other.book &&
                chapter == other.chapter &&
                verse == other.verse &&
                endVerse == other.endVerse);
    }

    bool IsLocationValid()
    {
        return (book > 0 && chapter > 0 && verse > 0 && endVerse > 0);
    }

    bool IsSameAs(const TabbedLocation &other)
    {
        return (tab == other.tab &&
                book == other.book &&
                chapter == other.chapter &&
                verse == other.verse &&
                endVerse == other.endVerse);
    }

    TabbedLocation operator =(const TabbedLocation &other)
    {
        tab = other.tab;
        book = other.book;
        chapter = other.chapter;
        verse = other.verse;
        endVerse = other.endVerse;
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

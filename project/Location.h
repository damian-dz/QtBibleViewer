#ifndef LOCATION_H
#define LOCATION_H

#include "precomp.h"

namespace qbv {

struct Location
{
    int tab;
    int book;
    int chapter;
    int verseFrom;
    int verseTo;

    Location() :
        tab(-1),
        book(-1),
        chapter(-1),
        verseFrom(-1),
        verseTo(-1)
    {

    }

    Location(int tab, int book, int chapter, int verseFrom, int verseTo) :
        tab(tab),
        book(book),
        chapter(chapter),
        verseFrom(verseFrom),
        verseTo(verseTo)
    {

    }

    Location(const Location &other) :
        tab(other.tab),
        book(other.book),
        chapter(other.chapter),
        verseFrom(other.verseFrom),
        verseTo(other.verseTo)
    {

    }

    bool isPassageEqual(const Location &other)
    {
        return (book == other.book &&
                chapter == other.chapter &&
                verseFrom == other.verseFrom &&
                verseTo == other.verseTo);
    }

    bool isPassageValid()
    {
        return (book > 0 && chapter > 0 && verseFrom > 0 && verseTo > 0);
    }

    bool isSameAs(const Location &other)
    {
        return (tab == other.tab &&
                book == other.book &&
                chapter == other.chapter &&
                verseFrom == other.verseFrom &&
                verseTo == other.verseTo);
    }

    Location operator =(const Location &other)
    {
        tab = other.tab;
        book = other.book;
        chapter = other.chapter;
        verseFrom = other.verseFrom;
        verseTo = other.verseTo;
        return *this;
    }

    bool operator ==(const Location &other)
    {
        return this->isSameAs(other);
    }

    bool operator !=(const Location &other)
    {
        return !this->isSameAs(other);
    }
};
}

#endif // LOCATION_H

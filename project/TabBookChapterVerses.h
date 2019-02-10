#ifndef TABBOOKCHAPTERVERSES_H
#define TABBOOKCHAPTERVERSES_H

#include "precomp.h"

struct TabBookChapterVerses
{
    int tab;
    int book;
    int chapter;
    int verseFrom;
    int verseTo;

    TabBookChapterVerses() :
        tab(-1),
        book(-1),
        chapter(-1),
        verseFrom(-1),
        verseTo(-1)
    {

    }

    TabBookChapterVerses(int tab, int book, int chapter, int verseFrom, int verseTo) :
        tab(tab),
        book(book),
        chapter(chapter),
        verseFrom(verseFrom),
        verseTo(verseTo)
    {

    }

    TabBookChapterVerses(const TabBookChapterVerses &tbcvv) :
        tab(tbcvv.tab),
        book(tbcvv.book),
        chapter(tbcvv.chapter),
        verseFrom(tbcvv.verseFrom),
        verseTo(tbcvv.verseTo)
    {

    }

    bool isPassageEqual(const TabBookChapterVerses &tbcvv)
    {
        return (book == tbcvv.book &&
                chapter == tbcvv.chapter &&
                verseFrom == tbcvv.verseFrom &&
                verseTo == tbcvv.verseTo);
    }

    bool isPassageValid()
    {
        return (book > 0 && chapter > 0 && verseFrom > 0 && verseTo > 0);
    }

    bool isSameAs(const TabBookChapterVerses &tbcvv)
    {
        return (tab == tbcvv.tab &&
                book == tbcvv.book &&
                chapter == tbcvv.chapter &&
                verseFrom == tbcvv.verseFrom &&
                verseTo == tbcvv.verseTo);
    }

    TabBookChapterVerses operator =(const TabBookChapterVerses &tbcvv)
    {
        tab = tbcvv.tab;
        book = tbcvv.book;
        chapter = tbcvv.chapter;
        verseFrom = tbcvv.verseFrom;
        verseTo = tbcvv.verseTo;
        return *this;
    }

    bool operator ==(const TabBookChapterVerses &tbcvv)
    {
        return this->isSameAs(tbcvv);
    }

    bool operator !=(const TabBookChapterVerses &tbcvv)
    {
        return !this->isSameAs(tbcvv);
    }
};

#endif // TABBOOKCHAPTERVERSES_H

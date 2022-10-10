#ifndef LOCATION_H
#define LOCATION_H

namespace qbv {

/*!
 * \brief Provides a structure for storing verse locations.
 */
struct Location
{
    int book;
    int chapter;
    int verse1;
    int verse2;

    Location() :
        book(-1),
        chapter(-1),
        verse1(-1),
        verse2(-1)
    {

    }

    Location(int book, int chapter, int verseFrom, int verseTo) :
        book(book),
        chapter(chapter),
        verse1(verseFrom),
        verse2(verseTo)
    {

    }

    Location(const Location &other) :
        book(other.book),
        chapter(other.chapter),
        verse1(other.verse1),
        verse2(other.verse2)
    {

    }

    QString BookAsStr() const
    {
        return QString::number(book);
    }

    QString ChapterAsStr() const
    {
        return QString::number(chapter);
    }

    QString VerseFromAsStr() const
    {
        return QString::number(verse1);
    }

    QString VerseToAsStr() const
    {
        return QString::number(verse2);
    }

    bool IsValid()
    {
        return (book > 0 && chapter > 0 && verse1 > 0 && verse2 > 0);
    }

    bool IsValidChapter() const
    {
        return (book > 0 && chapter > 0);
    }

    bool IsSameAs(const Location &other)
    {
        return (book == other.book &&
                chapter == other.chapter &&
                verse1 == other.verse1 &&
                verse2 == other.verse2);
    }

    QString ToQString() const
    {
        return QString::number(book) + " " + QString::number(chapter) + ":" +
               QString::number(verse1) + "-" + QString::number(verse2);
    }

    Location operator =(const Location &other)
    {
        book = other.book;
        chapter = other.chapter;
        verse1 = other.verse1;
        verse2 = other.verse2;
        return *this;
    }

    bool operator ==(const Location &other)
    {
        return this->IsSameAs(other);
    }

    bool operator !=(const Location &other)
    {
        return !this->IsSameAs(other);
    }
};
}

#endif // LOCATION_H

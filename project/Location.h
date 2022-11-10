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

    Location(int book, int chapter, int verse) :
        book(book),
        chapter(chapter),
        verse1(verse),
        verse2(verse)
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

    bool IsSameAs(const Location &other) const
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

    bool operator ==(const Location &other) const
    {
        return this->IsSameAs(other);
    }

    bool operator !=(const Location &other) const
    {
        return !this->IsSameAs(other);
    }

    bool operator<(const Location &other) const
    {
        bool result = false;
        if (!this->IsSameAs(other)) {
            if (other.book == book && other.chapter == chapter && other.verse1 == verse1) {
                result = verse2 < other.verse2;
            } else if (other.book == book && other.chapter == chapter) {
                result = verse1 < other.verse1;
            } else if (other.book == book) {
                result = chapter < other.chapter;
            } else {
                result = book < other.book;
            }
        }
        return result;
    }

    bool operator>(const Location &other) const
    {
        bool result = false;
        if (!this->IsSameAs(other)) {
           result =  !(*this < other);
        }
        return result;
    }
};

struct PassageWithLocation
{
    QString passage;
    Location location;
};


}

#endif // LOCATION_H

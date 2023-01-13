#ifndef LOCATION_H
#define LOCATION_H

namespace qbv {

/*!
 * \brief Provides a structure for storing verse locations.
 */
struct Location
{
    int book;
    int endBook;
    int chapter;
    int endChapter;
    int verse;
    int endVerse;

    Location() :
        book(-1),
        endBook(-1),
        chapter(-1),
        endChapter(-1),
        verse(-1),
        endVerse(-1)
    {

    }

    Location(int book, int endBook, int chapter, int endChapter, int verse, int endVerse) :
        book(book),
        endBook(endBook),
        chapter(chapter),
        endChapter(endChapter),
        verse(verse),
        endVerse(endVerse)
    {

    }

    Location(int book, int chapter, int verse, int endVerse) :
        book(book),
        endBook(book),
        chapter(chapter),
        endChapter(chapter),
        verse(verse),
        endVerse(endVerse)
    {

    }

    Location(int book, int chapter, int verse) :
        book(book),
        endBook(book),
        chapter(chapter),
        endChapter(chapter),
        verse(verse),
        endVerse(verse)
    {

    }

    Location(const Location &other) :
        book(other.book),
        endBook(other.endBook),
        chapter(other.chapter),
        endChapter(other.endChapter),
        verse(other.verse),
        endVerse(other.endVerse)
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

    QString VerseAsStr() const
    {
        return QString::number(verse);
    }

    QString EndVerseAsStr() const
    {
        return QString::number(endVerse);
    }

    bool IsValid()
    {
        return (book > 0 && chapter > 0 && verse > 0 && endVerse > 0);
    }

    bool IsChapterValid() const
    {
        return (book > 0 && chapter > 0);
    }

    bool IsSingleVerse() const
    {
        return book == endBook && chapter == endChapter && verse == endVerse;
    }

    bool IsSameAs(const Location &other) const
    {
        return (book == other.book &&
                endBook == other.endBook &&
                chapter == other.chapter &&
                endChapter == other.endChapter &&
                verse == other.verse &&
                endVerse == other.endVerse);
    }

    QString ToQString() const
    {
        return QString::number(book) + " " + QString::number(chapter) + ":" +
               QString::number(verse) + "-" + QString::number(endVerse);
    }

    Location operator =(const Location &other)
    {
        book = other.book;
        endBook = other.endBook;
        chapter = other.chapter;
        endVerse = other.endVerse;
        verse = other.verse;
        endVerse = other.endVerse;
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
            if (other.book == book && other.chapter == chapter && other.verse == verse) {
                result = endVerse < other.endVerse;
            } else if (other.book == book && other.chapter == chapter) {
                result = verse < other.verse;
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

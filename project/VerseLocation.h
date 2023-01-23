#ifndef VERSELOCATION_H
#define VERSELOCATION_H

namespace qbv {

struct VerseLocation
{
    int book;
    int chapter;
    int verse;

    QString ToQString() const
    {
        return QString::number(book) + " " + QString::number(chapter) + ":" + QString::number(verse);
    }
};

}


#endif // VERSELOCATION_H

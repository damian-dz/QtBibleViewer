#ifndef PARSEDVERSELOCATION_H
#define PARSEDVERSELOCATION_H


namespace qbv {

struct ParsedVerseLocation
{
    QString bookName;
    int chapter;
    int verse;


    QString ToQString() const
    {
        return bookName + " " + QString::number(chapter) + ":" + QString::number(verse);
    }
};

}

#endif // PARSEDVERSELOCATION_H

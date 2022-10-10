#ifndef SCRIPTUREWITHNOTES_H
#define SCRIPTUREWITHNOTES_H

namespace qbv {

struct PassageWithNotes
{
    QList<int> verses;
    QStringList formatted;
    QStringList unformatted;
    QStringList notes;

    int Count() const
    {
        return formatted.count();
    }

    bool IsNullOrEmpty(int idx) const
    {
        return formatted[idx].isNull() || formatted[idx].isEmpty();
    }
};

}

#endif // SCRIPTUREWITHNOTES_H

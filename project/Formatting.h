#ifndef FORMATTING_H
#define FORMATTING_H


class Formatting
{
   // static const inline QRegularExpression m_noteRgx = QRegularExpression("<RF>[^<]*<Rf>");
    static const inline QRegularExpression m_noteRgx = QRegularExpression("<tn>[^<]*</tn>");
    static const inline QRegularExpression m_strongRgx = QRegularExpression("<W[HG]\\d{1,4}>");
public:
    static void formatScripture(QString &text, QStringList &notes, bool hasStrong);
    static void populateNotes(QString &text, QStringList &notes);
};

#endif // FORMATTING_H

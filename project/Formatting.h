#ifndef FORMATTING_H
#define FORMATTING_H


class Formatting
{
    static const inline QRegularExpression m_noteRgx = QRegularExpression("<RF>[^<]*<Rf>");
    static const inline QRegularExpression m_strongRgx = QRegularExpression("<W[HG]\\d{1,4}>");
public:
    static void formatScripture(QString &text, QStringList &notes, bool hasStrong);
};

#endif // FORMATTING_H

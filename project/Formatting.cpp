#include "Formatting.h"

/*!
 * \brief Formats the input text by replacing some of the tags and adding note anchors.
 * \param text the input text
 * \param notes a list of translator notes
 * \param hasStrong specifies if the translation includes Strong's numbers
 */
void Formatting::formatScripture(QString &text, QStringList &notes, bool hasStrong)
{
    text.replace(QStringLiteral("<CM>"), QStringLiteral("<br>"));
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    QRegularExpressionMatchIterator iter = m_noteRgx.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            notes.append(original);
            text.replace(original, QStringLiteral("<a href='c:") % QString::number(notes.count()) %
                         QStringLiteral("' style='text-decoration:none'><b>*</b></a> "));
        }
    }
    if (hasStrong) {
        QRegularExpressionMatchIterator iter = m_strongRgx.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
}

void Formatting::populateNotes(QString &text, QStringList &notes)
{
    QRegularExpressionMatchIterator iter = m_noteRgx.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            notes.append(original);
            text.replace(original, QStringLiteral("<a href='c:") % QString::number(notes.count()) %
                         QStringLiteral("' style='text-decoration:none'><b>*</b></a> "));
        }
    }
}

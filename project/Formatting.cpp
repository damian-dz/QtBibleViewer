#include "Formatting.h"


/*!
 * \brief Extracts the inner text of a heading.
 * \param text the input text
 * \return the heading's inner text
 */
QString Formatting::ExtractHeading(QString &text)
{
    QString heading = m_headingRgx.match(text).captured(1);
    text.remove(m_headingRgx);
    return  heading;
}

/*!
 * \brief Formats the input text by replacing some of the tags and adding note anchors.
 * \param text the input text
 * \param notes a list of translator notes
 * \param hasStrong specifies if the translation includes Strong's numbers
 */
void Formatting::FormatScripture(QString &text, QStringList &notes, bool hasStrong)
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

void Formatting::FormatTextAndAddNotes(QString &text, QStringList &notes, bool hasStrong)
{
    text.replace("{JW}", QStringLiteral("<span style='color:%1'>").arg(m_jwColor.name()));
    text.replace("{OT}", QStringLiteral("<span style='font-weight:bold'>"));
    text.replace(m_endTagRgx, QStringLiteral("</span>"));
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

void Formatting::FormatTextAndRemoveNotes(QString &text)
{
    text.replace("{JW}", QStringLiteral("<span style='color:%1'>").arg(m_jwColor.name()));
    text.replace("{OT}", QStringLiteral("<span style='font-weight:bold'>"));
    text.replace(m_endTagRgx, QStringLiteral("</span>"));
    text.remove(m_noteRgx);
    text.remove(m_headingRgx);
}

void Formatting::FormatAsTeX(QString &text)
{
    text.remove(m_noteRgx);
    text.remove(m_tagRgx);
    text.replace("<i>", "\\textit{").replace("</i>", "}");
}

void Formatting::LeaveIntact(QString &text)
{
    Q_UNUSED(text);
}

void Formatting::RemoveNotes(QString &text)
{
    text.remove(m_noteRgx);
}

void Formatting::RemoveTagsAndNotes(QString &text)
{
    text.remove(m_noteRgx);
    text.remove(m_tagRgx);
    text.remove("<i>").remove("</i>");
}

void Formatting::formatResult(QString &text)
{
    text.replace("{JW}", QStringLiteral("<span style='color:%1'>").arg(m_jwColor.name()));
    text.replace("{OT}", "<span style='font-weight:bold'>");
    text.replace(m_endTagRgx, "</span>");
}

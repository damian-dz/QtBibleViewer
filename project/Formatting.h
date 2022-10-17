#ifndef FORMATTING_H
#define FORMATTING_H

#include "SearchOptions.h"

class Formatting
{
   // static const inline QRegularExpression m_noteRgx = QRegularExpression("<RF>[^<]*<Rf>");
   // static const inline QRegularExpression m_noteRgx = QRegularExpression("<tn>[^<]*</tn>");
   // static const inline QRegularExpression m_noteRgx = QRegularExpression("{TN}[^<]*{tn}");
    static const inline QRegularExpression m_headingRgx = QRegularExpression("{H}(.*?){h}");
    static const inline QRegularExpression m_noteRgx = QRegularExpression("{TN}.*?{tn}");
    static const inline QRegularExpression m_strongRgx = QRegularExpression("<X?W([HG][0-9]{1,4})x?>");
    static const inline QColor m_jwColor = QColor(190, 23, 24);
    static const inline QRegularExpression m_endTagRgx = QRegularExpression("{jw}|{ot}");
    static const inline QRegularExpression m_tagRgx = QRegularExpression("{..}");
public:
    static QRegularExpression GetHighlightRegex(const QString &phrase, SearchOptions options);
    static QString ExtractHeading(QString &text);
    static void FormatScripture(QString &text, QStringList &notes, bool hasStrong);
    static void FormatScriptureAndAddNotes(QString &text, QStringList &notes, bool hasStrong = false);
    static void FormatScriptureAndRemoveNotes(QString &text, bool hasStrong = false);
    static void FormatStrongAsHtml(QString &text);
    static void FormatStrongAsPlainText(QString &text);
    static void FormatAsTeX(QString &text);
    static void LeaveIntact(QString &text);
    static void RemoveNotes(QString &text);
    static void RemoveTagsAndNotes(QString &text);
    static void formatResult(QString &text);
};

#endif // FORMATTING_H

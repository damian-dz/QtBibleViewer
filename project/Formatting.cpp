#include "Formatting.h"

void Formatting::formatPassage(QString &text)
{
//    text.replace(QStringLiteral("<CM>"), QStringLiteral("<br>"));
//    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
//            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
//    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
//            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
//    QRegularExpressionMatchIterator iter = m_noteRgx.globalMatch(text);
//    while (iter.hasNext()) {
//        QRegularExpressionMatch match = iter.next();
//        if (match.hasMatch()) {
//            QString original = match.captured(0);
//            m_notes.append(original);
//            text.replace(original, QStringLiteral("<a href='c:") % QString::number(m_notes.count()) %
//                         QStringLiteral("' style='text-decoration:none'><b>*</b></a> "));
//        }
//    }
//    if (m_hasStrong) {
//        QRegularExpressionMatchIterator iter = m_strongRgx.globalMatch(text);
//        while (iter.hasNext()) {
//            QRegularExpressionMatch match = iter.next();
//            if (match.hasMatch()) {
//                QString original = match.captured(0);
//                QString modified = original.mid(2, original.size() - 3);
//                text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
//            }
//        }
//    }
}

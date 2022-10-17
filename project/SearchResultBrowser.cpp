#include "SearchResultBrowser.h"
#include "Formatting.h"

SearchResultBrowser::SearchResultBrowser(QWidget *parent) :
    QTextBrowser(parent),
    m_includeResultNumber(true)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);

    QObject::connect(this, QOverload<>::of(&QTextEdit::cursorPositionChanged),
                     [=] () { OnCursorPositionChanged(); });
    QObject::connect(this, QOverload<const QUrl &>::of(&QTextBrowser::anchorClicked),
                     [=] (const QUrl &link) { OnAnchorClicked(link); } );
}

SearchResultBrowser::~SearchResultBrowser()
{

}

void SearchResultBrowser::SetResult(const QString &result, const QString &ref)
{
    SetResults(QStringList{result}, QStringList{ref}, 0, 1);
}

bool SearchResultBrowser::IsBetween(int number, int first, int second) const
{
    return number >= first && number <= second;
}

//inline bool IsContainedInSelections(const QTextEdit::ExtraSelection &sel,
//                                    const QList<QTextEdit::ExtraSelection> &extraSelections)
//{
//    bool res = false;
//    for (int i = 0; i < extraSelections.count(); ++i) {
//        if (IsBetween(sel.cursor.position(),
//                      extraSelections[i].cursor.position() -
//                      extraSelections[i].cursor.selectedText().length(),
//                      extraSelections[i].cursor.position())) {
//            res = true;
//            break;
//        }
//    }
//    return res;
//}

bool SearchResultBrowser::IsContainedInPassages(const QTextEdit::ExtraSelection &sel) const
{
    bool res = false;
    for (QPair<int, int> offsets : m_passageOffsets) {
        if (IsBetween(sel.cursor.position(), offsets.first, offsets.second)) {
            res = true;
            break;
        }
    }
    return res;
}

void SearchResultBrowser::HighlightKeywords(const QRegularExpression &regex)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    QTextDocument::FindFlag flag = regex.patternOptions() == QRegularExpression::NoPatternOption ?
                QTextDocument::FindCaseSensitively : QTextDocument::FindFlag(0);
    while (QTextEdit::find(regex, flag)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = QTextEdit::textCursor();
        if (IsContainedInPassages(extra)) {
            extraSelections << extra;
        }
    }
    QTextEdit::setExtraSelections(extraSelections);
    QTextEdit::moveCursor(QTextCursor::Start);
}

void SearchResultBrowser::SetIncludeResultNumber(bool includeResultNumber)
{
    m_includeResultNumber = includeResultNumber;
}

void SearchResultBrowser::SetResults(const QStringList &results, const QStringList &refs, int startIdx, int endIdx)
{
    QObject::blockSignals(true);
    QTextEdit::clear();
    QTextBlockFormat format;
    format.setTopMargin(10);
    m_passageOffsets.clear();
    for (int i = startIdx; i < endIdx; ++i) {
        QTextCursor cursor = QTextEdit::textCursor();
        QString result = results[i].trimmed();
        Formatting::FormatScriptureAndRemoveNotes(result);
        cursor.movePosition(QTextCursor::End);
        i > startIdx ? cursor.insertBlock(format) : cursor.setBlockFormat(format);
        QString resultNumberStr = QString::number(i + 1);
        int start = cursor.position();
        if (m_includeResultNumber) {
            start =+ resultNumberStr.length() + 2;
            cursor.insertHtml("<span style='color: gray'>" % resultNumberStr % ":</span> " % result);
        } else {
            cursor.insertHtml(result);
        }
        m_passageOffsets << qMakePair(start, cursor.position());
        cursor.insertHtml(refs[i]);
        QTextEdit::moveCursor(QTextCursor::Start);
    }
    QObject::blockSignals(false);
}

void SearchResultBrowser::OnCursorPositionChanged()
{
    QTextCursor cursor = QTextEdit::textCursor();
    QTextBlockFormat format = cursor.blockFormat();
    format.setBackground(Qt::transparent);
    format.setBottomMargin(10);
    if (!m_lastCursor.isNull()) {
        m_lastCursor.setBlockFormat(format);
    }
    format.setBackground(QBrush(QColor(205, 255, 205)));
    cursor.setBlockFormat(format);
    m_lastCursor = cursor;
}

void SearchResultBrowser::OnAnchorClicked(const QUrl &link)
{
    QString linkText = link.toString();
    QStringList bookChapVers = linkText.split(",");
    int book = bookChapVers[0].toInt();
    int chapter = bookChapVers[1].toInt();
    int verse = bookChapVers[2].toInt();
    emit ReferenceClicked(book, chapter, verse);
}

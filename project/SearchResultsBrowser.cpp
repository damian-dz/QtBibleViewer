#include "SearchResultsBrowser.h"

SearchResultsBrowser::SearchResultsBrowser(AppConfig &config, qbv::DatabaseService &databaseService,QWidget *parent) :
    QTextBrowser(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService),
    m_includeResultNumber(true),
    m_shouldHighlightBackground(true),
    m_numResultsPerPage(25),
    m_resultIdx(0)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);
    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QObject::connect(this, QOverload<>::of(&QTextEdit::cursorPositionChanged),
                     [=] () { OnCursorPositionChanged(); });
    QObject::connect(this, QOverload<const QUrl &>::of(&QTextBrowser::anchorClicked),
                     [=] (const QUrl &url) { OnAnchorClicked(url); } );
}

void SearchResultsBrowser::SetIncludeResultNumber(bool includeResNumber)
{
    m_includeResultNumber = includeResNumber;
}

void SearchResultsBrowser::SetResult(qbv::PassageWithLocation result, bool hasStrong)
{
    SetResults(QList { result }, hasStrong);
}

void SearchResultsBrowser::SetResults(QList<qbv::PassageWithLocation> results, bool hasStrong, int pageIdx)
{
    m_results = results;
    m_hasStrong = hasStrong;
    m_resultIdx = 0;
    if (results.empty()) {
        QObject::blockSignals(true);
        QTextBrowser::clear();
        QObject::blockSignals(false);
    } else {
        DisplayPage(pageIdx);
    }
}

void SearchResultsBrowser::HighlightKeywords(const QRegularExpression &rgx)
{
    if (!rgx.pattern().isEmpty()) {
        QObject::blockSignals(true);
        QList<QTextEdit::ExtraSelection> extraSelections;
        QColor color(Qt::yellow);
        QTextDocument::FindFlag flag = rgx.patternOptions() == QRegularExpression::NoPatternOption ?
                    QTextDocument::FindCaseSensitively : QTextDocument::FindFlag(0);
        while (QTextEdit::find(rgx, flag)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);
            extra.cursor = QTextEdit::textCursor();
            if (IsContainedInPassages(extra)) {
                extraSelections << extra;
            }
        }
        QTextEdit::setExtraSelections(extraSelections);
        QTextEdit::moveCursor(QTextCursor::Start);
        QScrollBar *vScrollBar = QTextEdit::verticalScrollBar();
        vScrollBar->triggerAction(QScrollBar::SliderToMinimum);
        QObject::blockSignals(false);
    }
}

void SearchResultsBrowser::SetNumResultsPerPage(int numResults)
{
    m_numResultsPerPage = numResults;
}

void SearchResultsBrowser::DisplayPage(int idx)
{
    int startIdx = qMin(idx * m_numResultsPerPage, m_results.count() - 1);

    QObject::blockSignals(true);
    QTextEdit::clear();
    QTextBlockFormat format;
    format.setTopMargin(10);
    m_passageOffsets.clear();
    for (int i = startIdx; i < qMin(startIdx + m_numResultsPerPage, m_results.count()); ++i) {
        QTextCursor cursor = QTextEdit::textCursor();
        QString result = m_results[i].passage.trimmed();
        Formatting::FormatScriptureAndRemoveNotes(result, m_hasStrong);
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
        auto loc = m_results[i].location;
//        QString reference =  QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
//            .arg(QString::number(loc.book), QString::number(loc.chapter), QString::number(loc.verse),
//                 m_pDatabaseService->BookNameForNumber(loc.book),
//                 QString::number(loc.chapter), QString::number(loc.verse));
//        QString verseRange = loc.endVerse > loc.verse ? "%3-%4" : "%3";
//        QString chapterRange = loc.endChapter > loc.chapter ?
//            QString("%1:%2-%3:%4").arg(loc.chapter).arg(loc.verse).arg(loc.endChapter).arg(loc.endVerse) :
//            QString("%1:%2");
//        if (loc.endChapter <= loc.chapter)
//            chapterRange = chapterRange.arg(loc.chapter);

        QString chapVers;
        if (loc.endChapter > loc.chapter) {
            chapVers = QString("%1:%2–%3:%4").arg(loc.chapter).arg(loc.verse).arg(loc.endChapter).arg(loc.endVerse);
        } else {
            if (loc.endVerse > loc.verse)
                chapVers = QString("%1:%2–%3").arg(loc.chapter).arg(loc.verse).arg(loc.endVerse);
            else {
                chapVers = QString("%1:%2").arg(loc.chapter).arg(loc.verse);
            }
        }

        QString reference =  QString("<b><a href='%1,%2,%3' style='text-decoration:none'>&nbsp;— %4 %5</a></b>")
            .arg(loc.book).arg(loc.chapter).arg(loc.verse).arg(m_pDatabaseService->BookNameForNumber(loc.book))
            .arg(chapVers);

        cursor.insertHtml(reference);
        QTextEdit::moveCursor(QTextCursor::Start);
    }
    QObject::blockSignals(false);
    HighlightKeywords(m_highlightRegex);
}

void SearchResultsBrowser::DisplayNextPage()
{
    m_resultIdx += m_numResultsPerPage;
    qDebug() << m_resultIdx << (m_resultIdx / m_numResultsPerPage);
    DisplayPage(m_resultIdx / m_numResultsPerPage);
}

void SearchResultsBrowser::DisplayPrevPage()
{
    m_resultIdx -= m_numResultsPerPage;
    qDebug() << m_resultIdx << (m_resultIdx / m_numResultsPerPage);
    DisplayPage(m_resultIdx / m_numResultsPerPage);
}

bool SearchResultsBrowser::HasNext() const
{
    return m_resultIdx + m_numResultsPerPage < m_results.count();
}

bool SearchResultsBrowser::HasPrev() const
{
    return m_resultIdx > 0;
}

void SearchResultsBrowser::SetHighlightRegex(const QRegularExpression &highlightRgx)
{
    m_highlightRegex = highlightRgx;
}

void SearchResultsBrowser::SetShouldHighlightBackground(bool shouldHighlightBackground)
{
    m_shouldHighlightBackground = shouldHighlightBackground;
}

bool SearchResultsBrowser::IsBetween(int number, int first, int second) const
{
    return number >= first && number <= second;
}

bool SearchResultsBrowser::IsContainedInPassages(const QTextEdit::ExtraSelection &sel) const
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

void SearchResultsBrowser::OnAnchorClicked(const QUrl &url)
{
    QString link = url.toString();
    QStringList bookChapterVerse = link.split(",");
    int book = bookChapterVerse[0].toInt();
    int chapter = bookChapterVerse[1].toInt();
    int verse = bookChapterVerse[2].toInt();
    qbv::Location loc(book, chapter, verse, -1);
    emit ReferenceClicked(loc);
}

void SearchResultsBrowser::OnCursorPositionChanged()
{
    if (m_shouldHighlightBackground) {
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
}

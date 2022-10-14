#ifndef SEARCHRESULTBROWSER_H
#define SEARCHRESULTBROWSER_H

#include "precomp.h"

class SearchResultBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit SearchResultBrowser(QWidget *parent = nullptr);
    ~SearchResultBrowser();

    void HighlightKeywords(const QRegularExpression &regex);
    void SetIncludeResultNumber(bool includeResultNumber);
    void SetResult(const QString &result, const QString &ref);
    void SetResults(const QStringList &results, const QStringList &refs, int startIdx, int endIdx);

private:
    QTextCursor m_lastCursor;
    bool m_includeResultNumber;
    QList<QPair<int, int>> m_passageOffsets;

    bool IsBetween(int number, int first, int second) const;
    bool IsContainedInPassages(const QTextEdit::ExtraSelection &sel) const;

    void OnAnchorClicked(const QUrl &link);
    void OnCursorPositionChanged();

signals:
    void ReferenceClicked(int book, int chapter, int verse);
};

#endif // SEARCHRESULTBROWSER_H

#ifndef SEARCHRESULTSBROWSER_H
#define SEARCHRESULTSBROWSER_H

#include "precomp.h"

#include "AppConfig.h"
#include "DatabaseService.h"
#include "Location.h"

class SearchResultsBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit SearchResultsBrowser(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    void SetIncludeResultNumber(bool includeResNumber);
    void SetResult(qbv::PassageWithLocation result, bool hasStrong = false);
    void SetResults(QList<qbv::PassageWithLocation> results, bool hasStrong = false, int pageIdx = 0);
    void HighlightKeywords(const QRegularExpression &rgx);
    void SetNumResultsPerPage(int numResults);
    void DisplayPage(int idx);
    void DisplayNextPage();
    void DisplayPrevPage();
    bool HasNext() const;
    bool HasPrev() const;

    void SetHighlightRegex(const QRegularExpression &highlightRgx);
    void SetShouldHighlightBackground(bool shouldHighlightBackground);

private:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    QTextCursor m_lastCursor;
    bool m_includeResultNumber;
    bool m_shouldHighlightBackground;
    QList<QPair<int, int>> m_passageOffsets;
    int m_numResultsPerPage;
    int m_resultIdx;


    QRegularExpression m_highlightRegex;
    QList<qbv::PassageWithLocation> m_results;
    bool m_hasStrong;


    bool IsBetween(int number, int first, int second) const;
    bool IsContainedInPassages(const QTextEdit::ExtraSelection &sel) const;

    void OnAnchorClicked(const QUrl &link);
    void OnCursorPositionChanged();
};

#endif // SEARCHRESULTSBROWSER_H

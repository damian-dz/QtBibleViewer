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

    void SetResults(QList<qbv::PassageWithLocation> results, bool hasStrong = false);
    void HighlightKeywords(const QRegularExpression &rgx);

private:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    QTextCursor m_lastCursor;
    bool m_includeResultNumber;
    QList<QPair<int, int>> m_passageOffsets;

    QList<qbv::PassageWithLocation> m_results;

    bool IsBetween(int number, int first, int second) const;
    bool IsContainedInPassages(const QTextEdit::ExtraSelection &sel) const;

    void OnAnchorClicked(const QUrl &link);
    void OnCursorPositionChanged();
};

#endif // SEARCHRESULTSBROWSER_H

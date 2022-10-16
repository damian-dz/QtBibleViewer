#ifndef PASSAGEBROWSERNEW_H
#define PASSAGEBROWSERNEW_H

#include "precomp.h"

#include "AppConfig.h"
#include "DatabaseService.h"
#include "Formatting.h"
#include "PassageWithNotes.h"
#include "TabbedLocation.h"


class PassageBrowserNew: public QTextBrowser
{
    Q_OBJECT
public:
    explicit PassageBrowserNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);
    ~PassageBrowserNew();

    void SetAndLoadPassageWithNotes(qbv::PassageWithNotes passageWithNotes);
    void SetPassageWithNotes(qbv::PassageWithNotes passageWithNotes);
    void LoadPassageWithNotes();

    bool HasText() const;

    qbv::Location Location() const;
    void SetLocation(qbv::Location loc);

private:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;
    qbv::PassageWithNotes m_passage;

    qbv::Location m_location;
    QTextCursor m_lastCursor;

    QPair<int, int> m_selectedBlockRange;
    QPair<int, int> m_selectedVerseRange;

    void OnContextMenuRequested(const QPoint &pos);
    void OnCursorPositionChanged();
    void OnHighlighted(const QString &link);

    typedef void (&FormattingFunc)(QString &);
    void CopyWithReferenceTemplate(FormattingFunc format);
    void OnCopyWithReference();
    void OnRemoveHighlight();
    void OnAddNote();

    void ComputeSelectedBlockAndVerseRanges();
    void CopyToClipboard(QString &textToCopy, int verse1, int verse2);

signals:
    void AddNoteRequested(qbv::Location loc);
};

#endif // PASSAGEBROWSERNEW_H

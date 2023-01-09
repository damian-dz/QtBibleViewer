#ifndef PASSAGEBROWSER_H
#define PASSAGEBROWSER_H

#include "precomp.h"

#include "AppConfig.h"
#include "TabbedLocation.h"

class PassageBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit PassageBrowser(const QStringList &bookNames, AppConfig *config, QWidget *parent = nullptr);
    ~PassageBrowser();

    int GetBook() const;
    int GetChapter() const;
    qbv::Location GetLocation() const;
    int GetVerse1() const;
    int GetVerse2() const;
    int getVerseCount() const;
    void HighlightText(const QString &text);
    void HighlightVerse(int verse);
    void InsertNotFoundMessage(QTextCursor& cursor, int missingVerseCount, int& verseNumber);
    bool IsEmpty() const;
    void LoadPassage(const QSqlDatabase &module);
    void LoadPassage_New(const QSqlDatabase &module);
    void SelectPassage(int verse1, int verse2);
    void SetBook(int book);
    void SetChapter(int chapter);
    void SetHasOldTestament(bool hasOldTestament);
    void SetHasStrong(bool hasStrong);
    void SetLocation(const qbv::Location &location);
    void SetLocation(int book, int chapter, int verse1, int verse2);
    void SetVerse1(int verse1);
    void SetVerse2(int verse2);
    void SetHighlightColor(QColor &color);

private:
    const QStringList *m_pBookNames;
    AppConfig *m_pConfig;

    QPair<int, int> m_selectedBlockRange;
    bool m_hasOldTestament;
    bool m_hasStrong;
    QColor *m_highlightColor;
    QTextCursor m_lastCursor;
    qbv::Location m_location;
    QStringList m_notes;
    QList<int> m_verseNumbers;
    QPair<int, int> m_selectedVerseRange;
    QStringList m_versesUnformatted;

    typedef void (&FormattingFunc)(QString &);

    void ComputeSelectedBlockAndVerseRange();
    void CopyToClipboard(QString &textToCopy, int verse1, int verse2);
    void CopyWithReferenceTemplate(FormattingFunc format);
    void OnAddToFavorites();
    void OnContextMenuRequested(const QPoint &pos);
    void OnCursorPositionChanged();
    void OnCopyWithReference();
    void OnCopyAsTeXWithReference();
    void OnCopyAsHtmlWithReference();
    void OnHighlighted(const QUrl &link);
    void OnShowCrossReferences();

signals:
    void CrossReferenceRequested(int verse);
    void AddToFavoritesRequested(qbv::Location loc);
};

#endif // PASSAGEBROWSER_H

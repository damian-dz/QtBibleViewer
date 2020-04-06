#ifndef BIBLEPASSAGEBROWSER_H
#define BIBLEPASSAGEBROWSER_H

#include "precomp.h"

class BiblePassageBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit BiblePassageBrowser(QWidget *parent = nullptr);
    ~BiblePassageBrowser();

    void formatPassage(QString &text);
    int getBook() const;
    int getChapter() const;
    QString getPassageAsPlainText(int verseFrom, int verseTo, bool includeVerseNumber = true);
    int getVerseCount() const;
    void insertNotFoundMessage(QTextCursor& cursor, int missingVerseCount, int& verseNumber);
    void loadPassageV1(const QSqlDatabase &module);
    void loadPassageV2(const QSqlDatabase &module);
    QString removeVerseNumber(const QString &verse);
    void selectPassage(int verseFrom, int verseTo);
    void setBook(int book);
    void setChapter(int chapter);
    void setHasOldTestament(int hasOldTestament);
    void setHasStrong(bool hasStrong);
    void setIsZeroIndexed(bool isZeroIndexed);
    void setPassage(int book, int chapter, int verseFrom, int verseTo);
    void setVerseCount(int count);
    void setVerseFrom(int verseFrom);
    void setVerseTo(int verseTo);

private:
    int m_book;
    int m_chapter;
    bool m_isZeroIndexed;
    bool m_hasOldTestament;
    bool m_hasStrong;
    QStringList m_notes;
    int m_verseCount;
    int m_verseFrom;
    int m_verseTo;

    void initialize();

private slots:
    void onTextBrowserHighlighted(const QString &link);
};

#endif // BIBLEPASSAGEBROWSER_H

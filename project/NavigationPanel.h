#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H

#include "precomp.h"

#include "TabbedLocation.h"

class NavigationPanel : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit NavigationPanel(const QSqlDatabase& verseData, const QStringList &bookNames, const QStringList &bookAbbreviations, QWidget *parent = nullptr);

    int GetChapterCount(int book);
    qbv::Location GetLocation() const;
    int GetNumVerses(int book, int chapter);
    int GetChapterId(int book, int chapter);
    int GetVerseId(int book, int chapter, int verseFrom);
    int GetSelectedBook() const;
    int GetSelectedChapter() const;
    int GetSelectedVerseFrom() const;
    int GetSelectedVerseTo() const;
    void HideVerseTo();
    void reloadBookNames();
    void SetLocation_Old(int book, int chapter, int verseFrom, int verseTo, bool sendSignal = true);
    void SelectPassageByChapterId(int chapterId);
    void SelectPassageByVerseId(int verseId);
    void SetMoveByVerse(bool moveByVerse);
    void SetLocation(int book, int chapter, bool load = false);
    void SetLocation(qbv::Location location, bool load = false);
    void SetUiTexts();

signals:
    void LocationChanged(int book, int chapter, int verse1, int verse2);
    void randomChapterRequested();

private:
    QListWidget *ui_ListWidget_Book;
    QListWidget *ui_ListWidget_Chapter;
    QListWidget *ui_ListWidget_VerseFrom;
    QListWidget *ui_ListWidget_VerseTo;

    QWidget *ui_Bib_Widget_Find;
    QLabel *ui_Bib_Label_Find;
    QLabel *ui_Label_Book;
    QLabel *ui_Label_Chapter;
    QLabel *ui_Label_Verses;
    QLineEdit *ui_LineEdit_Find;
    QMenuBar *ui_MenuBar_Main;
    QPushButton *ui_Button_Close;
    QPushButton *ui_Button_Next;
    QPushButton *ui_Button_Prev;
    QPushButton *ui_Button_Random;
    QTabBar *ui_TabBar_Modules;
    QLineEdit *ui_LineEdit_MatchPassage;
    QVBoxLayout *ui_VerLayout_Modules;

    bool m_moveByVerse = false;
    QHash<QString, QLabel *> m_labels;
    const QStringList *m_bookNames;
    const QStringList *m_bookAbbreviations;
    int m_chapterId;
    int m_verseId;
    const QSqlDatabase *m_pVerseData;

    void ConnectSignals();
    QStringList GenerateRange(int min, int max);

    void OnLocationChanged(bool emitSignals = true);

private slots:
    void OnCurrentBookChanged(int currentRow);
    void OnCurrentChapterChanged(int currentRow);
    void OnCurrentVerseFromChanged(int currentRow);
    void OnCurrentVerseToChanged(int currentRow);

    void OnNextButtonClicked();
    void OnPrevButtonClicked();

    void OnMatchPassage();

    void OnRandomButtonClicked();
};

#endif // NAVIGATIONPANEL_H

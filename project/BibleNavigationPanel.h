#ifndef BIBLENAVIGATIONPANEL_H
#define BIBLENAVIGATIONPANEL_H

#include "precomp.h"

class BibleNavigationPanel : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit BibleNavigationPanel(const QSqlDatabase& verseData, const QStringList &bookNames, QWidget *parent = nullptr);

    int getChapterCount(int book);
    int getVerseCount(int book, int chapter);
    int getChapterNumber(int book, int chapter);
    void selectPassage(int book, int chapter);
    void selectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal = true);
    void selectPassage(int chapterNumber);
    void setUiTexts();

signals:
    void passageChanged(int book, int chapter, int verseFrom, int verseTo);
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

    QHash<QString, QLabel *> m_labels;
    const QStringList *m_bookNames;
    int m_chapterNumber;
    const QSqlDatabase *m_verseData;

    void connectSignals();
    QStringList generateRange(int min, int max);

    void sendPassageLocation();

private slots:
    void onCurrentBookChanged(int currentRow);
    void onCurrentChapterChanged(int currentRow);
    void onCurrentVerseFromChanged(int currentRow);
    void onCurrentVerseToChanged(int currentRow);

    void onNextButtonClicked();
    void onPrevButtonClicked();

    void onRandomButtonClicked();
};

#endif // BIBLENAVIGATIONPANEL_H

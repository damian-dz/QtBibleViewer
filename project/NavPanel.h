#ifndef NAVPANEL_H
#define NAVPANEL_H

#include "DatabaseService.h"

class NavPanel  : public QGridLayout
{
    Q_OBJECT
public:
    explicit NavPanel(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    void AddControls();
    void ConnectSignals();
    void SetUiTexts();


    qbv::Location GetLocation() const;
    void SetLocation(qbv::Location loc, bool emitSignal);

    int GetChapterId() const;
    void HideVerseTo();

signals:
    void LocationChanged(qbv::Location loc);

private:
    QListWidget *ui_ListWidget_Book;
    QListWidget *ui_ListWidget_Chapter;
    QListWidget *ui_ListWidget_VerseFrom;
    QListWidget *ui_ListWidget_VerseTo;

    QLabel *ui_Bib_Label_Find;
    QLabel *ui_Label_Book;
    QLabel *ui_Label_Chapter;
    QLabel *ui_Label_Verses;

    QLineEdit *ui_LineEdit_MatchPassage;

    QPushButton *ui_Button_Next;
    QPushButton *ui_Button_Prev;
    QPushButton *ui_Button_Random;

    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    bool m_moveByVerse;
    int m_chapterId;
    int m_verseId;

    QStringList GenerateRange(int min, int max);

    void OnCurrentBookChanged(int book);
    void OnCurrentChapterChanged(int chapter);
    void OnCurrentVerseFromChanged(int verse, bool emitSignals = true);
    void OnCurrentVerseToChanged(int verse, bool emitSignals = true);
    void OnRandomButtonClicked();
    void OnNavButtonClicked(int direction);
    void OnNextButtonClicked();

    void OnLocationChanged(bool emitSignals = true);

    void UpdateLocationInConfig(qbv::Location loc);

};

#endif // NAVPANEL_H

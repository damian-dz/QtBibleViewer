#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpacerItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStatusBar>
#include <QStringBuilder>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTime>
#include <QToolTip>
#include <QTranslator>
#include <QWidget>

#define DEFAULT_WIDTH 920
#define DEFAULT_HEIGHT 600
#define DEFAULT_FONT_FAMILY qApp->font().family()
#define DEFAULT_FONT_SIZE 10
#define MAX_WIDGET_HEIGHT 16777215

#define SET_LANGUAGE "language"
#define SET_MAX_RECENT_PASSAGES "maxRecent"
#define GROUP_MAIN_WINDOW "MainWindow"
#define SET_GEOMETRY "geometry"
#define SET_STATE "state"
#define GROUP_MODULE_DATA "ModuleData"
#define SET_INDEX "index"
#define SET_PASSAGE "passage"
#define SET_PATHS "paths"
#define SET_REMOVED_PATHS "removedPaths"
#define SET_COM_VERSE "comVerse"
#define GROUP_FONT "Font"
#define SET_FONT_FAMILY "family"
#define SET_FONT_SIZE "size"
#define GROUP_APPEARANCE "Appearance"
#define SET_STYLE "style"
#define SET_USE_BACKGROUND "useBackground"
#define SET_HIGHLIGHT_COLOR "highlightColor"
#define SET_TAB_POSITION "tabPosition"

#define ICON_ARROW_LEFT ":/img/img_res/arrleft.svg"
#define ICON_ARROW_RIGHT ":/img/img_res/arrright.svg"
#define ICON_CLOSE ":/img/img_res/close.svg"
#define ICON_COPY ":/img/img_res/copy.svg"
#define ICON_COPY_PLUS ":/img/img_res/copyplus.svg"
#define ICON_EXIT ":/img/img_res/exit.svg"
#define ICON_FIND ":/img/img_res/find.svg"
#define ICON_FOLDER ":/img/img_res/folder.svg"
#define ICON_HEART ":/img/img_res/heart.svg"
#define ICON_MAGNIFY ":/img/img_res/magnify.svg"
#define ICON_MINIFY ":/img/img_res/minify.svg"

#define COMBOBOX_STYLE "combobox-popup: 0"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &appDir, const QString &lang, const QString &style, const QString configPath, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void action_ChapterBrowser_Copy();
    void action_ChapterBrowser_CopyWithReference();
    void action_ChapterBrowser_InsertIntoFavorites();
    void action_ChapterBrowser_SelectAll();
    void action_EditMenu_CopyWithReference();
    void actionBack();
    void actionClear();
    void actionCopy();
    void actionCut();
    void actionFind();
    void actionForward();
    void actionPaste();
    void actionSelectAll();
    void actionShowBasicContextMenu(const QPoint &pos);
    void actionShowEditContextMenu(const QPoint &pos);
    void actionShowLineContextMenu(const QPoint &pos);
    void on_Bib_AnchorClicked_ChapterBrowser(const QUrl &arg1);
    void on_Bib_Clicked_PushButton_Close();
    void on_Bib_Clicked_PushButton_Next();
    void on_Bib_Clicked_PushButton_Prev();
    void on_Bib_Clicked_PushButton_Random();
    void on_Bib_CurrentChanged_TabWidget_Modules(int index);
    void on_Bib_CurrentIndexChanged_ComboBox_VerseFrom(int index);
    void on_Bib_CurrentIndexChanged_ComboBox_VerseTo(int index);
    void on_Bib_CurrentRowChanged_ListWidget_Book(int currentRow);
    void on_Bib_CurrentRowChanged_ListWidget_Chapter(int currentRow);
    void on_Bib_CursorPositionChanged_chapterBrowser();
    void on_Bib_CustomContextMenuRequested_ChapterBrowser(const QPoint &pos);
    void on_Bib_Highlighted_ChapterBrowser(const QUrl &arg1);
    void on_Bib_SelectionChanged_ChapterBrowser();
    void on_Bib_TabCloseRequested_TabWidget_Modules(int index);
    void on_Bib_TabMoved_Modules(int from, int to);
    void on_Bib_TextChanged_LineEdit_Find(const QString &text);
    void on_Com_AnchorClicked_TextBrowser_Compare(const QUrl &arg1);
    void on_Com_CurrentRowChanged_ListWidget_Book(int currentRow);
    void on_Com_CurrentRowChanged_ListWidget_Chapter(int currentRow);
    void on_Com_CurrentRowChanged_ListWidget_Verse(int currentRow);
    void on_CurrentChanged_TabWidget_Main(int index);
    void on_Dic_AnchorClicked_TextBrowser_Definition(const QUrl &arg1);
    void on_Dic_Highlighted_TextBrowser_Definition(const QUrl &arg1);
    void on_Dic_CurrentRowChanged_ListWidget_Dictionaries(int currentRow);
    void on_Dic_TextChanged_ListWidget_AllEntries(const QString &currentText);
    void on_Dic_TextEdited_LineEdit_Number(const QString &arg1);
    void on_Fav_AnchorClicked_TextBrowser_Passage(const QUrl &arg1);
    void on_Fav_Clicked_PushButton_Delete();
    void on_Fav_Clicked_PushButton_Save();
    void on_Fav_CurrentRowChanged_ListWidget_Passages(int currentRow);
    void on_Sea_AnchorClicked_TextBrowser_Results(const QUrl &arg1);
    void on_Sea_Clicked_PushButton_Next();
    void on_Sea_Clicked_PushButton_Prev();
    void on_Sea_Clicked_PushButton_RandomVerse();
    void on_Sea_Clicked_PushButton_Search();
    void on_Sea_ComboBox_CurrentIndexChanged_Section(int index);
    void on_Sea_ComboBox_CurrentIndexChanged_Translation(int index);
    void on_Sea_LineEdit_ReturnPressed_Search();
    void on_Sea_LineEdit_TextChanged_Search(const QString &text);
    void on_Sea_Toggled_RadioButton_Strong(bool checked);

private:
    int cursorCnt = 0;
    /* Auxiliary structures */
    struct ModuleData
    {
        QSqlDatabase database;
        QString name;
        bool hasOldTestament;
        bool hasStrong;
    };

    struct TabBookChapterVerses
    {
        int tab;
        int book;
        int chapter;
        int verseFrom;
        int verseTo;

        TabBookChapterVerses() :
            tab(-1), book(-1), chapter(-1), verseFrom(-1), verseTo(-1) { }

        TabBookChapterVerses(int tab, int book, int chapter, int verseFrom, int verseTo) :
            tab(tab), book(book), chapter(chapter), verseFrom(verseFrom), verseTo(verseTo) { }

        TabBookChapterVerses(const TabBookChapterVerses &tbcvv) :
            tab(tbcvv.tab),
            book(tbcvv.book),
            chapter(tbcvv.chapter),
            verseFrom(tbcvv.verseFrom),
            verseTo(tbcvv.verseTo) { }

        bool isPassageEqual(const TabBookChapterVerses &tbcvv)
        {
            return (book == tbcvv.book &&
                    chapter == tbcvv.chapter &&
                    verseFrom == tbcvv.verseFrom &&
                    verseTo == tbcvv.verseTo);
        }

        bool isSameAs(const TabBookChapterVerses &tbcvv)
        {
            return (tab == tbcvv.tab &&
                    book == tbcvv.book &&
                    chapter == tbcvv.chapter &&
                    verseFrom == tbcvv.verseFrom &&
                    verseTo == tbcvv.verseTo);
        }

        TabBookChapterVerses operator=(const TabBookChapterVerses &tbcvv) {
            tab = tbcvv.tab;
            book = tbcvv.book;
            chapter = tbcvv.chapter;
            verseFrom = tbcvv.verseFrom;
            verseTo = tbcvv.verseTo;
            return *this;
        }

        bool operator==(const TabBookChapterVerses &tbcvv) {
            return this->isSameAs(tbcvv);
        }

        bool operator!=(const TabBookChapterVerses &tbcvv) {
            return !this->isSameAs(tbcvv);
        }
    };

    /* GUI */
    QAction *ui_Act_Back;
    QAction *ui_Act_Copy;
    QAction *ui_Act_CopyWithRef;
    QAction *ui_ActDecreaseFont;
    QAction *ui_Act_Forward;
    QAction *ui_ActIncreaseFont;
    QLabel *ui_Label_Status;
    QMenu *ui_Menu_Language;
    QStatusBar *ui_StatusBar_Status;
    QTabWidget *ui_TabWidget_Main;
    QTextBrowser *m_textBrowser;
    QTextEdit *m_textEdit;
    QLineEdit *m_lineEdit;

    /* Bible Tab */
    QComboBox *ui_Bib_ComboBox_VerseFrom;
    QComboBox *ui_Bib_ComboBox_VerseTo;
    QLabel *ui_Bib_Label_Find;
    QLineEdit *ui_Bib_LineEdit_Find;
    QListWidget *ui_Bib_ListWidget_Book;
    QListWidget *ui_Bib_ListWidget_Chapter;
    QMenuBar *ui_Bib_MenuBar_Main;
    QPushButton *ui_Bib_Button_Close;
    QPushButton *ui_Bib_Button_Next;
    QPushButton *ui_Bib_Button_Prev;
    QPushButton *ui_Bib_Button_Random;
    QTabBar *ui_Bib_TabBar_Modules;
    QTabWidget *ui_Bib_TabWidget_Modules;

    /* Details Tab */
    QCheckBox *ui_Det_CheckBox_NewTestament;
    QCheckBox *ui_Det_CheckBox_OldTestament;
    QCheckBox *ui_Det_CheckBox_RightToLeft;
    QCheckBox *ui_Det_CheckBox_StrongsNumbers;
    QLineEdit *ui_Det_LineEdit_Abbreviation;
    QLineEdit *ui_Det_LineEdit_PublishDate;
    QLineEdit *ui_Det_LineEdit_Version;
    QLineEdit *ui_Det_LineEdit_VersionDate;
    QTextBrowser *ui_Det_TextBrowser_Comments;
    QTextBrowser *ui_Det_TextBrowser_Description;
    QVBoxLayout *ui_Bib_VerLayout_Modules;

    /* Search Tab */
    QCheckBox *ui_Sea_CheckBox_Case;
    QCheckBox *ui_Sea_CheckBox_WholeWords;
    QComboBox *ui_Sea_ComboBox_ResPerPage;
    QComboBox *ui_Sea_ComboBox_SearchFrom;
    QComboBox *ui_Sea_ComboBox_SearchTo;
    QComboBox *ui_Sea_ComboBox_Section;
    QComboBox *ui_Sea_ComboBox_Translation;
    QLineEdit *ui_Sea_LineEdit_Search;
    QPushButton *ui_Sea_Button_Next;
    QPushButton *ui_Sea_Button_Prev;
    QPushButton *ui_Sea_Button_RandomVerse;
    QPushButton *ui_Sea_Button_Search;
    QRadioButton *ui_Sea_RadioButton_All;
    QRadioButton *ui_Sea_RadioButton_Any;
    QRadioButton *ui_Sea_RadioButton_Exact;
    QRadioButton *ui_Sea_RadioButton_Strong;
    QTextBrowser *ui_Sea_TextBrowser_RandomVerse;
    QTextBrowser *ui_Sea_TextBrowser_Results;

    /* Compare Tab */
    QLabel *ui_Com_Label_ChapterNumber;
    QListWidget *ui_Com_ListWidget_Book;
    QListWidget *ui_Com_ListWidget_Chapter;
    QListWidget *ui_Com_ListWidget_Verse;
    QPushButton *ui_Com_Button_Next;
    QPushButton *ui_Com_Button_Prev;
    QTextBrowser *ui_Com_TextBrowser_Compare;

    /* Favorites Tab */
    QListWidget *ui_Fav_ListWidget_Passages;
    QPushButton *ui_Fav_Button_Delete;
    QPushButton *ui_Fav_Button_Save;
    QTextBrowser *ui_Fav_TextBrowser_Passage;
    QTextEdit *ui_Fav_TextEdit_Comment;

    /* Dictionary Tab */
    QListWidget *ui_Dic_ListWidget_AllEntries;
    QTextBrowser *ui_Dic_TextBrowser_Definition;

    /* Topics Tab */
    QTextBrowser *ui_Top_TextBrowser_Verses;

    /* Member variables */
    bool m_blockHistory;
    QPair<int, int> m_blockRange;
    QStringList m_bookNames;
    QList<QTextBrowser*> m_chapterBrowsers;
    QStringList m_comVerse;
    int m_crntStartRes;
    QFont m_currentFont;
    QSqlDatabase m_dbCntr;
    QSqlDatabase m_dbDict;
    QSqlDatabase m_dbUsr;
    QSqlDatabase m_dbXRef;
    QStringList m_dictPathList;
    QRegExp m_dispRgx;
    QString m_elapsedTime;
    QString m_executionPath;
    bool m_firstLoadCompare;
    QList<TabBookChapterVerses> m_favorites;
    QList<QStringList> m_globalNotes;
    QColor m_highlightColor;
    QVector<TabBookChapterVerses> m_history;
    QString m_language;
    QMap<int, QString> m_languages;
    QList<QTextCursor> m_lastCursors;
    QList<bool> m_loadedFlags;
    int m_maxRecentPassages;
    QStringList m_modulePathsList;
    QList<ModuleData> m_modules;
    bool m_modulesFound;
    int m_noteCount;
    int m_numResPerPage;
    QPixmap m_papyrusBckgrnd;
    int m_psgIdx;
    QStringList m_removedPathsList;
    QStringList m_resRefs;
    QStringList m_resVerses;
    QStringList m_sectionNames;
    QString m_settingsPath;
    QString m_style;
    int m_tabPos;
    bool m_useBackground;
    QList<QMap<int, int>> m_verseMaps;
    QPair<int, int> m_verseRange;

    /* Member functions */
    bool loadBibleModule(const QString &path);
    QString formatResult(QString &text, const QRegExp &regex, bool hasStrong);
    QString formatVerse(QString text, bool hasStrong);
    QStringList getModulePaths(const QString &path);
    TabBookChapterVerses getTabBookChapterVerses();
    TabBookChapterVerses loadSettings();
    void actHistory(bool goBack);
    void actionAboutQt();
    void actionDecreaseFontSize();
    void actionEnglish();
    void actionExit();
    void actionIncreaseFontSize();
    void actionOpenBibleModule();
    void actionPolish();
    void actionPreferences();
    void actionSpanish();
    void actionWordFrequency();
    void addMainTabs();
    void addModuleLayout(int index);
    void blockPassageSelectionSignals(bool isBlocked);
    void checkFontSizes();
    void checkLanguageAction(int idx, bool firstRun = false);
    void clearChapterBrowserData(int idx);
    void connectBibleTabSignals();
    void connectCompareTabSignals();
    void connectDetailsTabSignals();
    void connectDictionaryTabSignals();
    void connectFavoritesTabSignals();
    void connectSearchTabSignals();
    void displaySearchResults(int startIdx, int endIdx);
    void fillDetailsTab();
    void formatPassage(QString &text, bool hasStrong);
    void formatScripture(QString &text, int idx, bool hasStrong, const QRegularExpression &noteRgx, const QRegularExpression &strongRgx);
    void generateBibleTabControls();
    void generateBibModuleTabs();
    void generateCompareTabControls(int idx);
    void generateDetailsTab(int idx);
    void generateDictionaryTabControls(int idx);
    void generateFavoritesTabControls(int idx);
    void generateMainLayout();
    void generateMenuBarItems();
    void generateSearchTabControls(int idx);
    void generateTopicsTab(int idx);
    void getVerseRange();
    void highlightPassage(const TabBookChapterVerses &tbcvv);
    void iterateRecords(QSqlQuery &query, const QString &text, Qt::CaseSensitivity sensitivity, bool wholeWords, bool hasStrong);
    void iterateRecords(QSqlQuery &query, const QStringList &words, Qt::CaseSensitivity sensitivity, bool wholeWords, bool containsAll);
    void loadBackgroundPixmap();
    void loadFavorites();
    void loadPassage();
    void loadXRefDatabase();
    void openStrongDialog(const QString &number);
    void performSearch();
    void performSearchByStrong();
    void populateBookNames();
    void populateChapterListWidget(int chapter);
    void populateLanguageMap(const QString &lang);
    void populateSectionNames();
    void populateVersesComboBoxes(int verseFrom, int verseTo);
    void removeTabFromHistory(int idx);
    void saveSettings();
    void setBrowserBackground(QTextEdit &browser);
    void setTabBookChapterVerses(const TabBookChapterVerses &tbcvv, bool firstRun);
    void swapTabHistory(int from, int to);
    void updateFonts();
    void updateHistory(const TabBookChapterVerses &tbcvv);

    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H

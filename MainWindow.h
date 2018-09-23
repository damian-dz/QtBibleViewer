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
#include <QTabWidget>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTime>
#include <QToolTip>
#include <QTranslator>
#include <QWidget>

#define DEFAULT_WIDTH 920
#define DEFAULT_HEIGHT 600
#define DEFAULT_FONT_FAMILY "Arial"
#define DEFAULT_FONT_SIZE 10
#define MAX_RECENT_PASSAGES 100
#define MAX_WIDGET_HEIGHT 16777215

#define SET_LANGUAGE "language"
#define GROUP_MAIN_WINDOW "MainWindow"
#define SET_GEOMETRY "geometry"
#define SET_STATE "state"
#define GROUP_MODULE_DATA "ModuleData"
#define SET_INDEX "index"
#define SET_PASSAGE "passage"
#define SET_PATHS "paths"
#define SET_REMOVED_PATHS "removedPaths"
#define GROUP_FONT_SETTINGS "FontSettings"
#define SET_FONT_FAMILY "fontFamily"
#define SET_FONT_SIZE "fontSize"

#define ICON_ARROW_LEFT ":/img/img_res/arrleft.svg"
#define ICON_ARROW_RIGHT ":/img/img_res/arrright.svg"
#define ICON_CLOSE ":/img/img_res/close.svg"
#define ICON_COPY ":/img/img_res/copy.svg"
#define ICON_COPY_PLUS ":/img/img_res/copyplus.svg"
#define ICON_EXIT ":/img/img_res/exit.svg"
#define ICON_FOLDER ":/img/img_res/folder.svg"
#define ICON_HEART ":/img/img_res/heart.svg"
#define ICON_MAGNIFY ":/img/img_res/magnify.svg"
#define ICON_MINIFY ":/img/img_res/minify.svg"

#define COMBOBOX_STYLE "combobox-popup: 0;"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &appDir, const QString &lang, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void actBack();
    void actChapterBrowserSelectAll();
    void actFind();
    void actCopy();
    void actCopyWithReference();
    void actEditMenuCopyWithReference();
    void actForward();
    void clickedBibButtonClose();
    void textChangedBibLineEditFind(const QString &text);
    void bookListWidgetCurrentRowChanged(int currentRow);
    void chapterBrowserAnchorClicked(const QUrl &arg1);
    void chapterBrowserCustomContextMenuRequested(const QPoint &pos);
    void chapterBrowserHighlighted(const QUrl &arg1);
    void chapterBrowserSelectionChanged();
    void chapterListWidgetCurrentRowChanged(int currentRow);
    void mainTabWidgetCurrentChanged(int index);
    void modulesTabCloseRequested(int index);
    void modulesTabMoved(int from, int to);
    void modulesTabWidgetCurrentChanged(int index);
    void nextResultButtonClicked();
    void previousResultButtonClicked();
    void buttonClicked_BibNextChapter();
    void buttonClicked_BibPreviousChapter();
    void buttonClicked_BibRandomChapter();
    void buttonClicked_SeaRandomVerse();
    void searchButtonClicked();
    void searchLineEditReturnPressed();
    void searchLineEditTextChanged(const QString &text);
    void sectionCurrentIndexChanged(int index);
    void verseFromComboBoxCurrentIndexChanged(int index);
    void verseToComboBoxCurrentIndexChanged(int index);
    void currentRowChangedComListWidgetBook(int currentRow);
    void currentRowChangedComListWidgetChapter(int currentRow);
    void currentRowChangedComListWidgetVerse(int currentRow);

private:
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
        friend bool operator == (const TabBookChapterVerses &x, const TabBookChapterVerses &y) {
            return (x.tab == y.tab &&
                    x.book == y.book &&
                    x.chapter == y.chapter &&
                    x.verseFrom == y.verseFrom &&
                    x.verseTo == y.verseTo);
        }
    };

    /* GUI */
    QAction *ui_ActBack;
    QAction *ui_ActCopy;
    QAction *ui_ActCopyWithRef;
    QAction *ui_ActDecreaseFont;
    QAction *ui_ActForward;
    QAction *ui_ActIncreaseFont;
    QLabel *ui_Label_Status;
    QMenu *ui_Menu_Language;
    QStatusBar *ui_StatusBar_Status;
    QTabWidget *ui_TabWidget_Main;

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
    QTextBrowser *ui_Fav_TextBrowser_Passage;
    QTextBrowser *ui_Fav_TextBrowser_Comment;

    /* Dictionary Tab */
    QTextBrowser *ui_Dic_TextBrowser_Definition;

    /* Topics Tab */
    QTextBrowser *ui_Top_TextBrowser_Verses;

    /* Member variables */
    bool m_blockHistory;
    bool m_modulesFound;
    int m_crntStartRes;
    int m_numResPerPage;
    int m_psgIdx;
    QFont m_currentFont;
    QList<QStringList> m_globalNotes;
    QList<QTextBrowser *> m_chapterBrowsers;
    QList<ModuleData> m_modules;
    QMap<int, QString> m_languages;
    QPair<int, int> m_range;
    QPixmap m_papyrusBckgrnd;
    QRegExp m_dispRgx;
    QSqlDatabase m_dbCntr;
    QSqlDatabase m_dbXRef;
    QString m_elapsedTime;
    QString m_executionPath;
    QString m_language;
    QString m_settingsPath;
    QStringList m_bookNames;
    QStringList m_modulePathsList;
    QStringList m_removedPathsList;
    QStringList m_resRefs;
    QStringList m_resVerses;
    QStringList m_sectionNames;
    QVector<TabBookChapterVerses> m_history;

    /* Member functions */
    bool loadBibleModule(const QString &path);
    QString formatResult(QString &text, const QRegExp &regex, bool hasStrong);
    QString formatVerses(QString text, bool hasStrong);
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
    void connectBibleTabSignals();
    void connectSearchTabSignals();
    void displaySearchResults(int startIdx, int endIdx);
    void fillDetailsTab();
    void formatPassage(QString &text, bool hasStrong);
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
    void iterateRecords(QSqlQuery &query, const QStringList &words,
                        Qt::CaseSensitivity sensitivity, bool wholeWords, bool containsAll);
    void iterateRecords(QSqlQuery &query, const QString &text, bool wholeWords, bool hasStrong);
    void loadBackgroundPixmap();
    void loadPassage();
    void loadXRefDatabase();
    void performSearch();
    void populateBookNames();
    void populateChapterListWidget(int chapter);
    void populateLanguageMap(const QString &lang);
    void populateSectionNames();
    void populateVersesComboBoxes(int verseFrom, int verseTo);
    void saveSettings();
    void setBrowserBackground(QTextBrowser &browser);
    void setTabBookChapterVerses(const TabBookChapterVerses &tbcvv, bool firstRun);
    void updateFonts();
    void updateHistory(const TabBookChapterVerses &tbcvv);

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

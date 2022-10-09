#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define MENU_FILE "File"
//#define MENU_EDIT "Edit"
//#define MENU_STATISTICS "Statistics"
//#define MENU_OPTIONS "Options"
//#define MENU_VIEW "View"
//#define MENU_HELP "Help"

//#define ACT_OPEN_MODULE "OpenModule"
//#define ACT_MODULE_INFO "ModuleInfo"
//#define ACT_EXIT "Exit"
//#define ACT_SELECT_ALL "SelectAll"
//#define ACT_FIND "Find"
//#define ACT_WORD_FREQUENCY "WordFrequency"
//#define ACT_COMMON_WORDS "CommonWords"
//#define ACT_PREFERENCES "Preferences"
//#define ACT_HELP "Help"
//#define ACT_ABOUT "About"
//#define ACT_ABOUT_QT "AboutQt"

#define DEFAULT_WIN_WIDTH 920
#define DEFAULT_WIN_HEIGHT 600
#define MIN_WIN_WIDTH 720
#define MIN_WIN_HEIGHT 480

#define APP_MODULES_DIR "/App/modules"
#define APP_LANG_DIR "/App/lang"
#define VERSE_DATA_FILE_PATH "/App/data/VerseData.bblv"
#define XREF_FILE_PATH "/App/data/xref.bblv"
#define FAV_FILE_PATH "/App/data/fav.bblv"

#define ICON_FOLDER ":/img/img_res/folder.svg"
#define ICON_ARROW_LEFT ":/img/img_res/arrow_left.svg"
#define ICON_ARROW_RIGHT ":/img/img_res/arrow_right.svg"
#define ICON_BUBBLE ":/img/img_res/bubble.svg"
#define ICON_CLOSE ":/img/img_res/close.svg"
#define ICON_COGWHEEL ":/img/img_res/cogwheel.svg"
#define ICON_COPY ":/img/img_res/copy.svg"
#define ICON_COPY_PLUS ":/img/img_res/copy_plus.svg"
#define ICON_EXIT ":/img/img_res/exit.svg"
#define ICON_FIND ":/img/img_res/find.svg"
#define ICON_FOLDER ":/img/img_res/folder.svg"
#define ICON_HEART ":/img/img_res/heart.svg"
#define ICON_INFO ":/img/img_res/info.svg"
#define ICON_MAGNIFY ":/img/img_res/magnify.svg"
#define ICON_MINIFY ":/img/img_res/minify.svg"
#define ICON_SCROLL ":/img/img_res/scroll.svg"

#include "AppConfig.h"
#include "TabBible.h"
#include "TabSearch.h"
#include "TabCompare.h"
#include "TabDictionary.h"
#include "TabNotes.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                        QWidget *parent);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QTabWidget *ui_TabWidget_Main;

    TabBible *ui_TabBible;
    TabSearch *ui_TabSearch;
    TabCompare *ui_TabCompare;
    TabDictionary *ui_TabDictionary;
    TabNotes *ui_TabFavorites;

    QLabel *ui_Label_Status;

    QMenu *ui_Menu_Edit;
    QMenu *ui_Menu_File;
    QMenu *ui_Menu_Help;
    QMenu *ui_Menu_Import;
    QMenu *ui_Menu_Language;
    QMenu *ui_Menu_Options;
    QMenu *ui_Menu_Statistics;
    QMenu *ui_Menu_View;

    QAction *ui_Act_About;
    QAction *ui_Act_AboutQt;
    QAction *ui_Act_Back;
    QAction *ui_Act_CommonRareWords;
    QAction *ui_Act_Copy;
    QAction *ui_Act_CopyWithReference;
    QAction *ui_Act_Exit;
    QAction *ui_Act_Find;
    QAction *ui_Act_Forward;
    QAction *ui_Act_DecreaseFontSize;
    QAction *ui_Act_IncreaseFontSize;
    QAction *ui_Act_ModuleInfo;
    QAction *ui_Act_MySwordModule;
    QAction *ui_Act_OpenModule;
    QAction *ui_Act_Preferences;
    QAction *ui_Act_SelectAll;
    QAction *ui_Act_ShowHelp;
    QAction *ui_Act_TheWordModule;
    QAction *ui_Act_WordFrequency;

    AppConfig *m_pConfig;

    QTranslator m_appTs;
    QStringList m_shortBookNames;
    QStringList m_bookNames;
    QFont m_currentFont;
    QSqlDatabase m_verseData;
    QSqlDatabase m_xrefData;
    QString m_executionPath;
    QList<QAction *> m_langActions;
    QMap<QString, QString> m_languages;
    QList<qbv::Module> m_modules;
    bool m_modulesFound;

    QHash<QString, QMenu *> m_menus;
    QHash<QString, QAction *> m_actions;

    QHash<int, AbstractTab *> m_mainTabs;

    QTranslator *m_pTsApp;
    QTranslator *m_pTsQt;

    void LoadVerseData();
    void LoadCrossReferences();
    void CheckModulePaths();

    void SetLanguage(const QString &lang);
    void CloseDatabase(QSqlDatabase &db);
    void ConnectSignals();
    void CreateMenuBar();
    void InitializeBibleTab();
    void LoadModules();
    void PopulateShortBookNames();
    void PopulateBookNames();
    QStringList GetModulePaths(const QString &dirPath);
    bool LoadModule(const QString &filePath);

    void SaveSettings();
    void SetGlobalFont(const QFont &font);
    void SetUiTexts(bool skipBibleTab = false);
    void SetWindowGeometry();

    void UpdateFromSettings();

    void OnAbout();
    void OnAboutQt();
    void OnAddToNotes(qbv::Location loc);
    void onBack();
    void OnCommonRareWords();
    void onCopy();
    void onCopyWithReference();
    void OnDecreaseFontSize();
    void OnExit();
    void onFind();
    void onForward();
    void onImportMySwordModule();
    void onImportTheWordModule();
    void onIncreaseFontSize();
    void onLanguage();
    void OnModuleInfo();
    void onOpenModule();
    void onPreferences();
    void onSelectAll();
    void onShowHelp();
    void onWordFrequency();

    void OnMainTabChanged(int index);

    void OnSearchReferenceClicked(int book, int chapter, int verse);
    void OnCompareReferenceClicked(int idx, int book, int chapter, int verse);
};

#endif // MAINWINDOW_H

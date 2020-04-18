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

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                        QWidget *parent);

signals:

public slots:
    void onMainTabChanged(int index);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QTabWidget *ui_TabWidget_Main;

    TabBible *ui_TabBible;
    TabSearch *ui_TabSearch;
    TabCompare *ui_TabCompare;

    QLabel *ui_Label_Status;

    QMenu *ui_Menu_Edit;
    QMenu *ui_Menu_File;
    QMenu *ui_Menu_Help;
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
    QAction *ui_Act_OpenModule;
    QAction *ui_Act_Preferences;
    QAction *ui_Act_SelectAll;
    QAction *ui_Act_ShowHelp;
    QAction *ui_Act_WordFrequency;

    AppConfig *m_config;

    QTranslator m_appTs;
    QStringList m_bookNames;
    QFont m_currentFont;
    QSqlDatabase m_verseData;
    QString m_executionPath;
    QList<QAction *> m_langActions;
    QMap<QString, QString> m_languages;
    QList<Module> m_modules;
    bool m_modulesFound;

    QHash<QString, QMenu *> m_menus;
    QHash<QString, QAction *> m_actions;

    QHash<int, AbstractTab *> m_mainTabs;

    QTranslator *m_pTsApp;
    QTranslator *m_pTsQt;

    void loadVerseData();
    void checkModulePaths();

    void changeLanguage(const QString &lang);
    void closeDatabase(QSqlDatabase &db);
    void connectSignals();
    void createMenuBar();
    void initializeBibleTab();
    void loadModuleData();
    void populateBookNames();
    QStringList getModulePaths(const QString &dirPath);
    bool loadBibleModule(const QString &filePath);

    void setUiTexts();
    void setWindowGeometry();

    void updateFromSettings();

    void onAbout();
    void onAboutQt();
    void onBack();
    void onCommonRareWords();
    void onCopy();
    void onCopyWithReference();
    void onDecreaseFontSize();
    void onExit();
    void onFind();
    void onForward();
    void onIncreaseFontSize();
    void onLanguage();
    void onModuleInfo();
    void onOpenModule();
    void onPreferences();
    void onSelectAll();
    void onShowHelp();
    void onWordFrequency();
};

#endif // MAINWINDOW_H

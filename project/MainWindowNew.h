#ifndef MAINWINDOWNEW_H
#define MAINWINDOWNEW_H

#define DEFAULT_WIN_WIDTH 920
#define DEFAULT_WIN_HEIGHT 600
#define MIN_WIN_WIDTH 720
#define MIN_WIN_HEIGHT 480

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
#include "DatabaseService.h"
#include "TabBible.h"
#include "TabBibleNew.h"
#include "TabSearch.h"
#include "TabSearchNew.h"
#include "TabCompare.h"
#include "TabCompareNew.h"
#include "TabDictionary.h"
#include "TabNotesNew.h"

class MainWindowNew : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindowNew(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                           QWidget *parent = nullptr);

private:
    QTabWidget *ui_TabWidget_Main;

    TabBibleNew *ui_TabBible;
    TabSearchNew *ui_TabSearch;
    TabCompareNew *ui_TabCompare;
    TabDictionary *ui_TabDictionary;
    TabNotesNew *ui_TabNotes;

    QMenu *ui_Menu_File;
    QAction *ui_Act_AddModule;
    QAction *ui_Act_ModuleInfo;
    QMenu *ui_Menu_Import;
    QAction *ui_Act_MySwordModule;
    QAction *ui_Act_TheWordModule;
    QAction *ui_Act_Exit;

    QLabel *ui_Label_Status;

    const QString *m_pAppDir;
    const QString m_dataDir;
    AppConfig *m_pConfig;
    QTranslator *m_pTsApp;
    QTranslator *m_pTsQt;

    qbv::DatabaseService m_databaseService;

    void CreateMenuBar();
    void SetWindowGeometry();
    void SetUiTexts();
    void ConnectSingals();

    void OnOpenModule();
    void OnModuleInfo();
    void OnImportMySwordModule();
    void OnImportTheWordModule();

    void OnAddNoteRequested(qbv::Location loc);

    void OnTabIndexChanged(int idx);
    void OnExit();

    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOWNEW_H

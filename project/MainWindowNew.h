#ifndef MAINWINDOWNEW_H
#define MAINWINDOWNEW_H

#define DEFAULT_WIN_WIDTH 920
#define DEFAULT_WIN_HEIGHT 600
#define MIN_WIN_WIDTH 720
#define MIN_WIN_HEIGHT 480

#include "AppConfig.h"
#include "DatabaseService.h"
#include "TabBible.h"
#include "TabBibleNew.h"
#include "TabSearch.h"
#include "TabCompare.h"
#include "TabDictionary.h"
#include "TabNotes.h"

class MainWindowNew : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindowNew(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                           QWidget *parent = nullptr);

private:
    QTabWidget *ui_TabWidget_Main;

    TabBibleNew *ui_TabBibleNew;
    TabSearch *ui_TabSearch;
    TabCompare *ui_TabCompare;
    TabDictionary *ui_TabDictionary;
    TabNotes *ui_TabFavorites;

    QMenu *ui_Menu_File;

    QLabel *ui_Label_Status;

    const QString *m_pAppDir;
    AppConfig *m_pConfig;
    QTranslator *m_pTsApp;
    QTranslator *m_pTsQt;

    qbv::DatabaseService m_databaseService;

    void SetWindowGeometry();

    void SetUiTexts();

    void OnExit();

    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOWNEW_H

#include "MainWindowNew.h"

#include "DialogImport.h"

MainWindowNew::MainWindowNew(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                             QWidget *parent) :
    QMainWindow(parent),
    m_pAppDir(&appDir),
    m_dataDir(appDir + "/Data/"),
    m_pConfig(&config),
    m_pTsApp(&appTs),
    m_pTsQt(&qtTs),
    m_databaseService(m_dataDir, config)
{

    ui_TabBible = new TabBibleNew(config, m_databaseService);
    ui_TabSearch = new TabSearchNew(config, m_databaseService);
    ui_TabCompare = new TabCompareNew(config, m_databaseService);

    ui_TabNotes = new TabNotesNew(config, m_databaseService);

    ui_TabWidget_Main = new QTabWidget;
    ui_TabWidget_Main->addTab(ui_TabBible, nullptr);
    ui_TabWidget_Main->addTab(ui_TabSearch, nullptr);
    ui_TabWidget_Main->addTab(ui_TabCompare, nullptr);
    ui_TabWidget_Main->addTab(new QWidget, nullptr);
    ui_TabWidget_Main->addTab(ui_TabNotes, nullptr);


    QMainWindow::setCentralWidget(ui_TabWidget_Main);

    ui_Label_Status = new QLabel;
    QMainWindow::statusBar()->addWidget(ui_Label_Status);



    SetWindowGeometry();
    CreateMenuBar();
    ConnectSingals();
    SetUiTexts();

    m_languages.insert("English", "EN");
    m_languages.insert("español", "ES");
    m_languages.insert("polski", "PL");


    ui_TabBible->Initialize();
    ui_TabBible->SetTabIndexFromConfig();
    ui_TabBible->SetLocationFromConfig();
    ui_TabBible->SetFontFromConfig();
}

void MainWindowNew::CreateMenuBar()
{
    ui_Menu_File = menuBar()->addMenu(QString());
    ui_Act_AddModule = ui_Menu_File->addAction(QIcon(ICON_FOLDER), nullptr,
                                                this, &MainWindowNew::OnOpenModule, QKeySequence::Open);
    ui_Act_ModuleInfo = ui_Menu_File->addAction(nullptr, this, &MainWindowNew::OnModuleInfo, QKeySequence("Ctrl+I"));
    ui_Menu_Import = ui_Menu_File->addMenu(QString());
    ui_Act_MySwordModule = ui_Menu_Import->addAction(nullptr, this, &MainWindowNew::OnImportMySwordModule);

    ui_Act_TheWordModule = ui_Menu_Import->addAction(nullptr, this, &MainWindowNew::OnImportTheWordModule);
    ui_Menu_File->addSeparator();
    ui_Act_Exit = ui_Menu_File->addAction(QIcon(ICON_EXIT), nullptr,
                                          this, &MainWindowNew::OnExit, QKeySequence("Ctrl+Q"));

    ui_Menu_Options = menuBar()->addMenu(QString());
    ui_Act_Preferences = ui_Menu_Options->addAction(QIcon(ICON_COGWHEEL), nullptr, this, &MainWindowNew::OnPreferences);
    ui_Menu_Language = ui_Menu_Options->addMenu(QIcon(ICON_BUBBLE), QString());
    QStringList languages = { "English", "español", "polski" };
    for (const QString &language : languages) {
        QAction *action = ui_Menu_Language->addAction(language, this, &MainWindowNew::OnLanguage);
        action->setCheckable(true);
        m_langActions.append(action);
    }
}

void MainWindowNew::SetWindowGeometry()
{
    QMainWindow::setMinimumSize(QSize(MIN_WIN_WIDTH, MIN_WIN_HEIGHT));
    if (m_pConfig->general.window_geometry.isEmpty()) {
        QWidget::resize(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);
    } else {
        QWidget::restoreGeometry(m_pConfig->general.window_geometry);
    }
}

void MainWindowNew::SetUiTexts()
{
    ui_TabWidget_Main->setTabText(0, tr("Bible"));
    ui_TabWidget_Main->setTabText(1, tr("Search"));
    ui_TabWidget_Main->setTabText(2, tr("Compare"));
    ui_TabWidget_Main->setTabText(3, tr("Dictionary"));
    ui_TabWidget_Main->setTabText(4, tr("Notes"));

    ui_Menu_File->setTitle(tr("File"));
    ui_Act_AddModule->setText(tr("Add Bible Module"));
    ui_Act_ModuleInfo->setText(tr("Module Info"));
    ui_Menu_Import->setTitle(tr("Import..."));
    ui_Act_MySwordModule->setText(tr("MySword Module"));
    ui_Act_TheWordModule->setText(tr("TheWord Module"));
    ui_Act_Exit->setText(tr("Exit"));

    ui_Menu_Options->setTitle(tr("Options"));
    ui_Act_Preferences->setText(tr("Preferences"));
    ui_Menu_Language->setTitle(tr("Language"));
}

void MainWindowNew::ConnectSingals()
{
    QObject::connect(ui_TabWidget_Main, QOverload<int>::of(&QTabWidget::currentChanged),
                     [=] (int idx) { OnTabIndexChanged(idx); } );
    QObject::connect(ui_TabBible, QOverload<qbv::Location>::of(&TabBibleNew::AddNoteRequested),
                     [=] (qbv::Location loc) { OnAddNoteRequested(loc); } );

    QObject::connect(ui_TabSearch, QOverload<QString, qbv::Location>::of(&TabSearchNew::ResultReferenceClicked),
                     [=] (QString name, qbv::Location loc) { OnGoToVerseRequested(name, loc, true); } );
    QObject::connect(ui_TabSearch, QOverload<QString>::of(&TabSearchNew::StatusMsgSet),
                     [=] (QString msg) { ui_Label_Status->setText(msg); } );

    QObject::connect(ui_TabCompare, QOverload<QString, qbv::Location>::of(&TabCompareNew::BibleNameClicked),
                     [=] (QString name, qbv::Location loc) { OnGoToVerseRequested(name, loc, false); } );
}

void MainWindowNew::SetLanguage(const QString &lang)
{
    for (QAction *action : m_langActions) {
        action->setChecked(action->text() == lang);
    }
    if (m_languages[lang] != m_pConfig->general.language) {
        m_pConfig->general.language = m_languages[lang];
        if (m_pConfig->general.language != "EN") {
            m_pTsApp->load(m_pConfig->general.language.toLower(), m_dataDir % "Lang");
            qApp->installTranslator(m_pTsApp);
            m_pTsQt->load("qt_" + m_pConfig->general.language.toLower(), m_dataDir % "Lang");
            qApp->installTranslator(m_pTsQt);
        } else {
            qApp->removeTranslator(m_pTsApp);
            qApp->removeTranslator(m_pTsQt);
        }
       m_databaseService.PopulateBookNames();
       m_databaseService.PopulateShortBookNames();
       ui_TabBible->ReloadBookNames();
       SetUiTexts();
    }
}

void MainWindowNew::OnOpenModule()
{

}

void MainWindowNew::OnModuleInfo()
{

}

void MainWindowNew::OnImportMySwordModule()
{
    DialogImport importDlg(true, m_databaseService.DirBibles());
    if (importDlg.exec()) {
        for (const QString &file : importDlg.getImportedFiles()) {

        }
    }
}

void MainWindowNew::OnImportTheWordModule()
{

}

void MainWindowNew::OnPreferences()
{

}

void MainWindowNew::OnLanguage()
{
    const QString lang = qobject_cast<QAction *>(QObject::sender())->text();
    SetLanguage(lang);
}

void MainWindowNew::OnGoToVerseRequested(const QString &name, qbv::Location loc, bool changeVerse2)
{
    int idx = m_databaseService.IndexForBibleShortName(name);
    ui_TabWidget_Main->setCurrentIndex(0);
    int verse = loc.verse1;
    loc.verse1 = 1;
    if (changeVerse2) loc.verse2 = m_databaseService.GetNumVerses(loc.book, loc.chapter);
    ui_TabBible->SetLocation(loc, false);
    ui_TabBible->SetBibleIndex(idx);
    ui_TabBible->UpdatePassageBrowser(idx, loc);
    ui_TabBible->HighlightBlock(idx, verse - 1);
}

void MainWindowNew::OnAddNoteRequested(qbv::Location loc)
{
    ui_TabWidget_Main->setCurrentIndex(4);
    if (!ui_TabNotes->IsInitialized()) {
        ui_TabNotes->Initialize();
    }
    ui_TabNotes->AddToNotes(loc);
}

void MainWindowNew::OnTabIndexChanged(int idx)
{
    switch (idx) {
        case 0:
            ui_Label_Status->setText(ui_TabBible->LastStatusMsg());
            break;
        case 1:
            if (!ui_TabSearch->IsInitialized()) {
                ui_TabSearch->Initialize();
            }
            ui_TabSearch->SetFocusAndSelectAll();
            ui_Label_Status->setText(ui_TabSearch->LastStatusMsg());
            break;
        case 2:
            if (!ui_TabCompare->IsInitialized()) {
                ui_TabCompare->Initialize();
                ui_TabCompare->SetFontFromConfig();
            }
            ui_Label_Status->setText(ui_TabCompare->LastStatusMsg());
            break;
        case 3:
            break;
        case 4:
            if (!ui_TabNotes->IsInitialized()) {
                ui_TabNotes->Initialize();
            }
            ui_Label_Status->setText(ui_TabNotes->LastStatusMsg());
            break;
        default:
            QMessageBox::critical(this, tr("Error"), tr("Specified index not found."));
            break;
    }
}

/*!
 * \brief Triggers the <i>closeEvent</i> by closing the MainWindow and shuts down the program.
 */
void MainWindowNew::OnExit()
{
    QWidget::close();
    QCoreApplication::quit();
}

void MainWindowNew::closeEvent(QCloseEvent *event)
{
    ui_TabBible->SaveTabIndexToConfig();
    ui_TabBible->SaveLocationToConfig();
    m_pConfig->save();
    event->accept();
}

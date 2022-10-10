#include "MainWindowNew.h"

MainWindowNew::MainWindowNew(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                             QWidget *parent) :
    QMainWindow(parent),
    m_pAppDir(&appDir),
    m_pConfig(&config),
    m_pTsApp(&appTs),
    m_pTsQt(&qtTs),
    m_databaseService(appDir + "/Data/", config)
{
    ui_TabBibleNew = new TabBibleNew(config, m_databaseService);
    ui_TabBibleNew->Initialize();


    ui_TabWidget_Main = new QTabWidget;
    ui_TabWidget_Main->addTab(ui_TabBibleNew, nullptr);

    QMainWindow::setCentralWidget(ui_TabWidget_Main);

    ui_Label_Status = new QLabel;
    QMainWindow::statusBar()->addWidget(ui_Label_Status);



    SetWindowGeometry();
    CreateMenuBar();
    SetUiTexts();

    ui_TabBibleNew->SetLocationFromConfig();
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

    ui_Menu_File->setTitle(tr("File"));
    ui_Act_AddModule->setText(tr("Add Bible Module"));
    ui_Act_ModuleInfo->setText(tr("Module Info"));
    ui_Menu_Import->setTitle(tr("Import..."));
    ui_Act_MySwordModule->setText(tr("MySword Module"));
    ui_Act_TheWordModule->setText(tr("TheWord Module"));
    ui_Act_Exit->setText(tr("Exit"));

}

void MainWindowNew::OnOpenModule()
{

}

void MainWindowNew::OnModuleInfo()
{

}

void MainWindowNew::OnImportMySwordModule()
{

}

void MainWindowNew::OnImportTheWordModule()
{

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
    ui_TabBibleNew->SaveLocationToConfig();
    m_pConfig->save();
    event->accept();
}

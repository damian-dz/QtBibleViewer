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

    ui_Menu_File = menuBar()->addMenu(QString());

    SetWindowGeometry();
    SetUiTexts();

    ui_TabBibleNew->SetLocationFromConfig();
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
    m_pConfig->save();
    event->accept();
}

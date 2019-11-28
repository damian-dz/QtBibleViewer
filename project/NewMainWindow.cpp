#include "NewMainWindow.h"

#include "DialogInfo.h"
#include "DialogPreferences.h"

/*!
 * \brief Creates an instance of MainWindow.
 * \param appDir
 * \param config
 * \param appTs
 * \param qtTs
 * \param parent
 */
MainWindow::MainWindow(const QString &appDir, AppConfig &config, QTranslator &appTs, QTranslator &qtTs,
                       QWidget *parent) :
    QMainWindow(parent),
    ui_TabBible(nullptr),
    ui_TabSearch(nullptr),
    m_config(&config),
    m_executionPath(appDir),
    m_modulesFound(false),
    m_pTsApp(&appTs),
    m_pTsQt(&qtTs)
{
    setWindowGeometry();
    loadVerseData();
    checkModulePaths();
    loadModuleData();
    populateBookNames();

    int openTabIdx = 0;

    m_currentFont = QFont(m_config->fonts.browser_family, m_config->fonts.browser_size);

    ui_TabBible = new TabBible(m_verseData, m_bookNames, m_modules);
    ui_TabSearch = new TabSearch(m_bookNames, m_modules);

    ui_TabWidget_Main = new QTabWidget;
    ui_TabWidget_Main->addTab(ui_TabBible, nullptr);
    ui_TabWidget_Main->addTab(ui_TabSearch, nullptr);
    ui_TabWidget_Main->setCurrentIndex(openTabIdx);

    onMainTabChanged(openTabIdx);

    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->setContentsMargins(5, 5, 5, 5);
    mainHorLayout->addWidget(ui_TabWidget_Main);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainHorLayout);

    QMainWindow::setCentralWidget(mainWidget);

    connectSignals();
    createMenuBar();

    m_languages.insert("English", "EN");
    m_languages.insert("español", "ES");
    m_languages.insert("polski", "PL");

    for (QAction *action : m_langActions) {
        action->setChecked(m_languages[action->text()] == m_config->general.language);
    }

    ui_Label_Status = new QLabel;
    QMainWindow::statusBar()->addWidget(ui_Label_Status);
    QMainWindow::setWindowIcon(QIcon(ICON_SCROLL));

    setUiTexts();
}

/*!
 * \brief
 */
void MainWindow::checkModulePaths()
{
    const QString modulesDirPath = m_executionPath % "/App/modules";
    if (!QDir(modulesDirPath).exists()) {
        QMessageBox::critical(this, tr("Error"), tr("The directory /App/modules does not exist."));
        m_modulesFound = false;
    } else {
        QStringList localModulesPaths = getModulePaths(modulesDirPath);
        for (const QString &path : localModulesPaths) {
            if (!m_config->module_data.paths.contains(path) &&
                    !m_config->module_data.removed_paths.contains(path) &&
                    QFileInfo(path).exists()) {
                m_config->module_data.paths << path;
            }
        }
        if (m_config->module_data.paths.isEmpty()) {
            m_modulesFound = false;
            QMessageBox::critical(this, tr("Error"), tr("No Bible module found."));
        } else {
            for (int i = 0; i < m_config->module_data.paths.count(); ++i) {
                if (!QFileInfo(m_config->module_data.paths[i]).exists()) {
                    m_config->module_data.paths.removeAt(i--);
                }
            }
            m_modulesFound = true;
        }
    }
}

void MainWindow::changeLanguage(const QString &lang)
{
    for (QAction *action : m_langActions) {
        action->setChecked(action->text() == lang);
    }
    if (m_languages[lang] != m_config->general.language) {
        m_config->general.language = m_languages[lang];
        if (m_config->general.language != "EN") {
            m_pTsApp->load(m_config->general.language.toLower(), m_executionPath % "/App/lang");
            qApp->installTranslator(m_pTsApp);
            m_pTsQt->load("qt_" + m_config->general.language.toLower(), m_executionPath % "/App/lang");
            qApp->installTranslator(m_pTsQt);
        } else {
            qApp->removeTranslator(m_pTsApp);
            qApp->removeTranslator(m_pTsQt);
        }
        setUiTexts();
    }
}

void MainWindow::createMenuBar()
{
    ui_Menu_File = menuBar()->addMenu(QString());
    ui_Act_OpenModule = ui_Menu_File->addAction(QIcon(ICON_FOLDER), nullptr,
                                                this, &MainWindow::onOpenModule, QKeySequence::Open);
    ui_Act_ModuleInfo = ui_Menu_File->addAction(nullptr, this, &MainWindow::onModuleInfo, QKeySequence("Ctrl+I"));
    ui_Menu_File->addSeparator();
    ui_Act_Exit = ui_Menu_File->addAction(QIcon(ICON_EXIT), nullptr,
                                          this, &MainWindow::onExit, QKeySequence("Ctrl+Q"));

    ui_Menu_Edit = menuBar()->addMenu(QString());
    ui_Act_Back = ui_Menu_Edit->addAction(QIcon(ICON_ARROW_LEFT), nullptr, this, &MainWindow::onBack);
    ui_Act_Back->setDisabled(true);
    ui_Act_Forward = ui_Menu_Edit->addAction(QIcon(ICON_ARROW_RIGHT),
                                             nullptr, this, &MainWindow::onForward);
    ui_Act_Forward->setDisabled(true);
    ui_Menu_Edit->addSeparator();
    ui_Act_Copy = ui_Menu_Edit->addAction(QIcon(ICON_COPY), nullptr,
                                          this, &MainWindow::onCopy, QKeySequence::Copy);
    ui_Act_CopyWithReference = ui_Menu_Edit->addAction(QIcon(ICON_COPY_PLUS), nullptr,
                                                       this, &MainWindow::onCopyWithReference);
    ui_Act_SelectAll = ui_Menu_Edit->addAction(nullptr, this, &MainWindow::onSelectAll, QKeySequence::SelectAll);
    ui_Act_Find = ui_Menu_Edit->addAction(QIcon(ICON_FIND), nullptr, this, &MainWindow::onFind, QKeySequence::Find);

    ui_Menu_Statistics = menuBar()->addMenu(QString());
    ui_Act_WordFrequency = ui_Menu_Statistics->addAction(nullptr, this, &MainWindow::onWordFrequency);
    ui_Act_CommonRareWords = ui_Menu_Statistics->addAction(nullptr, this, &MainWindow::onCommonRareWords);

    ui_Menu_Options = menuBar()->addMenu(QString());
    ui_Act_Preferences = ui_Menu_Options->addAction(QIcon(ICON_COGWHEEL), nullptr, this, &MainWindow::onPreferences);
    ui_Menu_Language = ui_Menu_Options->addMenu(QIcon(ICON_BUBBLE), QString());
    QStringList languages = { "English", "español", "polski" };
    for (const QString &language : languages) {
        QAction *action = ui_Menu_Language->addAction(language, this, &MainWindow::onLanguage);
        action->setCheckable(true);
        m_langActions.append(action);
    }

    ui_Menu_View = menuBar()->addMenu(QString());
    ui_Act_IncreaseFontSize = ui_Menu_View->addAction(QIcon(ICON_MAGNIFY), nullptr, this,
                                                      &MainWindow::onIncreaseFontSize, QKeySequence::ZoomIn);
    ui_Act_DecreaseFontSize = ui_Menu_View->addAction(QIcon(ICON_MINIFY), nullptr, this,
                                                      &MainWindow::onDecreaseFontSize, QKeySequence::ZoomOut);

    ui_Menu_Help = menuBar()->addMenu(QString());
    ui_Act_ShowHelp = ui_Menu_Help->addAction(nullptr, this, &MainWindow::onShowHelp, QKeySequence::HelpContents);
    ui_Act_About = ui_Menu_Help->addAction(QIcon(ICON_INFO), nullptr, this, &MainWindow::onAbout);
    ui_Act_AboutQt = ui_Menu_Help->addAction(nullptr, this, &MainWindow::onAboutQt);
}

void MainWindow::loadVerseData()
{
    const QString verseDataPath = m_executionPath % "/App/data/verse_data.bblv";
    if (QFileInfo(verseDataPath).exists()) {
        m_verseData = QSqlDatabase::addDatabase("QSQLITE", "Counters");
        m_verseData.setDatabaseName(verseDataPath);
        if (!m_verseData.open()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open the verse data file."));
        }
    } else {
         QMessageBox::critical(this, tr("Error"), tr("The verse data file not found."));
    }

}

QStringList MainWindow::getModulePaths(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList filters("*.bbl.mybible");
    dir.setNameFilters(filters);
    QFileInfoList moduleList = dir.entryInfoList();
    QStringList modulePathList;
    for (const QFileInfo &file : moduleList) {
        modulePathList << file.absoluteFilePath();
    }
    return modulePathList;
}

bool MainWindow::loadBibleModule(const QString &filePath)
{
    QSqlDatabase dbBbl = QSqlDatabase::addDatabase("QSQLITE", filePath);
    dbBbl.setDatabaseName(filePath);
    dbBbl.open();
    QSqlQuery query(dbBbl);
    QString moduleName;
    bool hasOldTestament = false;
    bool hasStrong = false;
    QString command = "SELECT Abbreviation, OT, Strong FROM Details";
    if (query.exec(command)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            moduleName = record.value(0).toString();
            hasOldTestament = record.value(1).toBool();
        }
    }
    command = "SELECT Scripture FROM Bible";
    if (query.exec(command)) {
        if (query.next()) {
            hasStrong = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
        }
    }
    bool containsModule = false;
    for (const ModuleData &md : m_modules) {
        if (filePath == md.filePath) {
            containsModule = true;
            QMessageBox::critical(this, tr("Error"), tr("This module is already open."));
            break;
        }
    }
    if (!containsModule) {
        m_modules.append({ dbBbl, moduleName, filePath, hasOldTestament, hasStrong });
       // ui_TabBible->addModule(moduleName, filePath, hasOldTestament, hasStrong);
    }
    return !containsModule;
}


void MainWindow::onExit()
{

}

void MainWindow::onFind()
{

}

void MainWindow::onForward()
{

}

void MainWindow::onIncreaseFontSize()
{

}

void MainWindow::onLanguage()
{
    const QString lang = qobject_cast<QAction *>(QObject::sender())->text();
    changeLanguage(lang);
}

void MainWindow::onModuleInfo()
{
    if (!m_modules.isEmpty()) {
        DialogInfo infoDlg(m_modules[ui_TabBible->getCurrentIndex()].database,
                           QApplication::font(),
                           QPixmap(),
                           m_config->appearance.use_background_image);
        infoDlg.exec();
    } else {
        QMessageBox::information(this, tr("No Modules"), tr("There are no modules currently avaiable."));
    }
}

void MainWindow::onOpenModule()
{
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Open MYBIBLE Module"), "/",
                tr("MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)"));
    if (!filename.isEmpty()) {
//        if (ui_Bib_TabWidget_Modules->tabText(0) == "No module found") {
//            ui_Bib_TabWidget_Modules->removeTab(0);
//        }
//        if (!m_pConfig->module_data.paths.contains(filename)) {
//            if (loadBibleModule(filename)) {
//                m_pConfig->module_data.paths << filename;
//                ui_Bib_TabWidget_Modules->setCurrentIndex(ui_Bib_TabWidget_Modules->count() - 1);
//                if (m_pConfig->module_data.removed_paths.contains(filename)) {
//                    m_pConfig->module_data.removed_paths.removeAt(
//                                m_pConfig->module_data.removed_paths.indexOf(filename));
//                }
//            }
//        } else {
//            QMessageBox::critical(this, tr("Error"), tr("The selected file is already open."));
//        }
    }
}

void MainWindow::onPreferences()
{
    qDebug() << m_languages.value(m_config->general.language);
    DialogPreferences dlgPreferences(m_config, m_languages.keys(), m_languages.key(m_config->general.language),
                                     m_currentFont);
    dlgPreferences.setWindowIcon(QIcon(ICON_COGWHEEL));
    if (dlgPreferences.exec()) {
        changeLanguage(dlgPreferences.getLanguage());
    }
}

void MainWindow::onSelectAll()
{

}

void MainWindow::onShowHelp()
{

}

void MainWindow::onWordFrequency()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
//        saveSettings();
//        for (ModuleData &md : m_modules) {
//            closeDatabase(md.database);
//        }
//        closeDatabase(m_dbCntr);
//        closeDatabase(m_dbXRef);
//        closeDatabase(m_dbDict);
//        event->accept();
}


void MainWindow::connectSignals()
{
    QObject::connect(ui_TabWidget_Main, QOverload<int>::of(&QTabWidget::currentChanged),
                     [=] (int index) { onMainTabChanged(index); } );
}

void MainWindow::initializeBibleTab()
{
    ui_TabBible->initialize();
    for (const ModuleData &md : m_modules) {
        ui_TabBible->addModule(md.name, md.filePath, md.hasOldTestament, md.hasStrong);
    }

    ui_TabBible->setPassageTextBrowserFont(m_currentFont);
    ui_TabBible->selectModule(m_config->module_data.index);
    ui_TabBible->selectPassage(m_config->module_data.last_passage[0].toInt() - 1,
                               m_config->module_data.last_passage[1].toInt() - 1,
                               m_config->module_data.last_passage[2].toInt() - 1,
                               m_config->module_data.last_passage[3].toInt() - 1);

}

void MainWindow::loadModuleData()
{
    for (const QString &filePath: m_config->module_data.paths) {
        loadBibleModule(filePath);
    }
}

void MainWindow::populateBookNames()
{
    if (!m_bookNames.isEmpty()) {
        m_bookNames.clear();
    }
    m_bookNames << tr("Genesis")
                << tr("Exodus")
                << tr("Leviticus")
                << tr("Numbers")
                << tr("Deuteronomy")
                << tr("Joshua")
                << tr("Judges")
                << tr("Ruth")
                << tr("1 Samuel")
                << tr("2 Samuel")
                << tr("1 Kings")
                << tr("2 Kings")
                << tr("1 Chronicles")
                << tr("2 Chronicles")
                << tr("Ezra")
                << tr("Nehemiah")
                << tr("Esther")
                << tr("Job")
                << tr("Psalms")
                << tr("Proverbs")
                << tr("Ecclesiastes")
                << tr("Song of Solomon")
                << tr("Isaiah")
                << tr("Jeremiah")
                << tr("Lamentations")
                << tr("Ezekiel")
                << tr("Daniel")
                << tr("Hosea")
                << tr("Joel")
                << tr("Amos")
                << tr("Obadiah")
                << tr("Jonah")
                << tr("Micah")
                << tr("Nahum")
                << tr("Habakkuk")
                << tr("Zephaniah")
                << tr("Haggai")
                << tr("Zechariah")
                << tr("Malachi")
                << tr("Matthew")
                << tr("Mark")
                << tr("Luke")
                << tr("John")
                << tr("Acts")
                << tr("Romans")
                << tr("1 Corinthians")
                << tr("2 Corinthians")
                << tr("Galatians")
                << tr("Ephesians")
                << tr("Philippians")
                << tr("Colossians")
                << tr("1 Thessalonians")
                << tr("2 Thessalonians")
                << tr("1 Timothy")
                << tr("2 Timothy")
                << tr("Titus")
                << tr("Philemon")
                << tr("Hebrews")
                << tr("James")
                << tr("1 Peter")
                << tr("2 Peter")
                << tr("1 John")
                << tr("2 John")
                << tr("3 John")
                << tr("Jude")
                << tr("Revelation");
}

void MainWindow::setUiTexts()
{
    ui_TabWidget_Main->setTabText(0, tr("Bible"));
    ui_TabWidget_Main->setTabText(1, tr("Search"));

    ui_Menu_File->setTitle(tr("File"));
    ui_Act_OpenModule->setText(tr("Open Bible Module"));
    ui_Act_ModuleInfo->setText(tr("Module Info"));
    ui_Act_Exit->setText(tr("Exit"));

    ui_Menu_Edit->setTitle(tr("Edit"));
    ui_Act_Back->setText(tr("Back"));
    ui_Act_Forward->setText(tr("Forward"));
    ui_Act_Copy->setText(tr("Copy"));
    ui_Act_CopyWithReference->setText(tr("Copy with Reference"));
    ui_Act_SelectAll->setText(tr("Select All"));
    ui_Act_Find->setText(tr("Find"));

    ui_Menu_Statistics->setTitle(tr("Statistics"));
    ui_Act_WordFrequency->setText(tr("Word Frequency"));
    ui_Act_CommonRareWords->setText(tr("Common/Rare Words"));

    ui_Menu_Options->setTitle(tr("Options"));
    ui_Act_Preferences->setText(tr("Preferences"));
    ui_Menu_Language->setTitle(tr("Language"));

    ui_Menu_View->setTitle(tr("View"));
    ui_Act_IncreaseFontSize->setText(tr("Increase Font Size"));
    ui_Act_DecreaseFontSize->setText(tr("Decrease Font Size"));

    ui_Menu_Help->setTitle(tr("Help"));
    ui_Act_ShowHelp->setText(tr("Show Help"));
    ui_Act_About->setText(tr("About"));
    ui_Act_AboutQt->setText(tr("About Qt"));

    if (ui_TabBible->isInitialized()) {
        qDebug() << "ui_TabBible->setUiTexts()";
        ui_TabBible->setUiTexts();
    }
    if (ui_TabSearch->isInitialized()) {
        qDebug() << "ui_TabSearch->setUiTexts()";
        ui_TabSearch->setUiTexts();
    }
}

void MainWindow::setWindowGeometry()
{
    QMainWindow::setMinimumSize(QSize(720, 480));
    if (m_config->general.window_geometry.isEmpty()) {
        QMainWindow::resize(920, 600);
    } else {
        QWidget::restoreGeometry(m_config->general.window_geometry);
    }

}

void MainWindow::onAbout()
{
    QMessageBox infoMsgBox(QMessageBox::NoIcon, tr("About Qt Bible Viewer"),
                           tr("<p><b>Qt Bible Viewer</b> is an open-source application "
                              "currently developed by Damian Dzienniak. "
                              "The GitHub repositiory for the project can be found at "
                              "<a href='https://github.com/damian-dz/QtBibleViewer'> "
                              "github.com/damian-dz/QtBibleViewer</a>.</p>"));
    infoMsgBox.setWindowIcon(QIcon(ICON_INFO));
    infoMsgBox.exec();
}

void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::onBack()
{

}

void MainWindow::onCommonRareWords()
{

}

void MainWindow::onCopy()
{

}

void MainWindow::onCopyWithReference()
{

}

void MainWindow::onDecreaseFontSize()
{

}

void MainWindow::onMainTabChanged(int index)
{
    qDebug() << "onMainTabChanged()" << index;
    switch (index) {
    case 0:
        if (!ui_TabBible->isInitialized()) {
            initializeBibleTab();
        }
        break;
    case 1:
        if (!ui_TabSearch->isInitialized()) {
            ui_TabSearch->initialize();
        }
        break;
    default:
        QMessageBox::critical(this, tr("Error"), tr("Specified index not found."));
        break;
    }
}

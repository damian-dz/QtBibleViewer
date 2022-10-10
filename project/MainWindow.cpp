#include "MainWindow.h"

#include "DialogImport.h"
#include "DialogInfo.h"
#include "DialogPreferences.h"
#include "WidgetWordFrequency.h"
#include "WidgetCommonRareWords.h"
#include "Formatting.h"

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
    m_databaseService(appDir + "/Data/", config),
    m_pConfig(&config),
    m_executionPath(appDir),
    m_modulesFound(false),
    m_pTsApp(&appTs),
    m_pTsQt(&qtTs)
{
    PopulateBookNames();
    PopulateShortBookNames();
    LoadVerseData();
    LoadCrossReferences();
    CheckModulePaths();
    LoadModules();

    m_currentFont = QFont(m_pConfig->fonts.family, m_pConfig->fonts.size);

    //ui_TabBibleNew = new TabBibleNew(*m_pConfig, m_databaseService);
   // ui_TabBibleNew->Initialize();
    ui_TabBible = new TabBible(m_verseData, m_xrefData, m_bookNames, m_shortBookNames, m_modules, m_pConfig);
    ui_TabSearch = new TabSearch(m_bookNames, m_modules);
    ui_TabCompare = new TabCompare(m_bookNames, m_shortBookNames, m_modules, m_verseData);
    ui_TabDictionary = new TabDictionary;
    ui_TabFavorites = new TabNotes(m_executionPath % FAV_FILE_PATH, m_modules, m_bookNames);

    ui_TabWidget_Main = new QTabWidget;
    ui_TabWidget_Main->addTab(ui_TabBible, nullptr);
    ui_TabWidget_Main->addTab(ui_TabSearch, nullptr);
    ui_TabWidget_Main->addTab(ui_TabCompare, nullptr);
    ui_TabWidget_Main->addTab(ui_TabDictionary, nullptr);
    ui_TabWidget_Main->addTab(ui_TabFavorites, nullptr);
   // ui_TabWidget_Main->setCurrentIndex(0);
   // ui_TabWidget_Main->setFont()

    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->setContentsMargins(5, 5, 5, 5);
    mainHorLayout->addWidget(ui_TabWidget_Main);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainHorLayout);

    QMainWindow::setCentralWidget(mainWidget);

    ui_Label_Status = new QLabel;
    QMainWindow::statusBar()->addWidget(ui_Label_Status);
    QMainWindow::setWindowIcon(QIcon(ICON_SCROLL));

    m_languages.insert("English", "EN");
    m_languages.insert("español", "ES");
    m_languages.insert("polski", "PL");

    for (QAction *action : m_langActions) {
        action->setChecked(m_languages[action->text()] == m_pConfig->general.language);
    }
    qDebug() << "START";
    SetWindowGeometry();
    CreateMenuBar();
    SetUiTexts(true);
    ConnectSignals();

    OnMainTabChanged(0);
    qDebug() << "END";
}

void MainWindow::SetWindowGeometry()
{
    QMainWindow::setMinimumSize(QSize(MIN_WIN_WIDTH, MIN_WIN_HEIGHT));
    if (m_pConfig->general.window_geometry.isEmpty()) {
        QMainWindow::resize(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);
    } else {
        QWidget::restoreGeometry(m_pConfig->general.window_geometry);
    }
}

void MainWindow::LoadVerseData()
{
    const QString verseDataPath = m_executionPath % VERSE_DATA_FILE_PATH;
    if (QFileInfo(verseDataPath).exists()) {
        m_verseData = QSqlDatabase::addDatabase("QSQLITE", "VerseData");
        m_verseData.setDatabaseName(verseDataPath);
        if (!m_verseData.open()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open the verse data file."));
        }
    } else {
         QMessageBox::critical(this, tr("Error"), tr("The verse data file not found."));
    }
}

void MainWindow::LoadCrossReferences()
{
    const QString crossRefPath = m_executionPath % XREF_FILE_PATH;
    if (QFileInfo(crossRefPath).exists()) {
        m_xrefData = QSqlDatabase::addDatabase("QSQLITE", "CrossRefs");
        m_xrefData.setDatabaseName(crossRefPath);
        if (!m_xrefData.open()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open the cross-reference data file."));
        }
    } else {
         QMessageBox::critical(this, tr("Error"), tr("The verse data file not found."));
    }
}

void MainWindow::CheckModulePaths()
{
    const QString modulesDirPath = m_executionPath % APP_MODULES_DIR;
    if (!QDir(modulesDirPath).exists()) {
        QMessageBox::critical(this, tr("Error"), tr("The directory " APP_MODULES_DIR " does not exist."));
        m_modulesFound = false;
    } else {
        QStringList localModulesPaths = GetModulePaths(modulesDirPath);
        for (const QString &path : localModulesPaths) {
            if (!m_pConfig->module_data.paths.contains(path) &&
                !m_pConfig->module_data.removed_paths.contains(path) &&
                QFileInfo(path).exists()) {
                m_pConfig->module_data.paths << path;
            }
        }
        if (m_pConfig->module_data.paths.isEmpty()) {
            m_modulesFound = false;
            QMessageBox::critical(this, tr("Error"), tr("No Bible module found."));
        } else {
            for (int i = 0; i < m_pConfig->module_data.paths.count(); ++i) {
                if (!QFileInfo(m_pConfig->module_data.paths[i]).exists()) {
                    m_pConfig->module_data.paths.removeAt(i--);
                }
            }
            m_modulesFound = true;
        }
    }
}

void MainWindow::LoadModules()
{
    for (const QString &filePath: m_pConfig->module_data.paths) {
       // qDebug() << filePath;
        LoadModule(filePath);
    }
}



bool MainWindow::LoadModule(const QString &filePath)
{
    QSqlDatabase dbBbl = QSqlDatabase::addDatabase("QSQLITE", filePath);
    dbBbl.setDatabaseName(filePath);
    dbBbl.open();
    m_modules.append({ dbBbl });
    return true;
//    QSqlDatabase dbBbl = QSqlDatabase::addDatabase("QSQLITE", filePath);
//    dbBbl.setDatabaseName(filePath);
//    dbBbl.open();
//    QSqlQuery query(dbBbl);
//    QString moduleName;
//    bool hasOldTestament = false;
//    bool hasStrong = false;
//    QString command = "SELECT Abbreviation, OldTestament, Strong FROM Info";
//    if (query.exec(command)) {
//        if (query.next()) {
//            QSqlRecord record = query.record();
//            moduleName = record.value(0).toString();
//            qDebug() << "moduleName" << moduleName;
//            hasOldTestament = record.value(1).toBool();
//        }
//    }
//    command = "SELECT Scripture FROM Bible";
//    if (query.exec(command)) {
//        if (query.next()) {
//            hasStrong = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
//        }
//    }
//    bool containsModule = false;
//    for (const Module &module : m_modules) {
//        if (filePath == module.filePath) {
//            containsModule = true;
//            QMessageBox::critical(this, tr("Error"), tr("This module is already open."));
//            break;
//        }
//    }
//    if (!containsModule) {
//        m_modules.append({ dbBbl, moduleName, filePath, hasOldTestament, hasStrong });
//       // ui_TabBible->addModule(moduleName, filePath, hasOldTestament, hasStrong);
//    }
//    return !containsModule;
}



void MainWindow::PopulateBookNames()
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

void MainWindow::PopulateShortBookNames()
{
    if (!m_shortBookNames.isEmpty()) {
        m_shortBookNames.clear();
    }
    m_shortBookNames << tr("Gen")
                     << tr("Exo")
                     << tr("Lev")
                     << tr("Num")
                     << tr("Deu")
                     << tr("Jos")
                     << tr("Jdg")
                     << tr("Rut")
                     << tr("1Sa")
                     << tr("2Sa")
                     << tr("1Ki")
                     << tr("2Ki")
                     << tr("1Ch")
                     << tr("2Ch")
                     << tr("Ezr")
                     << tr("Neh")
                     << tr("Est")
                     << tr("Job")
                     << tr("Psa")
                     << tr("Pro")
                     << tr("Ecc")
                     << tr("Sol")
                     << tr("Isa")
                     << tr("Jer")
                     << tr("Lam")
                     << tr("Eze")
                     << tr("Dan")
                     << tr("Hos")
                     << tr("Joe")
                     << tr("Amo")
                     << tr("Oba")
                     << tr("Jon")
                     << tr("Mic")
                     << tr("Nah")
                     << tr("Hab")
                     << tr("Zep")
                     << tr("Hag")
                     << tr("Zec")
                     << tr("Mal")
                     << tr("Mat")
                     << tr("Mar")
                     << tr("Luk")
                     << tr("Joh")
                     << tr("Act")
                     << tr("Rom")
                     << tr("1Co")
                     << tr("2Co")
                     << tr("Gal")
                     << tr("Eph")
                     << tr("Phi")
                     << tr("Col")
                     << tr("1Th")
                     << tr("2Th")
                     << tr("1Ti")
                     << tr("2Ti")
                     << tr("Tit")
                     << tr("Phm")
                     << tr("Heb")
                     << tr("Jam")
                     << tr("1Pe")
                     << tr("2Pe")
                     << tr("1Jo")
                     << tr("2Jo")
                     << tr("3Jo")
                     << tr("Jud")
                     << tr("Rev");
}

void MainWindow::OnMainTabChanged(int index)
{
    qDebug() << "OnMainTabChanged()" << index;
    switch (index) {
        case 0:
            if (!ui_TabBible->IsInitialized()) {
                InitializeBibleTab();
            }
            ui_Label_Status->setText(ui_TabBible->GetLastStatusMsg());
            break;
        case 1:
            if (!ui_TabSearch->IsInitialized()) {
                ui_TabSearch->Initialize();
                ui_TabSearch->SetFont(m_currentFont);
            }
            ui_Label_Status->setText(ui_TabSearch->GetLastStatusMsg());
            break;
        case 2:
            if (!ui_TabCompare->IsInitialized()) {
                ui_TabCompare->Initialize();
                ui_TabCompare->SetFont(m_currentFont);
            }
            ui_Label_Status->setText(ui_TabCompare->GetLastStatusMsg());
            break;
        case 3:
            if (!ui_TabDictionary->IsInitialized()) {
                ui_TabDictionary->Initialize();
            }
            ui_Label_Status->setText(ui_TabDictionary->GetLastStatusMsg());
            break;
        case 4:
            if (!ui_TabFavorites->IsInitialized()) {
                ui_TabFavorites->Initialize();
            }
            ui_Label_Status->setText(ui_TabFavorites->GetLastStatusMsg());
            break;
        default:
            QMessageBox::critical(this, tr("Error"), tr("Specified index not found."));
            break;
    }
}

void MainWindow::OnSearchReferenceClicked(int book, int chapter, int verse)
{
    ui_TabWidget_Main->setCurrentIndex(0);
    int translationIdx = ui_TabSearch->GetTranslationIndex();
    ui_TabBible->SetActiveModule(translationIdx);
    ui_TabBible->SetNavigationLocation(book, chapter, true);
    ui_TabBible->HighlightVerse(verse);
}

void MainWindow::OnCompareReferenceClicked(int idx, int book, int chapter, int verse)
{
    ui_TabWidget_Main->setCurrentIndex(0);
    ui_TabBible->SetActiveModule(idx);
    ui_TabBible->SetNavigationLocation(book, chapter, true);
    ui_TabBible->HighlightVerse(verse);
}

void MainWindow::ConnectSignals()
{
    QObject::connect(ui_TabWidget_Main, QOverload<int>::of(&QTabWidget::currentChanged),
                     [=] (int index) { OnMainTabChanged(index); } );
    QObject::connect(ui_TabSearch, QOverload<int, int, int>::of(&TabSearch::ReferenceClicked),
                     [=] (int book, int chapter, int verse) { OnSearchReferenceClicked(book, chapter, verse); } );
    QObject::connect(ui_TabBible, QOverload<QString>::of(&TabBible::StatusMsgSet),
                     [=] (QString msg) { ui_Label_Status->setText(msg); } );

    QObject::connect(ui_TabBible, QOverload<qbv::Location>::of(&TabBible::AddToNotesRequested),
                     [=] (qbv::Location loc) { OnAddToNotes(loc); });
    QObject::connect(ui_TabSearch, QOverload<QString>::of(&TabSearch::StatusMsgSet),
                     [=] (QString msg) { ui_Label_Status->setText(msg); } );
    QObject::connect(ui_TabCompare, QOverload<int, int, int, int>::of(&TabCompare::TranslationNameClicked),
                     [=] (int idx, int book, int chapter, int verse)
                         { OnCompareReferenceClicked(idx, book, chapter, verse); } );
    QObject::connect(ui_TabCompare, QOverload<QString>::of(&TabCompare::StatusMsgSet),
                     [=] (QString msg) { ui_Label_Status->setText(msg); } );
}

void MainWindow::CreateMenuBar()
{
    ui_Menu_File = menuBar()->addMenu(QString());
    ui_Act_OpenModule = ui_Menu_File->addAction(QIcon(ICON_FOLDER), nullptr,
                                                this, &MainWindow::onOpenModule, QKeySequence::Open);
    ui_Act_ModuleInfo = ui_Menu_File->addAction(nullptr, this, &MainWindow::OnModuleInfo, QKeySequence("Ctrl+I"));
    ui_Menu_Import = ui_Menu_File->addMenu(QString());
    ui_Act_MySwordModule = ui_Menu_Import->addAction(nullptr, this, &MainWindow::onImportMySwordModule);

    ui_Act_TheWordModule = ui_Menu_Import->addAction(nullptr, this, &MainWindow::onImportTheWordModule);
    ui_Menu_File->addSeparator();
    ui_Act_Exit = ui_Menu_File->addAction(QIcon(ICON_EXIT), nullptr,
                                          this, &MainWindow::OnExit, QKeySequence("Ctrl+Q"));

    ui_Menu_Edit = menuBar()->addMenu(QString());
    ui_Act_Back = ui_Menu_Edit->addAction(QIcon(ICON_ARROW_LEFT), nullptr, this, &MainWindow::onBack);
    ui_Act_Back->setDisabled(true);
    ui_Act_Forward = ui_Menu_Edit->addAction(QIcon(ICON_ARROW_RIGHT),
                                             nullptr, this, &MainWindow::onForward);
    ui_Act_Forward->setDisabled(true);
    ui_Menu_Edit->addSeparator();
    ui_Act_Copy = ui_Menu_Edit->addAction(QIcon(ICON_COPY), nullptr,
                                          this, &MainWindow::onCopy);
    ui_Act_CopyWithReference = ui_Menu_Edit->addAction(QIcon(ICON_COPY_PLUS), nullptr,
                                                       this, &MainWindow::onCopyWithReference);
    ui_Act_SelectAll = ui_Menu_Edit->addAction(nullptr, this, &MainWindow::onSelectAll, QKeySequence::SelectAll);
    ui_Act_Find = ui_Menu_Edit->addAction(QIcon(ICON_FIND), nullptr, this, &MainWindow::onFind, QKeySequence::Find);

    ui_Menu_Statistics = menuBar()->addMenu(QString());
    ui_Act_WordFrequency = ui_Menu_Statistics->addAction(nullptr, this, &MainWindow::onWordFrequency);
    ui_Act_CommonRareWords = ui_Menu_Statistics->addAction(nullptr, this, &MainWindow::OnCommonRareWords);

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
                                                      &MainWindow::OnDecreaseFontSize, QKeySequence::ZoomOut);

    ui_Menu_Help = menuBar()->addMenu(QString());
    ui_Act_ShowHelp = ui_Menu_Help->addAction(nullptr, this, &MainWindow::onShowHelp, QKeySequence::HelpContents);
    ui_Act_About = ui_Menu_Help->addAction(QIcon(ICON_INFO), nullptr, this, &MainWindow::OnAbout);
    ui_Act_AboutQt = ui_Menu_Help->addAction(nullptr, this, &MainWindow::OnAboutQt);
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

void MainWindow::OnModuleInfo()
{
    if (!m_modules.isEmpty()) {
        DialogInfo infoDlg(m_modules[ui_TabBible->GetCurrentIndex()].translation,
                           QApplication::font(),
                           QPixmap(),
                           m_pConfig->appearance.use_background_image);
        infoDlg.exec();
    } else {
        QMessageBox::information(this, tr("No Modules"), tr("There are no modules currently avaiable."));
    }
}

/*!
 * \brief Triggers the <i>closeEvent</i> by closing the MainWindow and shuts down the program.
 */
void MainWindow::OnExit()
{
    QWidget::close();
    QCoreApplication::quit();
}

void MainWindow::SetLanguage(const QString &lang)
{
    for (QAction *action : m_langActions) {
        action->setChecked(action->text() == lang);
    }
    if (m_languages[lang] != m_pConfig->general.language) {
        m_pConfig->general.language = m_languages[lang];
        if (m_pConfig->general.language != "EN") {
            m_pTsApp->load(m_pConfig->general.language.toLower(), m_executionPath % APP_LANG_DIR);
            qApp->installTranslator(m_pTsApp);
            m_pTsQt->load("qt_" + m_pConfig->general.language.toLower(), m_executionPath % APP_LANG_DIR);
            qApp->installTranslator(m_pTsQt);
        } else {
            qApp->removeTranslator(m_pTsApp);
            qApp->removeTranslator(m_pTsQt);
        }
        PopulateBookNames();
        PopulateShortBookNames();
        ui_TabBible->ReloadBookNames();
        SetUiTexts();
    }
}

void MainWindow::CloseDatabase(QSqlDatabase &db)
{
    if (db.isOpen()) {
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(db.connectionName());
    }
}

QStringList MainWindow::GetModulePaths(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList filters("*.qbv");
    dir.setNameFilters(filters);
    QFileInfoList moduleList = dir.entryInfoList();
    QStringList modulePathList;
    for (const QFileInfo &file : moduleList) {
        modulePathList << file.absoluteFilePath();
       // qDebug() << file.absoluteFilePath();
    }
    return modulePathList;
}


void MainWindow::onFind()
{
    ui_TabBible->FindPhrase();
}

void MainWindow::onForward()
{

}

void MainWindow::onImportMySwordModule()
{
    qDebug() << "MainWindow::onImportMySwordModule()";
    DialogImport importDlg(true, m_executionPath % "/App/modules");
    if (importDlg.exec()) {
        for (const QString &file : importDlg.getImportedFiles()) {
            qDebug() << file;
            LoadModule(file);
            qbv::Module module = m_modules.last();
            // ui_TabBible->addModule(module.name(), module.filePath(), module.hasOT(), module.hasStrong());
            ui_TabBible->AddModule(module);
        }
    }
}

void MainWindow::onImportTheWordModule()
{
    DialogImport importDlg(false, m_executionPath % "/App/modules");
    importDlg.exec();
}

void MainWindow::onIncreaseFontSize()
{
    if (m_pConfig->fonts.size < 20) {
        QFont font(m_pConfig->fonts.family, m_pConfig->fonts.size + 1);
        SetGlobalFont(font);
        ++m_pConfig->fonts.size;
    }
}

void MainWindow::onLanguage()
{
    const QString lang = qobject_cast<QAction *>(QObject::sender())->text();
    SetLanguage(lang);
}

void MainWindow::onPreferences()
{
    qDebug() << m_languages.value(m_pConfig->general.language);
    DialogPreferences dlgPreferences(m_pConfig, m_languages.keys(), m_languages.key(m_pConfig->general.language),
                                     m_currentFont);
    dlgPreferences.setWindowIcon(QIcon(ICON_COGWHEEL));
    QString oldWindowStyle = m_pConfig->appearance.window_style;
    if (dlgPreferences.exec()) {
        dlgPreferences.UpdateSettings();
        SetLanguage(dlgPreferences.getLanguage());
        if (m_pConfig->appearance.window_style != oldWindowStyle) {
            qApp->setStyle(QStyleFactory::create(m_pConfig->appearance.window_style));
        }
        m_currentFont.setFamily(m_pConfig->fonts.family);
        m_currentFont.setPointSize(m_pConfig->fonts.size);
        UpdateFromSettings();
    }
}

void MainWindow::onSelectAll()
{
    ui_TabBible->SelectAllText();
}

void MainWindow::onShowHelp()
{

}

void MainWindow::onWordFrequency()
{
    int idx = ui_TabBible->GetCurrentIndex();
    WidgetWordFrequency *widgetWordFrequency = new WidgetWordFrequency(
                m_modules[idx].translation,
                m_bookNames,
                m_shortBookNames,
                m_pConfig->appearance.chart_animation);
    widgetWordFrequency->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        SaveSettings();
        for (qbv::Module &module : m_modules) {
            CloseDatabase(module.translation);
        }
        CloseDatabase(m_verseData);
        CloseDatabase(m_xrefData);
//      closeDatabase(m_dbXRef);
//      closeDatabase(m_dbDict);
        event->accept();
}

void MainWindow::SaveSettings()
{
    m_pConfig->general.window_geometry = QWidget::saveGeometry();
    m_pConfig->general.window_state = QMainWindow::saveState();
    m_pConfig->general.splitter_layout = ui_TabBible->GetSplitterLayout();
    m_pConfig->module_data.index = m_modulesFound ? ui_TabBible->GetCurrentIndex() : -1;
    m_pConfig->module_data.last_passage = QStringList(
                {
                    QString::number(ui_TabBible->GetSelectedBook()),
                    QString::number(ui_TabBible->GetSelectedChapter()),
                    QString::number(ui_TabBible->GetSelectedVerseFrom()),
                    QString::number(ui_TabBible->GetSelectedVerseTo()),
                });
    m_pConfig->fonts.family = m_currentFont.family();
   // m_pConfig->fonts.size = m_currentFont.pointSize();
    m_pConfig->save();
}




void MainWindow::InitializeBibleTab()
{
    qDebug() << "MainWindow::InitializeBibleTab()";
    ui_TabBible->Initialize();
    for (const qbv::Module &module : m_modules) {
        ui_TabBible->AddModule(module);
    }

    ui_TabBible->SetPassageBrowserFont(m_currentFont);
    ui_TabBible->SetPassageBrowserHighlightColor(m_pConfig->appearance.verse_highlight_color);

    if (m_pConfig->module_data.last_passage.count() < 4) {
        m_pConfig->module_data.last_passage = QStringList({ "1", "1", "1", "31" });
    }
    ui_TabBible->SelectPassageFromConfig();
    ui_TabBible->SetActiveModule(m_pConfig->module_data.index, true);
    ui_TabBible->LoadPassageInCurrentTab();
}

void MainWindow::SetUiTexts(bool skipBibleTab)
{
    ui_TabWidget_Main->setTabText(0, tr("Bible"));
    ui_TabWidget_Main->setTabText(1, tr("Search"));
    ui_TabWidget_Main->setTabText(2, tr("Compare"));
    ui_TabWidget_Main->setTabText(3, tr("Dictionary"));
    ui_TabWidget_Main->setTabText(4, tr("Notes"));

    ui_Menu_File->setTitle(tr("File"));
    ui_Act_OpenModule->setText(tr("Add Bible Module"));
    ui_Act_ModuleInfo->setText(tr("Module Info"));
    ui_Menu_Import->setTitle(tr("Import..."));
    ui_Act_MySwordModule->setText(tr("MySword Module"));
    ui_Act_TheWordModule->setText(tr("TheWord Module"));
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


    if (ui_TabBible->IsInitialized() && !skipBibleTab) {
        qDebug() << "ui_TabBible->setUiTexts()";
        ui_TabBible->SetUiTexts();
    }

    if (ui_TabSearch->IsInitialized()) {
        qDebug() << "ui_TabSearch->setUiTexts()";
        ui_TabSearch->SetUiTexts();
    }
    if (ui_TabCompare->IsInitialized()) {
        ui_TabCompare->SetUiTexts();
    }
    if (ui_TabDictionary->IsInitialized()) {
        ui_TabDictionary->SetUiTexts();
    }
    if (ui_TabFavorites->IsInitialized()) {
        ui_TabFavorites->SetUiTexts();
    }
}

void MainWindow::UpdateFromSettings()
{
    ui_TabBible->SetPassageBrowserFont(m_currentFont);
    if (ui_TabSearch->IsInitialized()) {
        ui_TabSearch->SetFont(m_currentFont);
    }
    ui_TabBible->UpdateFromConfig();
}

void MainWindow::OnAbout()
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

void MainWindow::OnAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::OnAddToNotes(qbv::Location loc)
{
    if (!ui_TabFavorites->IsInitialized()) {
        ui_TabFavorites->Initialize();
    }
    ui_TabFavorites->AddToNotes(loc);
    ui_TabWidget_Main->setCurrentIndex(4);
}

void MainWindow::onBack()
{

}

void MainWindow::OnCommonRareWords()
{
    int idx = ui_TabBible->GetCurrentIndex();
    WidgetCommonRareWords* widgetCommonRareWords = new WidgetCommonRareWords(
                m_modules[idx].translation, m_pConfig->appearance.chart_animation);
    widgetCommonRareWords->show();
}

void MainWindow::onCopy()
{

}

void MainWindow::onCopyWithReference()
{

}

void MainWindow::OnDecreaseFontSize()
{
    if (m_pConfig->fonts.size > 4) {
        QFont font(m_pConfig->fonts.family, m_pConfig->fonts.size - 1);
        SetGlobalFont(font);
        --m_pConfig->fonts.size;
    }
}

void MainWindow::SetGlobalFont(const QFont &font)
{
    if (ui_TabBible->IsInitialized()) {
        ui_TabBible->SetPassageBrowserFont(font);
    }
    if (ui_TabSearch->IsInitialized()) {
        ui_TabSearch->SetFont(font);
    }
    if (ui_TabCompare->IsInitialized()) {
        ui_TabCompare->SetFont(font);
    }
    m_currentFont = font;
}

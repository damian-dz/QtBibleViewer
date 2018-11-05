#include "MainWindow.h"
#include "PDialogXRef.h"
#include "PDialogPreferences.h"
#include "PDialogStrong.h"
#include "PWindowHistogram.h"


void createFavDatabase(QSqlDatabase &db, const QString &fileName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);
    if (db.open()) {
        QSqlQuery query(db);
        query.exec("CREATE TABLE Favorites (Book INT, Chapter INT, VerseFirst INT, VerseLast INT, Comment TEXT)");
        query.exec("CREATE UNIQUE INDEX \"fav_key\" ON \"Favorites\" "
                   "(\"Book\" ASC, \"Chapter\" ASC, \"VerseFirst\" ASC, \"VerseLast\" ASC)");
    }
}

MainWindow::MainWindow(const QString &appDir, const QString &lang, const QString configPath, QWidget *parent)
    : QMainWindow(parent),
      m_textBrowser(nullptr),
      m_textEdit(nullptr),
      m_lineEdit(nullptr),
      ui_Bib_LineEdit_Find(nullptr),
      m_blockHistory(false),
      m_executionPath(appDir),
      m_language(lang),
      m_settingsPath(configPath)
{
    generateMainLayout();
    populateLanguageMap(lang);
    TabBookChapterVerses tbcvv = loadSettings();
    generateBibModuleTabs();
    populateBookNames();
    if (tbcvv.tab != -1) {
        setTabBookChapterVerses(tbcvv, true);
    }
    connectBibleTabSignals();
    if (tbcvv.tab == -1 && m_modulesFound) {
         ui_Bib_ListWidget_Book->setCurrentRow(0);
    }
    QWidget::activateWindow();
}

MainWindow::~MainWindow()
{

}

void MainWindow::generateMainLayout()
{
    QMainWindow::setMinimumSize(QSize(640, 400));
    QMainWindow::setWindowTitle("Qt Bible Viewer");

    generateMenuBarItems();

    QWidget *mainWidget = new QWidget;
    QHBoxLayout *mainHBoxLayout = new QHBoxLayout;
    mainWidget->setLayout(mainHBoxLayout);

    ui_TabWidget_Main = new QTabWidget;
    mainHBoxLayout->addWidget(ui_TabWidget_Main);

    addMainTabs();
    generateBibleTabControls();

    QMainWindow::setCentralWidget(mainWidget);
}

void MainWindow::generateMenuBarItems()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(QIcon(ICON_FOLDER), tr("Open Bible Module"), this,
                &MainWindow::actionOpenBibleModule, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(ICON_EXIT), tr("Exit"), this,
                &MainWindow::actionExit, QKeySequence("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    ui_Act_Back = editMenu->addAction(QIcon(ICON_ARROW_LEFT),
                tr("Back"), this, &MainWindow::actionBack);
    ui_Act_Back->setDisabled(true);
    ui_Act_Forward = editMenu->addAction(QIcon(ICON_ARROW_RIGHT),
                tr("Forward"), this, &MainWindow::actionForward);
    ui_Act_Forward->setDisabled(true);
    editMenu->addSeparator();
    ui_Act_Copy = editMenu->addAction(QIcon(ICON_COPY),
                tr("Copy"), this, &MainWindow::action_ChapterBrowser_Copy, QKeySequence::Copy);
    ui_Act_Copy->setDisabled(true);
    ui_Act_CopyWithRef = editMenu->addAction(QIcon(ICON_COPY_PLUS),
                tr("Copy with Reference"), this, &MainWindow::action_EditMenu_CopyWithReference);
    ui_Act_CopyWithRef->setDisabled(true);
    editMenu->addSeparator();
    editMenu->addAction(tr("Select All"), this, &MainWindow::action_ChapterBrowser_SelectAll,
                        QKeySequence("Ctrl+A"));
    editMenu->addAction(tr("Find"), this, &MainWindow::actionFind,
                        QKeySequence("Ctrl+F"));

    QMenu *statisticsMenu = menuBar()->addMenu(tr("Statistics"));
    statisticsMenu->addAction(
                tr("Word Frequency"), this, &MainWindow::actionWordFrequency);

    QMenu *optionsMenu = menuBar()->addMenu(tr("Options"));
    optionsMenu->addAction(
                tr("Preferences"), this, &MainWindow::actionPreferences);

    ui_Menu_Language = optionsMenu->addMenu(tr("Language"));
    QAction *englishAct = ui_Menu_Language->addAction(
               "English", this, &MainWindow::actionEnglish);
    englishAct->setCheckable(true);
    QAction *spanishAct = ui_Menu_Language->addAction(
               "español", this, &MainWindow::actionSpanish);
    spanishAct->setCheckable(true);
    QAction *polishAct = ui_Menu_Language->addAction(
               "polski", this, &MainWindow::actionPolish);
    polishAct->setCheckable(true);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    ui_ActIncreaseFont = viewMenu->addAction(QIcon(ICON_MAGNIFY),
                tr("Increase Font Size"), this,
                &MainWindow::actionIncreaseFontSize, QKeySequence::ZoomIn);
    ui_ActDecreaseFont = viewMenu->addAction(QIcon(ICON_MINIFY),
                tr("Decrease Font Size"), this,
                &MainWindow::actionDecreaseFontSize, QKeySequence::ZoomOut);

    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(
                tr("Show Help"), this, &MainWindow::actionIncreaseFontSize);
    helpMenu->addAction(
                tr("About"), this, &MainWindow::actionIncreaseFontSize);
    helpMenu->addAction(
                tr("About Qt"), this, &MainWindow::actionAboutQt);

    ui_StatusBar_Status = new QStatusBar;
    ui_Label_Status = new QLabel;
    ui_StatusBar_Status->addWidget(ui_Label_Status);
    QMainWindow::setStatusBar(ui_StatusBar_Status);
}

void MainWindow::addMainTabs()
{
    ui_TabWidget_Main->addTab(new QWidget, tr("Bible"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Details"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Search"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Compare"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Favorites"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Dictionary"));
    ui_TabWidget_Main->addTab(new QWidget, tr("Topics"));
}

void MainWindow::generateBibleTabControls()
{
    QWidget *tabBibleWidget = ui_TabWidget_Main->widget(0);

    QHBoxLayout *tabBibleHorLayout = new QHBoxLayout;
    tabBibleHorLayout->setSpacing(5);
    tabBibleHorLayout->setContentsMargins(10, 10, 10, 10);
    tabBibleWidget->setLayout(tabBibleHorLayout);

    auto bookVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(bookVBoxLayout);

    auto bookLabel = new QLabel;
    bookLabel->setText(tr("Book:"));
    bookVBoxLayout->addWidget(bookLabel);

    ui_Bib_ListWidget_Book = new QListWidget;
    ui_Bib_ListWidget_Book->setMaximumSize(QSize(155, MAX_WIDGET_HEIGHT));
    ui_Bib_ListWidget_Book->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    bookVBoxLayout->addWidget(ui_Bib_ListWidget_Book);

    auto chapterVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(chapterVBoxLayout);

    auto chapterLabel = new QLabel;
    chapterLabel->setText(tr("Chapter:"));
    chapterVBoxLayout->addWidget(chapterLabel);

    ui_Bib_ListWidget_Chapter = new QListWidget;
    ui_Bib_ListWidget_Chapter->setMaximumSize(QSize(60, MAX_WIDGET_HEIGHT));
    ui_Bib_ListWidget_Chapter->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    chapterVBoxLayout->addWidget(ui_Bib_ListWidget_Chapter);

    auto versesVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(versesVBoxLayout);

    auto versesLabel = new QLabel;
    versesLabel->setText(tr("Verses:"));
    versesVBoxLayout->addWidget(versesLabel);

    ui_Bib_ComboBox_VerseFrom = new QComboBox;
    ui_Bib_ComboBox_VerseFrom->setMaximumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseFrom->setMinimumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseFrom->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Bib_ComboBox_VerseFrom->setMaxVisibleItems(30);
    ui_Bib_ComboBox_VerseFrom->setStyleSheet("combobox-popup: 0;");
    versesVBoxLayout->addWidget(ui_Bib_ComboBox_VerseFrom);

    ui_Bib_ComboBox_VerseTo = new QComboBox;
    ui_Bib_ComboBox_VerseTo->setMaximumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseTo->setMinimumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseTo->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Bib_ComboBox_VerseTo->setMaxVisibleItems(30);
    ui_Bib_ComboBox_VerseTo->setStyleSheet("combobox-popup: 0;");
    versesVBoxLayout->addWidget(ui_Bib_ComboBox_VerseTo);
    versesVBoxLayout->addStretch();

    ui_Bib_Button_Random = new QPushButton(tr("Random"));
    ui_Bib_Button_Random->setMaximumSize(QSize(55, 30));
    ui_Bib_Button_Random->setToolTip(tr("Go to a random chapter."));
    versesVBoxLayout->addWidget(ui_Bib_Button_Random);

    auto prevNextHorLayout = new QHBoxLayout;
    versesVBoxLayout->addLayout(prevNextHorLayout);

    ui_Bib_Button_Prev = new QPushButton("<<");
    ui_Bib_Button_Prev->setMaximumWidth(25);
    ui_Bib_Button_Prev->setToolTip(tr("Go to the previous chapter."));
    prevNextHorLayout->addWidget(ui_Bib_Button_Prev);

    ui_Bib_Button_Next = new QPushButton(">>");
    ui_Bib_Button_Next->setMaximumWidth(25);
    ui_Bib_Button_Next->setToolTip(tr("Go to the next chapter."));
    prevNextHorLayout->addWidget(ui_Bib_Button_Next);

    ui_Bib_VerLayout_Modules = new QVBoxLayout;
    tabBibleHorLayout->addLayout(ui_Bib_VerLayout_Modules);

    ui_Bib_TabWidget_Modules = new QTabWidget;
    ui_Bib_TabWidget_Modules->setMovable(true);
    ui_Bib_TabWidget_Modules->setTabsClosable(true);
    ui_Bib_VerLayout_Modules->addWidget(ui_Bib_TabWidget_Modules);

    ui_Bib_TabBar_Modules = ui_Bib_TabWidget_Modules->tabBar();
}

void MainWindow::populateLanguageMap(const QString &lang)
{
    m_languages.insert(0, "EN");
    m_languages.insert(1, "ES");
    m_languages.insert(2, "PL");
    checkLanguageAction(m_languages.key(lang), true);
}

MainWindow::TabBookChapterVerses MainWindow::loadSettings()
{
    qDebug() << m_settingsPath;
    TabBookChapterVerses tbcvv;
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.beginGroup(GROUP_MAIN_WINDOW);
    const QByteArray geometry = settings.value(SET_GEOMETRY, QByteArray()).toByteArray();
    if (!geometry.isNull() && !geometry.isEmpty()) {
        QWidget::restoreGeometry(geometry);
    }
    else {
        QMainWindow::resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    }
    const QByteArray state = settings.value(SET_STATE, QByteArray()).toByteArray();
    if (!state.isNull() && !state.isEmpty()) {
        QMainWindow::restoreState(state);
    }
    settings.endGroup();
    settings.beginGroup(GROUP_MODULE_DATA);
    const int setIndex = settings.value(SET_INDEX).toInt();
    const QStringList setPassage = settings.value(SET_PASSAGE).toStringList();
    if (!setPassage.isEmpty() && setPassage.count() == 4) {
        tbcvv = TabBookChapterVerses{ setIndex,
            setPassage[0].toInt(),
            setPassage[1].toInt(),
            setPassage[2].toInt(),
            setPassage[3].toInt() };
    }
    else {
        tbcvv = TabBookChapterVerses{ -1, -1, -1, -1, -1 };
    }
    m_modulePathsList = settings.value(SET_PATHS).toStringList();
    if (m_modulePathsList.count() == 1 && m_modulePathsList[0] == "") {
        m_modulePathsList.clear();
    }

    m_removedPathsList = settings.value(SET_REMOVED_PATHS).toStringList();
    if (m_removedPathsList.count() == 1 && m_removedPathsList[0] == "") {
        m_removedPathsList.clear();
    }
    m_comVerse = settings.value(SET_COM_VERSE).toStringList();
    if (m_comVerse.count() == 1 && m_comVerse[0] == "") {
        m_comVerse.clear();
    }
    settings.endGroup();
    settings.beginGroup(GROUP_FONT_SETTINGS);
    const QString setFontFamily = settings.value(SET_FONT_FAMILY).toString();
    const int setFontSize = settings.value(SET_FONT_SIZE).toInt();
    if (!setFontFamily.isEmpty() && setFontSize != 0) {
        m_currentFont = QFont(setFontFamily, setFontSize);
    } else {
        m_currentFont = QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE);
    }
    settings.endGroup();
    return tbcvv;
}

void MainWindow::generateBibModuleTabs()
{
    const QString moduleDirName = m_executionPath + "/modules";
    QString noModule = tr("No module found");
    if (!QDir(moduleDirName).exists()) {
        QDir().mkdir(moduleDirName);
        ui_Bib_TabWidget_Modules->addTab(new QWidget(), noModule);
        ui_Bib_ListWidget_Book->setDisabled(true);
        m_modulesFound = false;
    } else {
        QStringList localModules = getModulePaths(moduleDirName);
        foreach (QString path, localModules) {
            if (!m_modulePathsList.contains(path)
                    && !m_removedPathsList.contains(path)
                    && QFileInfo(path).exists()) {
                m_modulePathsList << path;
            }
        }
        if (m_modulePathsList.isEmpty()) {
            ui_Bib_TabWidget_Modules->addTab(new QWidget(), noModule);
            ui_Bib_ListWidget_Book->setDisabled(true);
            m_modulesFound = false;
        } else {
            loadBackgroundPixmap();
            loadXRefDatabase();
            for (int i = 0; i < m_modulePathsList.count(); ++i) {
                if (QFileInfo(m_modulePathsList[i]).exists()) {
                    loadBibleModule(m_modulePathsList[i]);
                } else {
                    m_modulePathsList.removeAt(i--);
                }
            }
            m_modulesFound = true;
        }
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
    ui_Bib_ListWidget_Book->addItems(m_bookNames);
}

void MainWindow::connectBibleTabSignals()
{
    QObject::connect(ui_TabWidget_Main, SIGNAL(currentChanged(int)),
                     this, SLOT(mainTabWidgetCurrentChanged(int)));
    QObject::connect(ui_Bib_ListWidget_Book, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Bib_CurrentRowChanged_ListWidget_Book(int)));
    QObject::connect(ui_Bib_ListWidget_Chapter, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Bib_CurrentRowChanged_ListWidget_Chapter(int)));
    QObject::connect(ui_Bib_ComboBox_VerseFrom, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Bib_CurrentIndexChanged_ComboBox_VerseFrom(int)));
    QObject::connect(ui_Bib_ComboBox_VerseTo, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Bib_CurrentIndexChanged_ComboBox_VerseTo(int)));
    QObject::connect(ui_Bib_Button_Random, SIGNAL(clicked()),
                     this, SLOT(on_Bib_ButtonClicked_RandomChapter()));
    QObject::connect(ui_Bib_Button_Prev, SIGNAL(clicked()),
                     this, SLOT(on_Bib_ButtonClicked_PreviousChapter()));
    QObject::connect(ui_Bib_Button_Next, SIGNAL(clicked()),
                     this, SLOT(on_Bib_ButtonClicked_NextChapter()));
    QObject::connect(ui_Bib_TabBar_Modules, SIGNAL(tabMoved(int, int)),
                     this, SLOT(on_Bib_TabMoved_Modules(int, int)));
    QObject::connect(ui_Bib_TabWidget_Modules, SIGNAL(currentChanged(int)),
                     this, SLOT(modulesTabWidgetCurrentChanged(int)));
    QObject::connect(ui_Bib_TabWidget_Modules, SIGNAL(tabCloseRequested(int)),
                     this, SLOT(modulesTabCloseRequested(int)));
}

void MainWindow::setTabBookChapterVerses(const TabBookChapterVerses &tbcvv, bool firstRun)
{
    ui_Bib_TabWidget_Modules->setCurrentIndex(tbcvv.tab);
    ui_Bib_ListWidget_Book->setCurrentRow(tbcvv.book - 1);
    populateChapterListWidget(tbcvv.chapter);
    populateVersesComboBoxes(tbcvv.verseFrom, tbcvv.verseTo);
    if (firstRun) {
        loadPassage();
    }
}

void MainWindow::populateChapterListWidget(int chapter)
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    int book = ui_Bib_ListWidget_Book->currentRow() + 1;
    QString queryString = "SELECT Chapter FROM Bible WHERE Book = " +
                          QString::number(book) +
                          " AND Verse = 1";
    QSqlQuery query(m_modules[idx].database);
    query.exec(queryString);
    QStringList chapterNumbers;
    while (query.next()) {
        chapterNumbers << query.record().value(0).toString();
    }
    ui_Bib_ListWidget_Chapter->clear();
    if (!chapterNumbers.isEmpty()) {
        ui_Bib_ListWidget_Chapter->addItems(chapterNumbers);
        ui_Bib_ListWidget_Chapter->setCurrentRow(chapter - 1);
    } else {
        m_chapterBrowsers[idx]->setHtml(
                    "<center><h2>" + tr("Unavailable in this module.") + "</center></h2>");
    }
}

void MainWindow::populateVersesComboBoxes(int verseFrom, int verseTo)
{
    int dbIdx = ui_Bib_TabWidget_Modules->currentIndex();
    int book = ui_Bib_ListWidget_Book->currentRow() + 1;
    int chapter = ui_Bib_ListWidget_Chapter->currentRow() + 1;
    QString queryString = "SELECT Verse FROM Bible WHERE Book = " + QString::number(book) +
                          " AND Chapter = " + QString::number(chapter);
    QSqlQuery query(m_modules[dbIdx].database);
    query.exec(queryString);
    QStringList verseNumbers;
    while (query.next()) {
        verseNumbers << query.record().value(0).toString();
    }
    ui_Bib_ComboBox_VerseFrom->blockSignals(true);
    ui_Bib_ComboBox_VerseTo->blockSignals(true);

    ui_Bib_ComboBox_VerseFrom->clear();
    ui_Bib_ComboBox_VerseFrom->addItems(verseNumbers);
    ui_Bib_ComboBox_VerseFrom->setCurrentIndex(verseFrom - 1);

    ui_Bib_ComboBox_VerseTo->clear();
    ui_Bib_ComboBox_VerseTo->addItems(verseNumbers);
    ui_Bib_ComboBox_VerseTo->blockSignals(false);
    int last = verseTo == -1 ? verseNumbers.count() - 1 : verseTo - 1;
    ui_Bib_ComboBox_VerseTo->setCurrentIndex(last);

    ui_Bib_ComboBox_VerseFrom->blockSignals(false);
}


void MainWindow::clearChapterBrowserData(int idx)
{
    m_chapterBrowsers[idx]->clear();
    m_globalNotes[idx].clear();
    m_verseMaps[idx].clear();
    m_noteCount = 0;
}

void MainWindow::loadFavorites()
{
    QString dirName = m_executionPath + "/user";
    QString fileName = dirName + "/fav.bblv";
    if (!QDir(dirName).exists()) {
        QDir().mkdir(dirName);
        createFavDatabase(m_dbUsr, fileName);
    } else if (!QDir().exists(fileName)) {
        createFavDatabase(m_dbUsr, fileName);
    } else {
        m_dbUsr = QSqlDatabase::addDatabase("QSQLITE");
        m_dbUsr.setDatabaseName(fileName);
        m_dbUsr.open();
        QSqlQuery query(m_dbUsr);
        query.exec("SELECT * FROM Favorites");
        QStringList favList;
        while (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verseFirst = record.value(2).toInt();
            int verseLast = record.value(3).toInt();
            QString passageId = m_bookNames[book - 1] + " " + QString::number(chapter) +
                    ":" + QString::number(verseFirst);
            if (verseFirst != verseLast) {
                passageId += "-" % QString::number(verseLast);
            }
            favList << passageId;
            m_favorites.append({ 0, book, chapter, verseFirst, verseLast });
        }
        if (favList.count() > 0) {
            ui_Fav_ListWidget_Passages->addItems(favList);
            ui_Fav_ListWidget_Passages->setCurrentRow(0);
        } else {
            ui_Fav_Button_Delete->setDisabled(true);
            ui_Fav_Button_Save->setDisabled(true);
        }
    }
}

#include <QDebug>
void MainWindow::loadPassage()
{
    qDebug() << "loadPassage";
    TabBookChapterVerses tbcvv = getTabBookChapterVerses();
    int idx = tbcvv.tab;
    QString bookStr = QString::number(tbcvv.book);
    QString chapterStr = QString::number(tbcvv.chapter);
    QString queryString = "SELECT Verse, Scripture FROM Bible"
                          " WHERE Book = " + bookStr +
                          " AND Chapter = " + chapterStr +
                          " AND Verse >= " + QString::number(tbcvv.verseFrom) +
                          " AND Verse <= " + QString::number(tbcvv.verseTo);
    QSqlQuery query(m_modules[idx].database);
    query.exec(queryString);
    QSqlQuery xRefQuery(m_dbXRef);
    clearChapterBrowserData(idx);
    QTextCursor cursor(m_chapterBrowsers[idx]->document());
    int verseCnt = 0;
    QRegularExpression noteRgx("<RF>[^<]*<Rf>");
    QRegularExpression strongRgx("<W[HG]\\d{1,4}>");
    while (query.next()) {
        QSqlRecord record = query.record();
        QString verse = record.value(0).toString();
        QString scripture = record.value(1).toString();
        QString xRefQueryString = "SELECT XRefs FROM CrossReferences WHERE BOOK = " + bookStr +
                                  " AND Chapter = " + chapterStr +
                                  " AND Verse = " + verse;
        m_verseMaps[idx][verseCnt] = verse.toInt();
        if (verseCnt > 0) {
            cursor.insertBlock();
        }
        xRefQuery.exec(xRefQueryString);
        if (xRefQuery.next()) {           
            verse = QString("<a href='x:%1:%2'>[%3]</a>")
                    .arg(verse, xRefQuery.record().value(0).toString(), verse);
            formatScripture(scripture, idx, m_modules[idx].hasStrong, noteRgx, strongRgx);
            cursor.insertHtml("<b>" + verse + "</b> " + scripture);
        } else {
            formatScripture(scripture, idx, m_modules[idx].hasStrong, noteRgx, strongRgx);
            cursor.insertHtml("<b>[" + verse + "]</b> " + scripture);
        }
        verseCnt++;
    }
    cursor.setPosition(0);
    m_chapterBrowsers[idx]->setTextCursor(cursor);
    if (!m_blockHistory) {
        updateHistory(tbcvv);
    }
    ui_Bib_Button_Prev->setDisabled(tbcvv.book == 1 && tbcvv.chapter == 1);
    ui_Bib_Button_Next->setDisabled(tbcvv.book == ui_Bib_ListWidget_Book->count() &&
                                    tbcvv.chapter == ui_Bib_ListWidget_Chapter->count());
}

MainWindow::TabBookChapterVerses MainWindow::getTabBookChapterVerses()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    int book = ui_Bib_ListWidget_Book->currentRow() + 1;
    int chapter = ui_Bib_ListWidget_Chapter->currentRow() + 1;
    int verseFrom = ui_Bib_ComboBox_VerseFrom->currentIndex() + 1;
    int verseTo = ui_Bib_ComboBox_VerseTo->currentIndex() + 1;
    return TabBookChapterVerses { idx, book, chapter, verseFrom, verseTo };
}

void MainWindow::updateHistory(const TabBookChapterVerses &tbcvv)
{
    if (m_history.count() == MAX_RECENT_PASSAGES) {
        m_history.removeFirst();
    } else if (m_psgIdx < m_history.count() - 1) {
        m_history.remove(m_psgIdx + 1, m_history.count() - 1 - m_psgIdx);
    }
    m_history.append(tbcvv);
    m_psgIdx = m_history.count() - 1;
    ui_Act_Back->setEnabled(m_history.count() > 1);
    ui_Act_Forward->setDisabled(true);
}

void MainWindow::on_Bib_SelectionChanged_ChapterBrowser()
{
    int idx = ui_Bib_TabBar_Modules->currentIndex();
    QTextCursor cursor = m_chapterBrowsers[idx]->textCursor();
    bool isEmpty = cursor.selectionStart() == cursor.selectionEnd();
    ui_Act_Copy->setDisabled(isEmpty);
    ui_Act_CopyWithRef->setDisabled(isEmpty);
}

void MainWindow::on_Sea_ButtonClicked_Search()
{
    ui_Sea_RadioButton_Strong->isChecked() ? performSearchByStrong() : performSearch();
}

void MainWindow::on_Sea_ButtonToggled_ByStrong(bool checked)
{
    ui_Sea_CheckBox_Case->setDisabled(checked);
    ui_Sea_CheckBox_WholeWords->setDisabled(checked);
    if (checked) {
        QRegExp regex("^[HG]\\d{1,4}$", Qt::CaseInsensitive);
        QValidator *validator = new QRegExpValidator(regex, this);
        ui_Sea_LineEdit_Search->setValidator(validator);
    } else {
        delete ui_Sea_LineEdit_Search->validator();
    }
}

void MainWindow::connectSearchTabSignals()
{
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(returnPressed()),
                     this, SLOT(on_Sea_LineEdit_ReturnPressed_Search()));
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(textChanged(QString)),
                     this, SLOT(on_Sea_LineEdit_TextChanged_Search(QString)));
    QObject::connect(ui_Sea_Button_Search, SIGNAL(clicked()),
                     this, SLOT(on_Sea_ButtonClicked_Search()));
    QObject::connect(ui_Sea_ComboBox_Section, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_Section(int)));
    QObject::connect(ui_Sea_Button_Prev, SIGNAL(clicked()),
                     this, SLOT(on_Sea_ButtonClicked_PreviousResult()));
    QObject::connect(ui_Sea_Button_Next, SIGNAL(clicked()),
                     this, SLOT(on_Sea_ButtonClicked_NextResult()));
    QObject::connect(ui_Sea_Button_RandomVerse, SIGNAL(clicked()),
                     this, SLOT(on_Sea_ButtonClicked_RandomVerse()));
}

void MainWindow::displaySearchResults(int startIdx, int endIdx)
{
    ui_Sea_TextBrowser_Results->clear();
    int index = ui_Sea_ComboBox_Translation->currentIndex();
    int i = startIdx;
    QString resultString;
    QRegExp regex("<RF>.*<Rf>");
    regex.setMinimal(true);
    while (i < m_resVerses.count() && i < endIdx) {
        resultString += formatResult(m_resVerses[i], regex, m_modules[index].hasStrong);
        resultString += m_resRefs[i++];
        resultString += "</a></b><br><br>";
    }
    ui_Sea_TextBrowser_Results->setHtml(resultString);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    while (ui_Sea_TextBrowser_Results->find(m_dispRgx)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = ui_Sea_TextBrowser_Results->textCursor();
        extraSelections.append(extra);
    }
    ui_Sea_TextBrowser_Results->setExtraSelections(extraSelections);
    regex = QRegExp("—(\\p{L}+|[1-3] \\p{L}+|\\p{L}+ \\p{L}+ \\p{L}+) \\d{1,3}:\\d{1,3}");
    color = Qt::transparent;
    while (ui_Sea_TextBrowser_Results->find(regex, QTextDocument::FindBackward)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = ui_Sea_TextBrowser_Results->textCursor();
        extraSelections.append(extra);
    }
    ui_Sea_TextBrowser_Results->setExtraSelections(extraSelections);
    ui_Sea_TextBrowser_Results->moveCursor(QTextCursor::Start);

    QString statusMessage;
    if (m_resVerses.count() == 1) {
        statusMessage = tr("Verse 1 (1 in total); Elapsed time: ") + m_elapsedTime;
    } else {
        statusMessage = tr("Verses ") + QString::number(startIdx + 1) + "-" + QString::number(i) +
            " ("  + QString::number(m_resVerses.count()) + tr(" in total);  Elapsed time: ") +
            m_elapsedTime;
    }
    ui_Label_Status->setText(statusMessage);
}

void MainWindow::generateCompareTabControls(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabCompareWidget = ui_TabWidget_Main->widget(idx);

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout(tabCompareWidget);
    mainHBoxLayout->setSpacing(5);
    mainHBoxLayout->setContentsMargins(10, 10, 10, 10);

    auto bookVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(bookVBoxLayout);

    auto bookLabel = new QLabel(tr("Book:"));
    bookVBoxLayout->addWidget(bookLabel);

    ui_Com_ListWidget_Book = new QListWidget;
    ui_Com_ListWidget_Book->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Book->setMaximumWidth(155);
    ui_Com_ListWidget_Book->addItems(m_bookNames);
    bookVBoxLayout->addWidget(ui_Com_ListWidget_Book);
    QObject::connect(ui_Com_ListWidget_Book, SIGNAL(currentRowChanged(int)),
                     this, SLOT(currentRowChangedComListWidgetBook(int)));

    QVBoxLayout *chapterVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(chapterVBoxLayout);

    QLabel *chapterLabel = new QLabel(tr("Chapter:"));
    chapterVBoxLayout->addWidget(chapterLabel);

    ui_Com_ListWidget_Chapter = new QListWidget;
    ui_Com_ListWidget_Chapter->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Chapter->setMaximumWidth(60);
    chapterVBoxLayout->addWidget(ui_Com_ListWidget_Chapter);
    QObject::connect(ui_Com_ListWidget_Chapter, SIGNAL(currentRowChanged(int)),
                     this, SLOT(currentRowChangedComListWidgetChapter(int)));



   // ui_Com_Label_ChapterNumber = new QLabel;
   // ui_Com_Label_ChapterNumber->setText("-/1189");
   // chapterVBoxLayout->addWidget(ui_Com_Label_ChapterNumber);

    QVBoxLayout *verseVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(verseVBoxLayout);

    QLabel *verseLabel = new QLabel(tr("Verse:"));
    verseVBoxLayout->addWidget(verseLabel);

    ui_Com_ListWidget_Verse = new QListWidget;
    ui_Com_ListWidget_Verse->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Verse->setMaximumWidth(60);
    verseVBoxLayout->addWidget(ui_Com_ListWidget_Verse);
    QObject::connect(ui_Com_ListWidget_Verse, SIGNAL(currentRowChanged(int)),
                     this, SLOT(currentRowChangedComListWidgetVerse(int)));

    QVBoxLayout *compareVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(compareVBoxLayout);

    ui_Com_TextBrowser_Compare = new QTextBrowser;
    ui_Com_TextBrowser_Compare->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Com_TextBrowser_Compare, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    ui_Com_TextBrowser_Compare->setFont(m_currentFont);
    ui_Com_TextBrowser_Compare->setOpenLinks(false);
    compareVBoxLayout->addWidget(ui_Com_TextBrowser_Compare);
    setBrowserBackground(*ui_Com_TextBrowser_Compare);
    QObject::connect(ui_Com_TextBrowser_Compare, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Com_AnchorClicked_TextBrowser_Compare(QUrl)));

    auto prevNextHLayout = new QHBoxLayout;
    compareVBoxLayout->addLayout(prevNextHLayout);

    ui_Com_Button_Prev = new QPushButton("<<");
    ui_Com_Button_Prev->setToolTip(tr("Go to the previous verse."));
    prevNextHLayout->addWidget(ui_Com_Button_Prev);

    ui_Com_Button_Next = new QPushButton(">>");
    ui_Com_Button_Next->setToolTip(tr("Go to the next verse."));
    prevNextHLayout->addWidget(ui_Com_Button_Next);
    prevNextHLayout->addStretch();

    m_dbCntr = QSqlDatabase::addDatabase("QSQLITE", "Counters");
    m_dbCntr.setDatabaseName(m_executionPath + "/data/counters.bblv");
    if (m_dbCntr.open()) {

    } else {
        QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
    }

    if (!m_comVerse.isEmpty()) {
        ui_Com_ListWidget_Book->setCurrentRow(m_comVerse[0].toInt() - 1);
        ui_Com_ListWidget_Chapter->setCurrentRow(m_comVerse[1].toInt() - 1);
        ui_Com_ListWidget_Verse->setCurrentRow(m_comVerse[2].toInt() - 1);
    } else {
        ui_Com_ListWidget_Book->setCurrentRow(0);
        ui_Com_ListWidget_Chapter->setCurrentRow(0);
        ui_Com_ListWidget_Verse->setCurrentRow(0);
    }
}

void MainWindow::currentRowChangedComListWidgetBook(int currentRow)
{
    QString queryString = "SELECT Chapter FROM Counters WHERE Book = " +
                          QString::number(currentRow + 1);
    QSqlQuery query(m_dbCntr);
    query.exec(queryString);
    QStringList chapterNumbers;
    while (query.next()) {
        chapterNumbers << query.record().value(0).toString();
    }
    ui_Com_ListWidget_Chapter->blockSignals(true);
    ui_Com_ListWidget_Chapter->clear();
    ui_Com_ListWidget_Chapter->addItems(chapterNumbers);
    ui_Com_ListWidget_Chapter->blockSignals(false);
    // if (!firstLoadCompare)
    ui_Com_ListWidget_Chapter->setCurrentRow(0);
}

void MainWindow::currentRowChangedComListWidgetChapter(int currentRow)
{
    int book = ui_Com_ListWidget_Book->currentRow() + 1;
    QString queryString = "SELECT VerseCount, ChapterNumber FROM Counters WHERE Book = " +
                          QString::number(book) + " AND Chapter = " + QString::number(currentRow + 1);
    QSqlQuery query(m_dbCntr);
    query.exec(queryString);
    int verseCount = 0;
    if (query.next()) {
        QSqlRecord record = query.record();
        verseCount = record.value(0).toInt();
        ui_Label_Status->setText(tr("Chapter: ") + record.value(1).toString() + "/1189");
    }
    QStringList verseNumbers;
    for (int i = 1; i <= verseCount; ++i) {
        verseNumbers << QString::number(i);
    }
    ui_Com_ListWidget_Verse->blockSignals(true);
    ui_Com_ListWidget_Verse->clear();
    ui_Com_ListWidget_Verse->addItems(verseNumbers);
    ui_Com_ListWidget_Verse->blockSignals(false);
    // if (!firstLoadCompare)
    ui_Com_ListWidget_Verse->setCurrentRow(0);
}

void MainWindow::currentRowChangedComListWidgetVerse(int currentRow)
{
    QString book = QString::number(ui_Com_ListWidget_Book->currentRow() + 1);
    QString chapter = QString::number(ui_Com_ListWidget_Chapter->currentRow() + 1);
    QString verse = QString::number(currentRow + 1);
    QString queryString = "SELECT Scripture FROM Bible WHERE Book = " + book +
            " AND Chapter = " + chapter +
            " AND Verse = " + verse;
    QString text = "<table border='1' cellpadding='8' width='100%'>";
    for (int i = 0; i < m_modules.count(); ++i) {
        QSqlQuery query(m_modules[i].database);
        query.exec(queryString);
        if (query.next()) {
            text += "<tr><td><b><a href='t:" + QString::number(i) + "'>" +
                    m_modules[i].name + ":</a></b>";
            QString scripture = query.record().value(0).toString().trimmed();
            text += " " + formatVerses(scripture, m_modules[i].hasStrong);
            text += "</td></tr>";
        }
    }
    text += QStringLiteral("</table>");
    ui_Com_TextBrowser_Compare->setHtml(text);
    m_comVerse.clear();
    m_comVerse << QString::number(ui_Com_ListWidget_Book->currentRow() + 1)
               << QString::number(ui_Com_ListWidget_Chapter->currentRow() + 1)
               << QString::number(ui_Com_ListWidget_Verse->currentRow() + 1);
}

QString MainWindow::formatVerses(QString text, bool hasStrong)
{
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    text.remove("<CM>");
    QRegExp rgxNotesHeadings("<RF>[^<]*<Rf>|<TS>[^<]*<Ts>");
    text.remove(rgxNotesHeadings);
    if (hasStrong) {
        QRegularExpression regex("<W[HG][0-9]{1,4}>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, " <a href='s:" + modified + "'>" + modified + "</a>");
            }
        }
    }
    return text;
}


void MainWindow::generateDetailsTab(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabDetailsWidget = ui_TabWidget_Main->widget(idx);

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout(tabDetailsWidget);
    mainVBoxLayout->setSpacing(5);
    mainVBoxLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *detailsHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(detailsHBoxLayout);

    QVBoxLayout *descriptionAbbreviationVBoxLayout = new QVBoxLayout;
    detailsHBoxLayout->addLayout(descriptionAbbreviationVBoxLayout);

    QVBoxLayout *descriptionVBoxLayout = new QVBoxLayout;
    descriptionAbbreviationVBoxLayout->addLayout(descriptionVBoxLayout);

    QLabel *descriptionLabel = new QLabel(tr("Description:"));
    descriptionVBoxLayout->addWidget(descriptionLabel);

    ui_Det_TextBrowser_Description = new QTextBrowser;
    ui_Det_TextBrowser_Description->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_TextBrowser_Description->setFont(m_currentFont);
    ui_Det_TextBrowser_Description->setOpenExternalLinks(false);
    QObject::connect(ui_Det_TextBrowser_Description, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowBasicContextMenu(QPoint)));
    descriptionVBoxLayout->addWidget(ui_Det_TextBrowser_Description);
    setBrowserBackground(*ui_Det_TextBrowser_Description);

    QVBoxLayout *abbreviationVBoxLayout = new QVBoxLayout;
    descriptionAbbreviationVBoxLayout->addLayout(abbreviationVBoxLayout);

    QLabel *abbreviationLabel = new QLabel(tr("Abbreviation:"));
    abbreviationVBoxLayout->addWidget(abbreviationLabel);

    ui_Det_LineEdit_Abbreviation = new QLineEdit;
    ui_Det_LineEdit_Abbreviation->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_Det_LineEdit_Abbreviation, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowLineContextMenu(QPoint)));
    ui_Det_LineEdit_Abbreviation->setReadOnly(true);
    abbreviationVBoxLayout->addWidget(ui_Det_LineEdit_Abbreviation);

    QVBoxLayout *commentsVBoxLayout = new QVBoxLayout;
    detailsHBoxLayout->addLayout(commentsVBoxLayout);

    QLabel *commentsLabel = new QLabel(tr("Comments:"));
    commentsVBoxLayout->addWidget(commentsLabel);

    ui_Det_TextBrowser_Comments = new QTextBrowser;
    ui_Det_TextBrowser_Comments->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_TextBrowser_Comments->setFont(m_currentFont);
    ui_Det_TextBrowser_Comments->setOpenExternalLinks(false);
    ui_Det_TextBrowser_Comments->setOpenLinks(false);
    QObject::connect(ui_Det_TextBrowser_Comments, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowBasicContextMenu(QPoint)));
    commentsVBoxLayout->addWidget(ui_Det_TextBrowser_Comments);
    setBrowserBackground(*ui_Det_TextBrowser_Comments);

    QHBoxLayout *versionPublishDateHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(versionPublishDateHBoxLayout);

    QVBoxLayout *versionVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(versionVBoxLayout);

    QLabel *versionLabel = new QLabel(tr("Version:"));
    versionVBoxLayout->addWidget(versionLabel);

    ui_Det_LineEdit_Version = new QLineEdit;
    ui_Det_LineEdit_Version->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_Det_LineEdit_Version, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowLineContextMenu(QPoint)));
    ui_Det_LineEdit_Version->setReadOnly(true);
    versionVBoxLayout->addWidget(ui_Det_LineEdit_Version);

    QVBoxLayout *versionDateVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(versionDateVBoxLayout);

    QLabel *versionDateLabel = new QLabel(tr("Version Date:"));
    versionDateVBoxLayout->addWidget(versionDateLabel);

    ui_Det_LineEdit_VersionDate = new QLineEdit;
    ui_Det_LineEdit_VersionDate->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_Det_LineEdit_VersionDate, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowLineContextMenu(QPoint)));
    ui_Det_LineEdit_VersionDate->setReadOnly(true);
    versionDateVBoxLayout->addWidget(ui_Det_LineEdit_VersionDate);

    auto publishDateVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(publishDateVBoxLayout);

    auto publishDateLabel = new QLabel(tr("Publish Date:"));
    publishDateVBoxLayout->addWidget(publishDateLabel);

    ui_Det_LineEdit_PublishDate = new QLineEdit;
    ui_Det_LineEdit_PublishDate->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_Det_LineEdit_PublishDate, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(actionShowLineContextMenu(QPoint)));
    ui_Det_LineEdit_PublishDate->setReadOnly(true);
    publishDateVBoxLayout->addWidget(ui_Det_LineEdit_PublishDate);

    QHBoxLayout *rightOldNewStrongHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(rightOldNewStrongHBoxLayout);

    ui_Det_CheckBox_RightToLeft = new QCheckBox(tr("Right to Left"));
    ui_Det_CheckBox_RightToLeft->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_RightToLeft->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_RightToLeft->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_RightToLeft);

    ui_Det_CheckBox_OldTestament = new QCheckBox(tr("Old Testament"));
    ui_Det_CheckBox_OldTestament->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_OldTestament->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_OldTestament->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_OldTestament);

    ui_Det_CheckBox_NewTestament = new QCheckBox(tr("New Testament"));
    ui_Det_CheckBox_NewTestament->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_NewTestament->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_NewTestament->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_NewTestament);

    ui_Det_CheckBox_StrongsNumbers = new QCheckBox(tr("Strong's Numbers"));
    ui_Det_CheckBox_StrongsNumbers->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_StrongsNumbers->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_StrongsNumbers->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_StrongsNumbers);

    detailsHBoxLayout->setStretchFactor(descriptionAbbreviationVBoxLayout, 1);
    detailsHBoxLayout->setStretchFactor(commentsVBoxLayout, 2);

    fillDetailsTab();
}

void MainWindow::fillDetailsTab()
{
    QString queryString = "SELECT "
                          "Description, "
                          "Abbreviation, "
                          "Comments, "
                          "Version, "
                          "VersionDate, "
                          "PublishDate, "
                          "RightToLeft, "
                          "OT, "
                          "NT, "
                          "Strong "
                          "FROM Details";
    int idx = ui_Bib_TabBar_Modules->currentIndex();
    QSqlQuery query(m_modules[idx].database);
    if (query.exec(queryString)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            ui_Det_TextBrowser_Description->setHtml(record.value(0).toString());
            ui_Det_LineEdit_Abbreviation->setText(record.value(1).toString());
            ui_Det_TextBrowser_Comments->setHtml(record.value(2).toString());
            ui_Det_LineEdit_Version->setText(record.value(3).toString());
            ui_Det_LineEdit_VersionDate->setText(record.value(4).toString());
            ui_Det_LineEdit_PublishDate->setText(record.value(5).toString());
            ui_Det_CheckBox_RightToLeft->setChecked(record.value(6).toBool());
            ui_Det_CheckBox_OldTestament->setChecked(record.value(7).toBool());
            ui_Det_CheckBox_NewTestament->setChecked(record.value(8).toBool());
            ui_Det_CheckBox_StrongsNumbers->setChecked(record.value(9).toBool());
        }
    }
}

void MainWindow::generateDictionaryTabControls(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabDictionaryWidget = ui_TabWidget_Main->widget(idx);

    QHBoxLayout *mainHorLayout = new QHBoxLayout(tabDictionaryWidget);
    mainHorLayout->setSpacing(5);
    mainHorLayout->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout *numberEntriesVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(numberEntriesVerLayout);

    QLabel *numberLabel = new QLabel(tr("Number:"));
    numberEntriesVerLayout->addWidget(numberLabel);

    QLineEdit *numberLineEdit = new QLineEdit;
    numberLineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(numberLineEdit, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
    numberLineEdit->setMaximumWidth(220);
    numberEntriesVerLayout->addWidget(numberLineEdit);
    numberLineEdit->setFocus();
    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
    QValidator *validator = new QRegExpValidator(regex, this);
    numberLineEdit->setValidator(validator);
    QObject::connect(numberLineEdit, SIGNAL(textEdited(QString)),
                     this, SLOT(on_Dic_TextEdited_LineEdit_Number(QString)));

    QLabel *allEntriesLabel = new QLabel(tr("All Entries:"));
    numberEntriesVerLayout->addWidget(allEntriesLabel);

    ui_Dic_ListWidget_AllEntries = new QListWidget;
    ui_Dic_ListWidget_AllEntries->setMaximumWidth(220);
    numberEntriesVerLayout->addWidget(ui_Dic_ListWidget_AllEntries);
    QObject::connect(ui_Dic_ListWidget_AllEntries, SIGNAL(currentTextChanged(QString)),
                     this, SLOT(on_Dic_TextChanged_ListWidget_AllEntries(QString)));

    QVBoxLayout *definitionVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(definitionVerLayout);

    QLabel *definitionLabel = new QLabel(tr("Definition:"));
    definitionVerLayout->addWidget(definitionLabel);

    ui_Dic_TextBrowser_Definition = new QTextBrowser;
    ui_Dic_TextBrowser_Definition->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Dic_TextBrowser_Definition, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    ui_Dic_TextBrowser_Definition->setFont(m_currentFont);
    ui_Dic_TextBrowser_Definition->setOpenLinks(false);
    definitionVerLayout->addWidget(ui_Dic_TextBrowser_Definition);
    setBrowserBackground(*ui_Dic_TextBrowser_Definition);
    QObject::connect(ui_Dic_TextBrowser_Definition, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Dic_AnchorClicked_TextBrowser_Definition(QUrl)));

    if (!m_dbStrong.isOpen()) {
        m_dbStrong = QSqlDatabase::addDatabase("QSQLITE", "Strong");
        m_dbStrong.setDatabaseName(m_executionPath + "/dictionaries/strong_lite.dct.mybible");
        if (m_dbStrong.open()) {

        } else {
            QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
        }
    }
    QSqlQuery fillQuery(m_dbStrong);
    QString queryString = "SELECT word FROM dictionary WHERE relativeorder > 0";
    QStringList dictEntryList;
    if (fillQuery.exec(queryString)) {
        while (fillQuery.next()) {
            dictEntryList << fillQuery.record().value(0).toString();
        }
    }
    ui_Dic_ListWidget_AllEntries->addItems(dictEntryList);
}

void MainWindow::generateFavoritesTabControls(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabFavoritesWidget = ui_TabWidget_Main->widget(idx);

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout(tabFavoritesWidget);
    mainHBoxLayout->setSpacing(5);
    mainHBoxLayout->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout *passagesVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(passagesVBoxLayout);

    QLabel *passagesLabel = new QLabel(tr("Favorite Passages:"));
    passagesVBoxLayout->addWidget(passagesLabel);

    ui_Fav_ListWidget_Passages = new QListWidget;
    ui_Fav_ListWidget_Passages->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Fav_ListWidget_Passages->setMaximumWidth(220);
    passagesVBoxLayout->addWidget(ui_Fav_ListWidget_Passages);
    connect(ui_Fav_ListWidget_Passages, SIGNAL(currentRowChanged(int)),
            this, SLOT(on_Fav_CurrentRowChanged_ListWidget_Passages(int)));

    QVBoxLayout *textCommentVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(textCommentVBoxLayout);

    ui_Fav_TextBrowser_Passage = new QTextBrowser;
    ui_Fav_TextBrowser_Passage->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Fav_TextBrowser_Passage, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    ui_Fav_TextBrowser_Passage->setFont(m_currentFont);
    ui_Fav_TextBrowser_Passage->setOpenLinks(false);
    textCommentVBoxLayout->addWidget(ui_Fav_TextBrowser_Passage);
    setBrowserBackground(*ui_Fav_TextBrowser_Passage);

    QHBoxLayout *commentDeleteSaveHBoxLayout = new QHBoxLayout;
    textCommentVBoxLayout->addLayout(commentDeleteSaveHBoxLayout);

    QLabel *commentLabel = new QLabel(tr("Comment:"));
    commentDeleteSaveHBoxLayout->addWidget(commentLabel);

    commentDeleteSaveHBoxLayout->addStretch();

    ui_Fav_Button_Delete = new QPushButton(tr("Delete"));
    connect(ui_Fav_Button_Delete, SIGNAL(clicked()),
            this, SLOT(on_Fav_ButtonClicked_Delete()));
    commentDeleteSaveHBoxLayout->addWidget(ui_Fav_Button_Delete);

    ui_Fav_Button_Save = new QPushButton(tr("Save"));
    connect(ui_Fav_Button_Save, SIGNAL(clicked()),
            this, SLOT(on_Fav_ButtonClicked_Save()));
    commentDeleteSaveHBoxLayout->addWidget(ui_Fav_Button_Save);

    ui_Fav_TextEdit_Comment = new QTextEdit;
    ui_Fav_TextEdit_Comment->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Fav_TextEdit_Comment, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
    ui_Fav_TextEdit_Comment->setFont(m_currentFont);
    textCommentVBoxLayout->addWidget(ui_Fav_TextEdit_Comment);
    setBrowserBackground(*ui_Fav_TextEdit_Comment);

    loadFavorites();
}

void MainWindow::generateSearchTabControls(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabSearchWidget = ui_TabWidget_Main->widget(idx);
    auto mainVBoxLayout = new QVBoxLayout(tabSearchWidget);
    mainVBoxLayout->setSpacing(5);
    mainVBoxLayout->setContentsMargins(10, 10, 10, 10);

    auto enterSearchSectionHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(enterSearchSectionHBoxLayout);

    auto enterSearchVBoxLayout = new QVBoxLayout;
    enterSearchSectionHBoxLayout->addLayout(enterSearchVBoxLayout);

    auto enterLabel = new QLabel(tr("Enter a Word or Phrase:"));
    enterSearchVBoxLayout->addWidget(enterLabel);

    auto enterHBoxLayout = new QHBoxLayout;
    enterSearchVBoxLayout->addLayout(enterHBoxLayout);
    ui_Sea_LineEdit_Search = new QLineEdit;
    ui_Sea_LineEdit_Search->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
    enterHBoxLayout->addWidget(ui_Sea_LineEdit_Search);
    ui_Sea_LineEdit_Search->setFocus();
    ui_Sea_Button_Search = new QPushButton(tr("Search"));
    ui_Sea_Button_Search->setToolTip(tr("Perform a search for the word/phrase."));

    enterHBoxLayout->addWidget(ui_Sea_Button_Search);
    ui_Sea_Button_Search->setDisabled(true);

    auto rangeVerLayout = new QVBoxLayout;
    enterSearchSectionHBoxLayout->addLayout(rangeVerLayout);

    ui_Sea_ComboBox_SearchFrom = new QComboBox;
    ui_Sea_ComboBox_SearchFrom->setMinimumWidth(128);
    ui_Sea_ComboBox_SearchFrom->setMaxVisibleItems(30);
    ui_Sea_ComboBox_SearchFrom->setStyleSheet(COMBOBOX_STYLE);
    ui_Sea_ComboBox_SearchFrom->addItems(m_bookNames);
    rangeVerLayout->addWidget(ui_Sea_ComboBox_SearchFrom);

    ui_Sea_ComboBox_SearchTo = new QComboBox;
    ui_Sea_ComboBox_SearchTo->setMaxVisibleItems(30);
    ui_Sea_ComboBox_SearchTo->addItems(m_bookNames);
    ui_Sea_ComboBox_SearchTo->setCurrentIndex(ui_Sea_ComboBox_SearchTo->count() - 1);
    ui_Sea_ComboBox_SearchTo->setStyleSheet(COMBOBOX_STYLE);
    rangeVerLayout->addWidget(ui_Sea_ComboBox_SearchTo);

    auto sectionVBoxLayout = new QVBoxLayout;
    enterSearchSectionHBoxLayout->addLayout(sectionVBoxLayout);

    auto sectionLabel = new QLabel(tr("Bible Section:"));
    sectionVBoxLayout->addWidget(sectionLabel);

    ui_Sea_ComboBox_Section = new QComboBox;
    ui_Sea_ComboBox_Section->setMaxVisibleItems(12);
    ui_Sea_ComboBox_Section->setStyleSheet("combobox-popup: 0;");
    ui_Sea_ComboBox_Section->setMinimumWidth(170);
    sectionVBoxLayout->addWidget(ui_Sea_ComboBox_Section);

    populateSectionNames();

    auto resultsOptionsHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(resultsOptionsHBoxLayout);

    auto resultsVBoxLayout = new QVBoxLayout;
    resultsOptionsHBoxLayout->addLayout(resultsVBoxLayout);

    auto resultsLabel = new QLabel(tr("Results:"));
    resultsVBoxLayout->addWidget(resultsLabel);

    ui_Sea_TextBrowser_Results = new QTextBrowser;
    ui_Sea_TextBrowser_Results->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui_Sea_TextBrowser_Results, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    ui_Sea_TextBrowser_Results->setFont(m_currentFont);
    ui_Sea_TextBrowser_Results->setOpenLinks(false);
    resultsVBoxLayout->addWidget(ui_Sea_TextBrowser_Results);
    setBrowserBackground(*ui_Sea_TextBrowser_Results);
    QObject::connect(ui_Sea_TextBrowser_Results, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Sea_AnchorClicked_TextBrowser_Results(QUrl)));

    auto optionsVBoxLayout = new QVBoxLayout;
    resultsOptionsHBoxLayout->addLayout(optionsVBoxLayout);

        auto optionsLabel = new QLabel(tr("Search Options:"));
        optionsVBoxLayout->addWidget(optionsLabel);

        auto translationHBoxLayout = new QHBoxLayout;
        optionsVBoxLayout->addLayout(translationHBoxLayout);

            auto translationLabel = new QLabel(tr("Translation:"));
            translationHBoxLayout->addWidget(translationLabel);

            QStringList translationNames;
            for (ModuleData md : m_modules) {
                translationNames << md.name;
            }
            ui_Sea_ComboBox_Translation = new QComboBox;
            ui_Sea_ComboBox_Translation->addItems(translationNames);
            ui_Sea_ComboBox_Translation->setCurrentIndex(ui_Bib_TabBar_Modules->currentIndex());
            ui_Sea_ComboBox_Translation->setStyleSheet(COMBOBOX_STYLE);
            translationHBoxLayout->addWidget(ui_Sea_ComboBox_Translation);
            connect(ui_Sea_ComboBox_Translation, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_Translation(int)));

        auto resPerPageHBoxLayout = new QHBoxLayout;
        optionsVBoxLayout->addLayout(resPerPageHBoxLayout);
            auto resPerPageLabel = new QLabel(tr("Results per Page:"));
            resPerPageHBoxLayout->addWidget(resPerPageLabel);
            ui_Sea_ComboBox_ResPerPage = new QComboBox;
            QStringList resPerPageOptions;
            resPerPageOptions << "10" << "15" << "25" << "30" << "40" << "50" << "75" << "100";
            ui_Sea_ComboBox_ResPerPage->addItems(resPerPageOptions);
            ui_Sea_ComboBox_ResPerPage->setMaximumWidth(45);
            ui_Sea_ComboBox_ResPerPage->setCurrentIndex(2);
            resPerPageHBoxLayout->addWidget(ui_Sea_ComboBox_ResPerPage);
        ui_Sea_CheckBox_Case = new QCheckBox(tr("Case-Sensitive"));
        ui_Sea_CheckBox_Case->setToolTip(tr("Check to make the search case-sensitive."));
        optionsVBoxLayout->addWidget(ui_Sea_CheckBox_Case);
        ui_Sea_CheckBox_WholeWords = new QCheckBox(tr("Whole Words Only"));
        ui_Sea_CheckBox_WholeWords->setToolTip(tr("Check to ignore word fragments."));
        optionsVBoxLayout->addWidget(ui_Sea_CheckBox_WholeWords);
        ui_Sea_RadioButton_Exact = new QRadioButton(tr("Exact Phrase"));
        ui_Sea_RadioButton_Exact->setToolTip(tr("Looks for the exact sequence of characters."));
        optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Exact);
        ui_Sea_RadioButton_Exact->setChecked(true);
        ui_Sea_RadioButton_All = new QRadioButton(tr("All of the Words"));
        ui_Sea_RadioButton_All->setToolTip(tr("All of the words must be present."));
        optionsVBoxLayout->addWidget(ui_Sea_RadioButton_All);
        ui_Sea_RadioButton_Any = new QRadioButton(tr("Any of the Words"));
        optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Any);
        ui_Sea_RadioButton_Strong = new QRadioButton(tr("By Strong's Number"));
        connect(ui_Sea_RadioButton_Strong, SIGNAL(toggled(bool)),
                this, SLOT(on_Sea_ButtonToggled_ByStrong(bool)));
        ui_Sea_RadioButton_Strong->setEnabled(m_modules[ui_Sea_ComboBox_Translation->currentIndex()].hasStrong);
        optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Strong);
        optionsVBoxLayout->addStretch();

    auto randomVerseHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(randomVerseHBoxLayout);
        auto randomVerseVBoxLayout = new QVBoxLayout;
        randomVerseHBoxLayout->addLayout(randomVerseVBoxLayout);
            auto prevNextHBoxLayout = new QHBoxLayout;
            randomVerseVBoxLayout->addLayout(prevNextHBoxLayout);
                ui_Sea_Button_Prev = new QPushButton("<<");
                ui_Sea_Button_Prev->setDisabled(true);
                prevNextHBoxLayout->addWidget(ui_Sea_Button_Prev);
                ui_Sea_Button_Next = new QPushButton(">>");
                ui_Sea_Button_Next->setDisabled(true);
                prevNextHBoxLayout->addWidget(ui_Sea_Button_Next);
            ui_Sea_Button_RandomVerse = new QPushButton(tr("Random Verse"));
            randomVerseVBoxLayout->addWidget(ui_Sea_Button_RandomVerse);
        ui_Sea_TextBrowser_RandomVerse = new QTextBrowser;
        ui_Sea_TextBrowser_RandomVerse->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(ui_Sea_TextBrowser_RandomVerse, SIGNAL(customContextMenuRequested(QPoint)),
                         this, SLOT(actionShowBasicContextMenu(QPoint)));
        ui_Sea_TextBrowser_RandomVerse->setFont(m_currentFont);
        ui_Sea_TextBrowser_RandomVerse->setMaximumHeight(80);
        randomVerseHBoxLayout->addWidget(ui_Sea_TextBrowser_RandomVerse);
        setBrowserBackground(*ui_Sea_TextBrowser_RandomVerse);

        connectSearchTabSignals();
}

void MainWindow::generateTopicsTab(int idx)
{
    loadBackgroundPixmap();
    QWidget *tabDictionaryWidget = ui_TabWidget_Main->widget(idx);
    auto mainHBoxLayout = new QHBoxLayout(tabDictionaryWidget);
    mainHBoxLayout->setSpacing(5);
    mainHBoxLayout->setContentsMargins(10, 10, 10, 10);

    auto topicsVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(topicsVBoxLayout);
    auto topicsLabel = new QLabel(tr("Topics:"));
    topicsVBoxLayout->addWidget(topicsLabel);
    auto topicsListWidget = new QListWidget;
    topicsListWidget->setMaximumWidth(220);
    topicsVBoxLayout->addWidget(topicsListWidget);

    auto versesVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(versesVBoxLayout);
    auto versesLabel = new QLabel(tr("Verses:"));
    versesVBoxLayout->addWidget(versesLabel);
    ui_Top_TextBrowser_Verses = new QTextBrowser;
    versesVBoxLayout->addWidget(ui_Top_TextBrowser_Verses);
    setBrowserBackground(*ui_Top_TextBrowser_Verses);
}

void MainWindow::saveSettings()
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.setValue(SET_LANGUAGE, m_language);
    settings.beginGroup(GROUP_MAIN_WINDOW);
    settings.setValue(SET_GEOMETRY, QWidget::saveGeometry());
    settings.setValue(SET_STATE, QMainWindow::saveState());
    settings.endGroup();
    settings.beginGroup(GROUP_MODULE_DATA);
    if (m_modulesFound) {
        settings.setValue(SET_INDEX, ui_Bib_TabWidget_Modules->currentIndex());
    } else {
        settings.setValue(SET_INDEX, -1);
    }
    QStringList setPassage;
    setPassage << QString::number(ui_Bib_ListWidget_Book->currentRow() + 1)
               << QString::number(ui_Bib_ListWidget_Chapter->currentRow() + 1)
               << QString::number(ui_Bib_ComboBox_VerseFrom->currentIndex() + 1)
               << QString::number(ui_Bib_ComboBox_VerseTo->currentIndex() + 1);
    settings.setValue(SET_PASSAGE, setPassage);
    settings.setValue(SET_PATHS, m_modulePathsList);
    settings.setValue(SET_REMOVED_PATHS, m_removedPathsList);
    settings.setValue(SET_COM_VERSE, m_comVerse);
    settings.endGroup();
    settings.beginGroup(GROUP_FONT_SETTINGS);
    settings.setValue(SET_FONT_FAMILY, m_currentFont.family());
    settings.setValue(SET_FONT_SIZE, m_currentFont.pointSize());
    settings.endGroup();
}

void MainWindow::formatPassage(QString &text, bool hasStrong)
{
    text.replace("<CM>", "<br>");
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    int dbIdx = ui_Bib_TabWidget_Modules->currentIndex();
    m_globalNotes[dbIdx].clear();
    int noteCount = 0;
    QRegularExpression regex("<RF>[^<]*<Rf>");
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            m_globalNotes[dbIdx] << original;
            text.replace(original, "<a href='c:" + QString::number(noteCount) +
                         "' style='text-decoration:none'><b>*</b></a> ");
            noteCount++;
        }
    }
    if (hasStrong) {
        QRegularExpression regex("<W[HG]\\d{1,4}>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QString(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
}

void MainWindow::formatScripture(QString &text,
                                 int idx,
                                 bool hasStrong,
                                 const QRegularExpression &noteRgx,
                                 const QRegularExpression &strongRgx)
{
    text.replace("<CM>", "<br>");
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    QRegularExpressionMatchIterator iter = noteRgx.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            m_globalNotes[idx] << original;
            text.replace(original, "<a href='c:" + QString::number(m_noteCount) +
                         "' style='text-decoration:none'><b>*</b></a> ");
            m_noteCount++;
        }
    }
    if (hasStrong) {
        QRegularExpressionMatchIterator iter = strongRgx.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QString(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
}

QString MainWindow::formatResult(QString &text, const QRegExp &regex, bool hasStrong)
{
    text.remove(regex);
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    text.remove("<CM>");
    if (hasStrong) {
        QRegularExpression regex("<W[^<]*>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QString(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
    return text;
}

void MainWindow::on_Bib_CurrentRowChanged_ListWidget_Book(int currentRow)
{
    populateChapterListWidget(1);
    Q_UNUSED(currentRow);
}

void MainWindow::actHistory(bool goBack)
{
    if (goBack) {
        m_psgIdx--;
    } else {
        m_psgIdx++;
    }
    m_blockHistory = true;
    blockPassageSelectionSignals(true);
    setTabBookChapterVerses(m_history[m_psgIdx], false);
    blockPassageSelectionSignals(false);
    m_blockHistory = false;
    ui_Act_Back->setEnabled(m_psgIdx > 0);
    ui_Act_Forward->setEnabled(m_psgIdx < m_history.count() - 1);
}

void MainWindow::actionBack()
{
    actHistory(true);
}

void MainWindow::actionForward()
{
    actHistory(false);
}

void MainWindow::actionPaste()
{
    if (m_textEdit) {
        m_textEdit->paste();
    } else if (m_lineEdit) {
        m_lineEdit->paste();
    }
}

void MainWindow::actionSelectAll()
{
    if (m_textBrowser) {
        m_textBrowser->selectAll();
    } else if (m_textEdit) {
        m_textEdit->selectAll();
    } else if (m_lineEdit) {
        m_lineEdit->selectAll();
    }
}

void MainWindow::on_Bib_CurrentRowChanged_ListWidget_Chapter(int currentRow)
{
    populateVersesComboBoxes(1, -1);
    Q_UNUSED(currentRow);
}

void MainWindow::mainTabWidgetCurrentChanged(int index)
{
    QWidget *wg = ui_TabWidget_Main->widget(index);
    if (wg->children().count() == 0) {
        switch (index) {
            case 1:
                generateDetailsTab(index);
                break;
            case 2:
                generateSearchTabControls(index);
                break;
            case 3:
                generateCompareTabControls(index);
                break;
            case 4:
                generateFavoritesTabControls(index);
                break;
            case 5:
                generateDictionaryTabControls(index);
                break;
            case 6:
                generateTopicsTab(index);
                break;
            default:
                QMessageBox::critical(this, tr("Error"), tr("Specified index not found."));
                break;
        }
    } else {
        switch (index) {
            case 1:
                fillDetailsTab();
                break;
            default:
                break;
        }
    }
}

void MainWindow::on_Bib_CurrentIndexChanged_ComboBox_VerseFrom(int index)
{
    if (index > ui_Bib_ComboBox_VerseTo->currentIndex()) {
        ui_Bib_ComboBox_VerseFrom->blockSignals(true);
        ui_Bib_ComboBox_VerseFrom->setCurrentIndex(ui_Bib_ComboBox_VerseTo->currentIndex());
        ui_Bib_ComboBox_VerseFrom->blockSignals(false);
    }
    loadPassage();
    //QMessageBox::information(this, "" , "verseFromComboBoxCurrentIndexChanged");
}

void MainWindow::on_Bib_CurrentIndexChanged_ComboBox_VerseTo(int index)
{
    if (index < ui_Bib_ComboBox_VerseFrom->currentIndex()) {
        ui_Bib_ComboBox_VerseTo->blockSignals(true);
        ui_Bib_ComboBox_VerseTo->setCurrentIndex(ui_Bib_ComboBox_VerseFrom->currentIndex());
        ui_Bib_ComboBox_VerseTo->blockSignals(false);
    }
    loadPassage();
    //QMessageBox::information(this, "" , "verseToComboBoxCurrentIndexChanged");
}



void MainWindow::on_Sea_ButtonClicked_PreviousResult()
{
    ui_Sea_Button_Next->setEnabled(true);
    m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
    int startIdx = qMax(0, m_crntStartRes - 2 * m_numResPerPage);
    int endIdx = startIdx + m_numResPerPage;
    displaySearchResults(startIdx, endIdx);
    m_crntStartRes = endIdx;
    ui_Sea_Button_Prev->setDisabled(startIdx == 0);
}

void MainWindow::on_Bib_ButtonClicked_PreviousChapter()
{
    if (ui_Bib_ListWidget_Chapter->currentRow() > 0) {
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->currentRow() - 1);
    } else {
        ui_Bib_ListWidget_Book->setCurrentRow(ui_Bib_ListWidget_Book->currentRow() - 1);
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->count() - 1);
    }
}

void MainWindow::on_Bib_ButtonClicked_NextChapter()
{
    if (ui_Bib_ListWidget_Chapter->currentRow() < ui_Bib_ListWidget_Chapter->count() - 1) {
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->currentRow() + 1);
    } else {
        ui_Bib_ListWidget_Book->setCurrentRow(ui_Bib_ListWidget_Book->currentRow() + 1);
    }
}

void MainWindow::on_Bib_ButtonClicked_RandomChapter()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    QSqlQuery query(m_modules[idx].database);
    query.exec("SELECT * FROM Bible"
               " ORDER BY RANDOM() LIMIT 1");
    if (query.next()) {
        QString book = query.record().value(0).toString();
        QString chapter = query.record().value(1).toString();
        query.exec("SELECT MAX(Verse) FROM Bible WHERE"
                   " Book = " + book + " AND Chapter = " + chapter);
        if (query.next()) {
            int maxVerse = query.record().value(0).toInt();
            blockPassageSelectionSignals(true);
            setTabBookChapterVerses({ idx, book.toInt(), chapter.toInt(), 1, maxVerse }, false);
            blockPassageSelectionSignals(false);
        }
    }
}

void MainWindow::on_Sea_ButtonClicked_NextResult()
{
    ui_Sea_Button_Prev->setEnabled(true);
    m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
    int endIdx = m_crntStartRes + m_numResPerPage;
    displaySearchResults(m_crntStartRes, endIdx);
    m_crntStartRes = endIdx;
    ui_Sea_Button_Next->setDisabled(endIdx >= m_resVerses.count());
}

void MainWindow::on_Sea_ButtonClicked_RandomVerse()
{
    int idx = ui_Sea_ComboBox_Translation->currentIndex();
    int min = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
    if (min < 40 && m_modules[idx].hasOldTestament) {
        QString bookFrom = QString::number(ui_Sea_ComboBox_SearchFrom->currentIndex() + 1);
        QString bookTo = QString::number(ui_Sea_ComboBox_SearchTo->currentIndex() + 1);
        QSqlQuery query(m_modules[idx].database);
        query.exec("SELECT * FROM Bible"
                   " WHERE Book >= " + bookFrom + " AND Book <= " + bookTo +
                   " ORDER BY RANDOM() LIMIT 1");
//        query.exec("SELECT * FROM table WHERE id IN (SELECT id FROM Bible"
//                   " WHERE Book >= " + bookFrom + " AND Book <= " + bookTo +
//                   " ORDER BY RANDOM() LIMIT 1)");
        if (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            QString text = formatResult(scripture, QRegExp("<RF>.*<Rf>"), m_modules[idx].hasStrong);
            text += QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                    .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
            ui_Sea_TextBrowser_RandomVerse->setHtml(text);
        }
    } else {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The selected module contains only the New Testament."));
        ui_Sea_ComboBox_Section->setCurrentIndex(7);
    }

}

void MainWindow::on_Sea_ComboBox_CurrentIndexChanged_Section(int index)
{
    switch (index) {
        case 0:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(0);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(ui_Sea_ComboBox_SearchTo->count() - 1);
            break;
        case 1:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(0);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(38);
            break;
        case 2:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(0);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(4);
            break;
        case 3:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(5);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(16);
            break;
        case 4:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(17);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(21);
            break;
        case 5:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(22);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(26);
            break;
        case 6:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(27);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(38);
            break;
        case 7:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(39);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(ui_Sea_ComboBox_SearchTo->count() - 1);
            break;
        case 8:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(39);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(43);
            break;
        case 9:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(44);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(57);
            break;
        case 10:
            ui_Sea_ComboBox_SearchFrom->setCurrentIndex(58);
            ui_Sea_ComboBox_SearchTo->setCurrentIndex(ui_Sea_ComboBox_SearchTo->count() - 1);
        default:
            break;
    }
}

void MainWindow::on_Sea_ComboBox_CurrentIndexChanged_Translation(int index)
{
    ui_Sea_RadioButton_Strong->setEnabled(m_modules[index].hasStrong);
    ui_Sea_RadioButton_Exact->setChecked(!m_modules[index].hasStrong);
}

void MainWindow::on_Bib_TabMoved_Modules(int from, int to)
{
    m_modules.swap(from, to);
    m_modulePathsList.swap(from, to);
    m_chapterBrowsers.swap(from, to);
    m_globalNotes.swap(from, to);
    m_verseMaps.swap(from, to);
}

void MainWindow::modulesTabCloseRequested(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Removal"),
                                  tr("Are you sure you want to remove this module?\n"
                                  "It will not be visible unless you re-add it manually."),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_removedPathsList << m_modulePathsList[index];
        m_modules.removeAt(index);
        m_modulePathsList.removeAt(index);
        m_chapterBrowsers.removeAt(index);
        m_globalNotes.removeAt(index);
        m_verseMaps.removeAt(index);
        ui_Bib_TabWidget_Modules->removeTab(index);
    }
}

void MainWindow::modulesTabWidgetCurrentChanged(int index)
{
    if (ui_Bib_ComboBox_VerseFrom->count() > 0 && ui_Bib_ComboBox_VerseTo->count() > 0) {
        loadPassage();
    }
    Q_UNUSED(index);
}

void closeDatabase(QSqlDatabase &db)
{
    if (db.isOpen()) {
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(db.connectionName());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    for (ModuleData md : m_modules) {
        closeDatabase(md.database);
    }
    closeDatabase(m_dbCntr);
    closeDatabase(m_dbXRef);
    closeDatabase(m_dbStrong);
    event->accept();
}

void MainWindow::blockPassageSelectionSignals(bool isBlocked)
{
    ui_Bib_ListWidget_Book->blockSignals(isBlocked);
    ui_Bib_ListWidget_Chapter->blockSignals(isBlocked);
    ui_Bib_ComboBox_VerseFrom->blockSignals(isBlocked);
    ui_Bib_ComboBox_VerseTo->blockSignals(isBlocked);
}

void MainWindow::setBrowserBackground(QTextEdit &browser)
{
    QPalette palette;
    palette.setBrush(browser.viewport()->backgroundRole(), QBrush(m_papyrusBckgrnd));
    browser.viewport()->setPalette(palette);
}

void MainWindow::loadBackgroundPixmap()
{
    if (m_papyrusBckgrnd.isNull()) {
        m_papyrusBckgrnd = QPixmap(":/img/img_res/papyrus.jpg");
    }
}

void MainWindow::addModuleLayout(int idx)
{
    QTextBrowser *chapterBrowser = new QTextBrowser(ui_Bib_TabWidget_Modules->widget(idx));
    chapterBrowser->setFont(m_currentFont);
    chapterBrowser->setOpenLinks(false);
    chapterBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    setBrowserBackground(*chapterBrowser);
    connect(chapterBrowser, SIGNAL(highlighted(QUrl)),
            this, SLOT(on_Bib_Highlighted_ChapterBrowser(QUrl)));
    connect(chapterBrowser, SIGNAL(anchorClicked(QUrl)),
            this, SLOT(on_Bib_AnchorClicked_ChapterBrowser(QUrl)));
    connect(chapterBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(on_Bib_CustomContextMenuRequested_ChapterBrowser(const QPoint &)));
    connect(chapterBrowser, SIGNAL(selectionChanged()),
            this, SLOT(on_Bib_SelectionChanged_ChapterBrowser()));
    QHBoxLayout *chapterLayout = new QHBoxLayout(ui_Bib_TabWidget_Modules->widget(idx));
    chapterLayout->setContentsMargins(5, 5, 5, 5);
    chapterLayout->addWidget(chapterBrowser);
    m_chapterBrowsers.append(chapterBrowser);
    m_globalNotes << QStringList();
    m_verseMaps << QMap<int, int>();
}

void MainWindow::getVerseRange()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    QTextCursor cur = m_chapterBrowsers[idx]->textCursor();
    QTextDocument *doc = m_chapterBrowsers[idx]->document();
    int firstBlock = doc->findBlock(cur.selectionStart()).blockNumber();
    int lastBlock = doc->findBlock(cur.selectionEnd()).blockNumber();
    int firstVerse = m_verseMaps[idx].value(firstBlock);
    int lastVerse = m_verseMaps[idx].value(lastBlock);
    m_blockRange = qMakePair(firstBlock, lastBlock);
    m_verseRange = qMakePair(firstVerse, lastVerse);
}

void MainWindow::highlightPassage(const TabBookChapterVerses &tbcvv)
{
    int idx = tbcvv.tab;
    QSqlQuery query(m_modules[idx].database);
    if (query.exec("SELECT MAX(Verse) FROM Bible WHERE"
                   " Book = " + QString::number(tbcvv.book) +
                   " AND Chapter = " + QString::number(tbcvv.chapter)) ) {
        if (query.next()) {
            ui_TabWidget_Main->setCurrentIndex(0);
            int maxVerse = query.record().value(0).toInt();
            blockPassageSelectionSignals(true);
            setTabBookChapterVerses({ idx, tbcvv.book, tbcvv.chapter, 1, maxVerse }, false);
            blockPassageSelectionSignals(false);
            QTextCursor cur = m_chapterBrowsers[idx]->textCursor();
            QTextDocument *doc = m_chapterBrowsers[idx]->document();
            int firstIdx = m_verseMaps[idx].key(tbcvv.verseFrom);
            int lastIdx = m_verseMaps[idx].key(tbcvv.verseTo);
            QTextBlock startBlock = doc->findBlockByNumber(firstIdx);
            QTextBlock endBlock = doc->findBlockByNumber(lastIdx);
            int start = startBlock.position();
            int end = endBlock.position() + endBlock.text().length();
            cur.setPosition(start, QTextCursor::MoveAnchor);
            cur.setPosition(end, QTextCursor::KeepAnchor);
            m_chapterBrowsers[idx]->setTextCursor(cur);
            m_chapterBrowsers[idx]->setFocus();
        }
    }
}

void MainWindow::action_ChapterBrowser_InsertIntoFavorites()
{
    ui_TabWidget_Main->setCurrentIndex(4);
    QSqlQuery query(m_dbUsr);
    QString book = QString::number(ui_Bib_ListWidget_Book->currentRow() + 1);
    QString chapter = QString::number(ui_Bib_ListWidget_Chapter->currentRow() + 1);
    QString verseFirst = QString::number(m_verseRange.first);
    QString verseLast = QString::number(m_verseRange.second);
    QString queryString = "SELECT Comment FROM Favorites WHERE Book = " % book %
            " AND Chapter = " % chapter %
            " AND VerseFirst = " % verseFirst %
            " AND VerseLast = " % verseLast;
    if (query.exec(queryString)) {
        if (query.next()) {
            QMessageBox::critical(this, tr("Error"), tr("An entry for this passage already exists."));
            int index = m_favorites.indexOf(TabBookChapterVerses({ 0,
                                                                   book.toInt(),
                                                                   chapter.toInt(),
                                                                   m_verseRange.first,
                                                                   m_verseRange.second }));
            ui_Fav_ListWidget_Passages->setCurrentRow(index);
            return;
        }
        queryString = "INSERT INTO Favorites (Book, Chapter, VerseFirst, VerseLast)"
                      "VALUES (" + book + ", " + chapter + ", " + verseFirst + ", " + verseLast + ")";
        query.exec(queryString);
        QString passageId = m_bookNames[ui_Bib_ListWidget_Book->currentRow()] + " " + chapter + ":" + verseFirst;
        if (m_verseRange.first != m_verseRange.second) {
            passageId += "-" + verseLast;
        }
        ui_Fav_ListWidget_Passages->addItem(passageId);
        m_favorites.append({ 0, book.toInt(), chapter.toInt(), m_verseRange.first, m_verseRange.second });
        ui_Fav_ListWidget_Passages->setCurrentRow(ui_Fav_ListWidget_Passages->count() - 1);
    }
}

void MainWindow::action_ChapterBrowser_Copy()
{
    m_chapterBrowsers[ui_Bib_TabWidget_Modules->currentIndex()]->copy();
}

void MainWindow::action_ChapterBrowser_CopyWithReference()
{
    int firstVerse = m_verseRange.first;
    int lastVerse = m_verseRange.second;
    int firstBlock = m_blockRange.first;
    int lastBlock = m_blockRange.second;
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    QTextDocument *doc = m_chapterBrowsers[idx]->document();
    QString textToCopy;
    for (int i = firstBlock; i <= lastBlock; ++i) {
        textToCopy += " " + doc->findBlockByNumber(i).text();
    }
    textToCopy = textToCopy.replace(QRegExp("\\s+"), " ").remove("*");
    QClipboard *clipboard = QApplication::clipboard();
    QString reference = "—" + m_bookNames[ui_Bib_ListWidget_Book->currentRow()] +
                        " " + ui_Bib_ListWidget_Chapter->currentItem()->text() +
                        ":" + QString::number(firstVerse);
    if (firstVerse != lastVerse) {
        reference += "-" + QString::number(lastVerse);
    }
    clipboard->setText(textToCopy.trimmed() + reference);
}

void MainWindow::action_EditMenu_CopyWithReference()
{
    getVerseRange();
    action_ChapterBrowser_CopyWithReference();
}

void MainWindow::action_ChapterBrowser_SelectAll()
{
    m_chapterBrowsers[ui_Bib_TabWidget_Modules->currentIndex()]->selectAll();
}

void MainWindow::actionClear()
{
    if (m_textEdit) {
        m_textEdit->clear();
    } else if (m_lineEdit) {
        m_lineEdit->clear();
    }
}

void MainWindow::actionCopy()
{
    if (m_textBrowser) {
        m_textBrowser->copy();
    } else if (m_textEdit) {
        m_textEdit->copy();
    } else if (m_lineEdit) {
        m_lineEdit->copy();
    }
}

void MainWindow::actionCut()
{
    if (m_textEdit) {
        m_textEdit->cut();
    } else if (m_lineEdit) {
        m_lineEdit->cut();
    }
}

void MainWindow::on_Bib_ButtonClicked_Close()
{
    ui_Bib_Label_Find->hide();
    ui_Bib_LineEdit_Find->hide();
    ui_Bib_Button_Close->hide();
}

void MainWindow::on_Bib_TextChanged_LineEdit_Find(const QString &text)
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    m_chapterBrowsers[idx]->extraSelections().clear();
    m_chapterBrowsers[idx]->moveCursor(QTextCursor::Start);
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    QTextEdit::ExtraSelection extra;
    while (m_chapterBrowsers[idx]->find(text)) {
        extra.format.setBackground(color);
        extra.cursor = m_chapterBrowsers[idx]->textCursor();
        extraSelections.append(extra);
    }
    m_chapterBrowsers[idx]->setExtraSelections(extraSelections);
    if (!text.isNull() && !text.isEmpty()) {
        extra.cursor.clearSelection();
        m_chapterBrowsers[idx]->setTextCursor(extra.cursor);
    }
}

void MainWindow::on_Com_AnchorClicked_TextBrowser_Compare(const QUrl &arg1)
{
    qDebug() << arg1.toString();
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 't') {
        highlightPassage(TabBookChapterVerses { argString.split(":")[1].toInt(),
                                                m_comVerse[0].toInt(),
                                                m_comVerse[1].toInt(),
                                                m_comVerse[2].toInt(),
                                                m_comVerse[2].toInt() });
    } else if (firstChar == 's') {
        if (!m_dbStrong.isOpen()) {
            m_dbStrong = QSqlDatabase::addDatabase("QSQLITE", "Strong");
            m_dbStrong.setDatabaseName(m_executionPath + "/dictionaries/strong_lite.dct.mybible");
            if (!m_dbStrong.open()) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
            }
        }
        PDialogStrong strongDialog(m_dbStrong, argString.split(":")[1], m_currentFont, m_papyrusBckgrnd, this);
        strongDialog.exec();
    }
}

void MainWindow::on_Dic_TextEdited_LineEdit_Number(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui_Dic_ListWidget_AllEntries->setCurrentRow(-1);
        return;
    }
    QList<QListWidgetItem *> matchList;
    matchList = ui_Dic_ListWidget_AllEntries->findItems(arg1, Qt::MatchContains);
    if (matchList.count() > 0) {
        ui_Dic_ListWidget_AllEntries->setCurrentItem(matchList[0]);
    }
}

void MainWindow::on_Dic_TextChanged_ListWidget_AllEntries(const QString &currentText)
{
    QSqlQuery query(m_dbStrong);
    QString queryString = "SELECT data FROM dictionary WHERE word = '" +  currentText + "'";
    QString definition = "<center><h2><a class='dict' href='S" + currentText +
                "' style='text-decoration:none'>" + currentText + "</a></h2></center>";
    if (query.exec(queryString)) {
        if (query.next()) {
            definition += query.record().value(0).toString();
        }
        ui_Dic_TextBrowser_Definition->setHtml(definition);
    }
}

void MainWindow::on_Dic_AnchorClicked_TextBrowser_Definition(const QUrl &arg1)
{
    QString argString = arg1.toString();
    qDebug() << argString;
    QString id = argString.left(2);
    if (id == "#d" || id == "#s") {
        QString entry = argString.right(argString.length() - 2);
        QSqlQuery query(m_dbStrong);
        QString queryString = "SELECT relativeorder, data "
                              "FROM dictionary "
                              "WHERE word = '" +  entry + "'";
        query.exec(queryString);
        if (query.next()) {
            QSqlRecord record = query.record();
            ui_Dic_TextBrowser_Definition->setHtml(record.value(1).toString());
            ui_Dic_ListWidget_AllEntries->setCurrentRow(record.value(0).toInt() - 1);
        }
    } else if (id == "#b") {
        int dbIdx = ui_Bib_TabWidget_Modules->currentIndex();
        QString entry = argString.right(argString.length() - 2);
        QStringList indices = entry.split(".");
        highlightPassage(TabBookChapterVerses { dbIdx,
                                                indices[0].toInt(),
                                                indices[1].toInt(),
                                                indices[2].toInt(),
                                                indices[2].toInt() });
    }
}

void MainWindow::on_Sea_AnchorClicked_TextBrowser_Results(const QUrl &arg1)
{
    QString argString = arg1.toString();
    int dbIdx = ui_Bib_TabWidget_Modules->currentIndex();
    QStringList indices = argString.split(",");
    highlightPassage(TabBookChapterVerses { dbIdx,
                                            indices[0].toInt(),
                                            indices[1].toInt(),
                                            indices[2].toInt(),
                                            indices[2].toInt() });
}

void MainWindow::actionFind()
{
    if (ui_Bib_LineEdit_Find == nullptr) {
        QHBoxLayout *findHorLayout = new QHBoxLayout;

        ui_Bib_Label_Find = new QLabel(tr("Find:"));
        findHorLayout->addWidget(ui_Bib_Label_Find);

        ui_Bib_LineEdit_Find = new QLineEdit;
        ui_Bib_LineEdit_Find->setClearButtonEnabled(true);
        findHorLayout->addWidget(ui_Bib_LineEdit_Find);
        connect(ui_Bib_LineEdit_Find, SIGNAL(textChanged(QString)),
                this, SLOT(on_Bib_TextChanged_LineEdit_Find(QString)));

        ui_Bib_Button_Close = new QPushButton;
        ui_Bib_Button_Close->setIcon(QIcon(ICON_CLOSE));
        ui_Bib_Button_Close->setMaximumSize(QSize(22, 22));
        findHorLayout->addWidget(ui_Bib_Button_Close);
        connect(ui_Bib_Button_Close, SIGNAL(clicked()),
                this, SLOT(on_Bib_ButtonClicked_Close()));

        ui_Bib_VerLayout_Modules->addLayout(findHorLayout);
        ui_Bib_LineEdit_Find->setFocus();
    } else {
        ui_Bib_Label_Find->show();
        ui_Bib_LineEdit_Find->show();
        ui_Bib_Button_Close->show();
        ui_Bib_LineEdit_Find->setFocus();
        ui_Bib_LineEdit_Find->selectAll();
    }
}

void MainWindow::on_Sea_LineEdit_TextChanged_Search(const QString &text)
{
    ui_Sea_Button_Search->setEnabled(text.trimmed().length() > 0);
}

void MainWindow::on_Fav_CurrentRowChanged_ListWidget_Passages(int currentRow)
{
    QString book = QString::number(m_favorites[currentRow].book);
    QString chapter = QString::number(m_favorites[currentRow].chapter);
    QString verseFirst = QString::number(m_favorites[currentRow].verseFrom);
    QString verseLast = QString::number(m_favorites[currentRow].verseTo);
    QString queryString = "SELECT Comment FROM Favorites WHERE Book = " + book +
            " AND Chapter = " + chapter +
            " AND VerseFirst = " + verseFirst +
            " AND VerseLast = " + verseLast;
    QSqlQuery query(m_dbUsr);
    if (!query.exec(queryString))
        return;
    if (query.next())
        ui_Fav_TextEdit_Comment->setText(query.record().value(0).toString());
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    query = QSqlQuery(m_modules[idx].database);
    queryString = "SELECT Verse, Scripture FROM Bible WHERE Book = " + book +
            " AND Chapter = " + chapter +
            " AND Verse >= " + verseFirst +
            " AND Verse <= " + verseLast;
    if (!query.exec(queryString))
        return;
    QString passage;
    while (query.next()) {
        QSqlRecord record = query.record();
        QString verse = record.value(0).toString();
        QString scripture = record.value(1).toString();
        passage += " <b>" + verse + "</b>" + " " + scripture;
    }
    if (passage.isNull()) {
        ui_Fav_TextBrowser_Passage->setHtml("<center><h2>" + tr("Unavailable in this module.") + "</center></h2>");
        return;
    }
    QRegExp rgx("<RF>.*<Rf>");
    rgx.setMinimal(true);
    passage = formatResult(passage, rgx, m_modules[idx].hasStrong).trimmed();
    passage += "<b>—" + m_bookNames[m_favorites[currentRow].book - 1] + " " +
            chapter + ":" + verseFirst;
    if (verseFirst != verseLast) {
        passage += "-" + verseLast + "</b>";
    } else {
        passage += "</b>";
    }
    ui_Fav_TextBrowser_Passage->setHtml(passage);
    ui_Fav_Button_Delete->setEnabled(true);
    ui_Fav_Button_Save->setEnabled(true);
}

void MainWindow::on_Fav_ButtonClicked_Delete()
{
    int index = ui_Fav_ListWidget_Passages->currentRow();
    QString passageId = ui_Fav_ListWidget_Passages->currentItem()->text();
    QMessageBox questionMsgBox(QMessageBox::Question,
                               tr("Confirm Deletion"),
                               tr("Delete entry ") + passageId + "?",
                               QMessageBox::Yes | QMessageBox::No);
    questionMsgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    questionMsgBox.setButtonText(QMessageBox::No, tr("No"));
    if (questionMsgBox.exec() == QMessageBox::Yes) {
        QSqlQuery query(m_dbUsr);
        QString book = QString::number(m_favorites[index].book);
        QString chapter = QString::number(m_favorites[index].chapter);
        QString verseFirst = QString::number(m_favorites[index].verseFrom);
        QString verseLast = QString::number(m_favorites[index].verseTo);
        QString queryString = "DELETE FROM Favorites WHERE Book = " + book +
                             " AND Chapter = " + chapter +
                             " AND VerseFirst = " + verseFirst +
                             " AND VerseLast = " + verseLast;
        if (query.exec(queryString)) {
            m_favorites.removeAt(index);
            ui_Fav_ListWidget_Passages->blockSignals(true);
            delete ui_Fav_ListWidget_Passages->currentItem();
            ui_Fav_TextBrowser_Passage->clear();
            ui_Fav_TextEdit_Comment->clear();
            if (index > 0) {
                on_Fav_CurrentRowChanged_ListWidget_Passages(index - 1);
            } else if (index == 0 && ui_Fav_ListWidget_Passages->count() > 0) {
                qDebug() << "hi";
                on_Fav_CurrentRowChanged_ListWidget_Passages(0);
            }
            ui_Fav_ListWidget_Passages->blockSignals(false);
            ui_Fav_Button_Delete->setEnabled(ui_Fav_ListWidget_Passages->count() > 0);
            ui_Fav_Button_Save->setEnabled(ui_Fav_ListWidget_Passages->count() > 0);
        }
    }
}

void MainWindow::on_Fav_ButtonClicked_Save()
{
    int index = ui_Fav_ListWidget_Passages->currentRow();
    QString book = QString::number(m_favorites[index].book);
    QString chapter = QString::number(m_favorites[index].chapter);
    QString verseFirst = QString::number(m_favorites[index].verseFrom);
    QString verseLast = QString::number(m_favorites[index].verseTo);
    QString comment = ui_Fav_TextEdit_Comment->toPlainText();
    QString queryString = "UPDATE Favorites SET Comment = '" + comment + "'" +
                          " WHERE Book = " + book +
                          " AND Chapter = " + chapter +
                          " AND VerseFirst = " + verseFirst +
                          " AND VerseLast = " + verseLast;
    QSqlQuery query(m_dbUsr);
    if (query.exec(queryString)) {
        statusBar()->showMessage(tr("Entry updated."), 2500);
    }
}

void MainWindow::actionShowBasicContextMenu(const QPoint &pos)
{
    m_textBrowser = qobject_cast<QTextBrowser *>(QObject::sender());
    m_textEdit = nullptr;
    m_lineEdit = nullptr;
    QPoint globalPos = m_textBrowser->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = m_textBrowser->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[2]->setDisabled(m_textBrowser->toPlainText().isEmpty());
    contextMenu.exec(globalPos);
}

void MainWindow::actionShowEditContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Cut"),
                          this,
                          SLOT(actionCut()),
                          QKeySequence("Ctrl+X"));
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addAction(tr("Paste"),
                          this,
                          SLOT(actionPaste()),
                          QKeySequence("Ctrl+V"));
    contextMenu.addAction(tr("Clear"),
                          this,
                          SLOT(actionClear()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    QList<QAction *> contextActions = contextMenu.actions();
    QString senderName = QObject::sender()->metaObject()->className();
    QPoint globalPos;
    if (senderName == "QLineEdit") {
        m_textEdit = nullptr;
        m_textBrowser = nullptr;
        m_lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
        globalPos = m_lineEdit->mapToGlobal(pos);
        bool isSelected = (m_lineEdit->selectedText().length() > 0);
        contextActions[0]->setEnabled(isSelected);
        contextActions[1]->setEnabled(isSelected);
        bool isEmpty = m_lineEdit->text().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    } else if (senderName == "QTextEdit") {
        m_lineEdit = nullptr;
        m_textBrowser = nullptr;
        m_textEdit = qobject_cast<QTextEdit *>(QObject::sender());
        globalPos = m_textEdit->mapToGlobal(pos);
        QTextCursor cursor = m_textEdit->textCursor();
        contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        contextActions[1]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        bool isEmpty = m_textEdit->toPlainText().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    }
    contextActions[2]->setDisabled(qApp->clipboard()->text().isEmpty());
    contextMenu.exec(globalPos);
}

void MainWindow::actionShowLineContextMenu(const QPoint &pos)
{
    m_textBrowser = nullptr;
    m_textEdit = nullptr;
    m_lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
    QPoint globalPos = m_lineEdit->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    QList<QAction *> contextActions = contextMenu.actions();
    contextActions[0]->setDisabled(m_lineEdit->selectionLength() == 0);
    contextActions[2]->setDisabled(m_lineEdit->text().isEmpty() ||
                                   m_lineEdit->selectionLength() == m_lineEdit->text().length());
    contextMenu.exec(globalPos);
}

void MainWindow::on_Sea_LineEdit_ReturnPressed_Search()
{
    ui_Sea_Button_Search->click();
}

void MainWindow::on_Bib_CustomContextMenuRequested_ChapterBrowser(const QPoint &pos)
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    QPoint globalPos = m_chapterBrowsers[idx]->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(QIcon(ICON_COPY), tr("Copy"), this,
                          SLOT(action_ChapterBrowser_Copy()), QKeySequence("Ctrl+C"));
    contextMenu.addAction(QIcon(ICON_COPY_PLUS), tr("Copy with Reference"), this,
                          SLOT(action_ChapterBrowser_CopyWithReference()));
    contextMenu.addAction(tr("Select All"), this,
                          SLOT(action_ChapterBrowser_SelectAll()), QKeySequence("Ctrl+A"));
    contextMenu.addSeparator();
    getVerseRange();
    QString addVerseMsg = m_verseRange.first == m_verseRange.second ?
                tr("Add Verse ") + QString::number(m_verseRange.first) + tr(" to Favorites") :
                tr("Add Verses ") + QString::number(m_verseRange.first) +
                "–" + QString::number(m_verseRange.second) + tr(" to Favorites");
    contextMenu.addAction(QIcon(ICON_HEART), addVerseMsg, this, SLOT(action_ChapterBrowser_InsertIntoFavorites()));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(ICON_ARROW_LEFT), tr("Back"), this,
                          SLOT(actionBack()), QKeySequence(tr("Ctrl+Left")));
    contextMenu.addAction(QIcon(ICON_ARROW_RIGHT), tr("Forward"), this,
                          SLOT(actionForward()), QKeySequence(tr("Ctrl+Right")));
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = m_chapterBrowsers[idx]->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[1]->setEnabled(contextActions[0]->isEnabled());
    contextActions[4]->setDisabled(m_verseRange.first == 0 && m_verseRange.second == 0);
    contextActions[6]->setEnabled(ui_Act_Back->isEnabled());
    contextActions[7]->setEnabled(ui_Act_Forward->isEnabled());
    contextMenu.exec(globalPos);
}

QStringList MainWindow::getModulePaths(const QString &path)
{
    QDir dir(path);
    QStringList filters;
    filters << "*.bbl.mybible";
    dir.setNameFilters(filters);
    QFileInfoList moduleList = dir.entryInfoList();
    QStringList modulePathList;
    foreach (QFileInfo file, moduleList) {
        modulePathList << file.absoluteFilePath();
    }
    return modulePathList;
}


void MainWindow::populateSectionNames()
{
    if (!m_sectionNames.isEmpty()) {
        m_sectionNames.clear();
    }
    m_sectionNames << tr("Entire Bible")
                   << tr("Old Testament")
                   << tr("Pentateuch")
                   << tr("Historical Books")
                   << tr("Poetical Books")
                   << tr("Major Prophets")
                   << tr("Minor Prophets")
                   << tr("New Testament")
                   << tr("Gospels & Acts")
                   << tr("Pauline Epistles")
                   << tr("General Epistles & Revelation")
                   << tr("Custom Range");
    ui_Sea_ComboBox_Section->addItems(m_sectionNames);
}

void MainWindow::actionOpenBibleModule()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open MYBIBLE Module"), "/",
                tr("MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)"));
    if (!fileName.isNull() && !fileName.isEmpty()) {
        if (ui_Bib_TabWidget_Modules->tabText(0) == "No module found") {
            ui_Bib_TabWidget_Modules->removeTab(0);
        }
        if (!m_modulePathsList.contains(fileName)) {
            if (loadBibleModule(fileName)) {
                m_modulePathsList << fileName;
                ui_Bib_TabWidget_Modules->setCurrentIndex(ui_Bib_TabWidget_Modules->count() - 1);
                if (m_removedPathsList.contains(fileName)) {
                    m_removedPathsList.removeAt(m_removedPathsList.indexOf(fileName));
                }
            }
        } else {
            QMessageBox::critical(this, tr("Error"), tr("The selected file is already open."));
        }
    }
}

bool MainWindow::loadBibleModule(const QString &path)
{
    QSqlDatabase dbBbl = QSqlDatabase::addDatabase("QSQLITE", path);
    dbBbl.setDatabaseName(path);
    dbBbl.open();
    QSqlQuery query(dbBbl);
    QString moduleName;
    bool hasOldTestament = false;
    bool hasStrong = false;
    QString queryString = "SELECT Abbreviation, OT, Strong FROM Details";
    query.exec(queryString);
    if (query.next()) {
        QSqlRecord record = query.record();
        moduleName = record.value(0).toString();
        hasOldTestament = record.value(1).toBool();
    }
    queryString = "SELECT Scripture FROM Bible";
    query.exec(queryString);
    if (query.next()) {
        hasStrong = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
    }
    bool containsModule = false;
    foreach (ModuleData md, m_modules) {
        if (moduleName == md.name) {
            containsModule = true;
            QMessageBox::critical(this, tr("Error"), tr("A module with this name is already open."));
            break;
        }
    }
    if (!containsModule) {
        m_modules.append({ dbBbl, moduleName, hasOldTestament, hasStrong });
        int idx = m_modules.count() - 1;
        ui_Bib_TabWidget_Modules->addTab(new QWidget(), m_modules[idx].name);
        addModuleLayout(idx);
    }
    return !containsModule;
}

void MainWindow::on_Bib_Highlighted_ChapterBrowser(const QUrl &arg1)
{
    QString argString = arg1.toString();
    if (!argString.isNull()) {
        QStringList argSplit = argString.split(":");
        if (argSplit[0] == "c") {
            int idx = ui_Bib_TabWidget_Modules->currentIndex();
            QPoint point = m_chapterBrowsers[idx]->mapFromParent(QCursor::pos());
            double offset = 3.4 * m_currentFont.pointSize();
            point.setY(point.y() - offset);
            QRect rect;
            int subIdx = argSplit[1].toInt();
            QString markupText = m_globalNotes[idx][subIdx];
            QString plainText = markupText.mid(4, markupText.size() - 8);
            plainText.replace("[i]", "<i>").replace("[/i]", "</i>");
            // plainText.replace("[0A]", "<font color=#00000a>").replace("[0a]", "</font>");
            QString note = "<p style='white-space:pre'>" + plainText + "</p>";
            QToolTip::setFont(m_currentFont);
            QToolTip::showText(point, note, 0, rect, 2147483647);
        }
    } else {
        QToolTip::hideText();
    }
}

void MainWindow::loadXRefDatabase()
{
    QString path = m_executionPath + "/data/xref.bblv";
    if (QFileInfo(path).exists()) {
        if (!m_dbXRef.isOpen()) {
            m_dbXRef = QSqlDatabase::addDatabase("QSQLITE", "CrossReferences");
            m_dbXRef.setDatabaseName(path);
            m_dbXRef.open();
        }
    }
}

void MainWindow::updateFonts()
{
    for (QTextBrowser *tb : m_chapterBrowsers) {
        tb->setFont(m_currentFont);
    }
    if (ui_TabWidget_Main->widget(1)->children().count() > 0) {
        ui_Det_TextBrowser_Description->setFont(m_currentFont);
        ui_Det_TextBrowser_Comments->setFont(m_currentFont);
    }
    if (ui_TabWidget_Main->widget(2)->children().count() > 0) {
        ui_Sea_TextBrowser_Results->setFont(m_currentFont);
        ui_Sea_TextBrowser_RandomVerse->setFont(m_currentFont);
    }
    if (ui_TabWidget_Main->widget(3)->children().count() > 0) {
        ui_Com_TextBrowser_Compare->setFont(m_currentFont);
    }
    if (ui_TabWidget_Main->widget(4)->children().count() > 0) {
        ui_Fav_TextBrowser_Passage->setFont(m_currentFont);
        ui_Fav_TextEdit_Comment->setFont(m_currentFont);
    }
    if (ui_TabWidget_Main->widget(5)->children().count() > 0) {
        ui_Dic_TextBrowser_Definition->setFont(m_currentFont);
    }
}

void MainWindow::on_Bib_AnchorClicked_ChapterBrowser(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        if (!m_dbStrong.isOpen()) {
            m_dbStrong = QSqlDatabase::addDatabase("QSQLITE", "Strong");
            m_dbStrong.setDatabaseName(m_executionPath + "/dictionaries/strong_lite.dct.mybible");
            if (!m_dbStrong.open()) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
            }
        }
        PDialogStrong strongDialog(m_dbStrong, argString, m_currentFont, m_papyrusBckgrnd, this);
        strongDialog.exec();
    } else if (firstChar == 'x') {
        int idx = ui_Bib_TabWidget_Modules->currentIndex();
        QStringList verseInfo;
        verseInfo << argString
                  << QString::number(ui_Bib_ListWidget_Book->currentRow())
                  << QString::number(ui_Bib_ListWidget_Chapter->currentItem()->text().toInt());
        PDialogXRef dlgXRef(m_modules[idx].database,
                            verseInfo,
                            m_bookNames,
                            m_papyrusBckgrnd,
                            m_currentFont);
        dlgXRef.exec();
    }
}

void MainWindow::actionExit()
{
    QMainWindow::close();
    QApplication::quit();
}

void MainWindow::actionWordFrequency()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    PWindowHistogram *histogramWindow = new PWindowHistogram(m_modules[idx].database);
    histogramWindow->show();
}

void MainWindow::checkFontSizes()
{
    ui_ActIncreaseFont->setDisabled(m_currentFont.pointSize() >= 20);
    ui_ActDecreaseFont->setDisabled(m_currentFont.pointSize() <= 8);
}

void MainWindow::actionIncreaseFontSize()
{
    m_currentFont.setPointSize(m_currentFont.pointSize() + 1);
    updateFonts();
    checkFontSizes();
}

void MainWindow::actionDecreaseFontSize()
{
    m_currentFont.setPointSize(m_currentFont.pointSize() - 1);
    updateFonts();
    checkFontSizes();
}

void MainWindow::checkLanguageAction(int idx, bool firstRun)
{
    QList<QAction *> languageActions = ui_Menu_Language->actions();
    for (int i = 0; i < languageActions.count(); ++i) {
        languageActions[i]->setChecked(i == idx);
    }
    m_language = m_languages.value(idx);
    if (!firstRun) {
        QMessageBox::information(this,
                                 tr("Language Change"),
                                 tr("The change will take effect after restarting the program."));
    }
}

void MainWindow::actionEnglish()
{
    checkLanguageAction(0);
}

void MainWindow::actionPolish()
{
    checkLanguageAction(2);
}

void MainWindow::actionSpanish()
{
    checkLanguageAction(1);
}

void MainWindow::actionPreferences()
{
    PDialogPreferences dlgPreferences(m_currentFont);
    if (dlgPreferences.exec()) {
        m_currentFont.setFamily(dlgPreferences.getFontFamily());
        m_currentFont.setPointSize(dlgPreferences.getFontSize());
        updateFonts();
    }
}


void MainWindow::actionAboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

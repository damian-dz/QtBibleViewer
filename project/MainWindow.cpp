#include "MainWindow.h"
#include "PDialogPreferences.h"
#include "PDialogStrong.h"
#include "PDialogXRef.h"
#include "PWindowHistogram.h"

inline void createFavDatabase(QSqlDatabase &db, const QString &filename)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);
    if (db.open()) {
        QSqlQuery query(db);
        query.exec("CREATE TABLE Favorites (Book INT, Chapter INT, VerseFirst INT, VerseLast INT, Comment TEXT)");
        query.exec("CREATE UNIQUE INDEX \"fav_key\" ON \"Favorites\" "
                   "(\"Book\" ASC, \"Chapter\" ASC, \"VerseFirst\" ASC, \"VerseLast\" ASC)");
    }
}

inline bool isBetween(int nmbr, int frst, int scnd)
{
    return nmbr >= frst && nmbr <= scnd;
}

inline bool isContainedInSelections(const QTextEdit::ExtraSelection &sel,
                                    const QList<QTextEdit::ExtraSelection> &extraSelections)
{
    bool res = false;
    for (int i = 0; i < extraSelections.count(); ++i) {
        if (isBetween(sel.cursor.position(),
                      extraSelections[i].cursor.position() -
                      extraSelections[i].cursor.selectedText().length(),
                      extraSelections[i].cursor.position())) {
            res = true;
            break;
        }
    }
    return res;
}

inline void removeBrowserBackground(QTextEdit &browser)
{
    browser.viewport()->setPalette(browser.style()->standardPalette());
}

MainWindow::MainWindow(const QString &appDir,
                       const QString &lang,
                       QTranslator &appTs,
                       QTranslator &qtTs,
                       const QString &style,
                       const QString configPath,
                       QWidget *parent)
    : QMainWindow(parent),
      m_textBrowser(nullptr),
      m_textEdit(nullptr),
      m_lineEdit(nullptr),
      m_labels(QVector<QLabel *>(26)),
      ui_Bib_LineEdit_Find(nullptr),
      m_blockHistory(false),
      m_executionPath(appDir),
      m_firstLoadCompare(true),
      m_language(lang),
      m_settingsPath(configPath),
      m_style(style),
      m_tsApp(&appTs),
      m_tsQt(&qtTs)
{
    TabBookChapterVerses tbcvv = loadSettings();
    generateMainLayout();
    populateLanguageMap(lang);
    generateBibModuleTabs();
    populateBookNames();   
    loadDictPaths();
    if (tbcvv.isPassageValid()) {
        setTabBookChapterVerses(tbcvv, true);
    }
    connectBibleTabSignals();
    if (!tbcvv.isPassageValid() && m_modulesFound) {
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
    QMenu *fileMenu = menuBar()->addMenu(QString());
    m_menus << fileMenu;
    m_actions << fileMenu->addAction(QIcon(ICON_FOLDER), QString(), this,
                &MainWindow::actionOpenBibleModule, QKeySequence::Open);

    fileMenu->addSeparator();

    m_actions << fileMenu->addAction(QIcon(ICON_EXIT), QString(), this,
                &MainWindow::actionExit, QKeySequence("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(QString());
    m_menus << editMenu;

    ui_Act_Back = editMenu->addAction(QIcon(ICON_ARROW_LEFT),
                QString(), this, &MainWindow::actionBack);
    ui_Act_Back->setDisabled(true);

    ui_Act_Forward = editMenu->addAction(QIcon(ICON_ARROW_RIGHT),
                QString(), this, &MainWindow::actionForward);
    ui_Act_Forward->setDisabled(true);

    editMenu->addSeparator();

    ui_Act_Copy = editMenu->addAction(QIcon(ICON_COPY),
                QString(), this, &MainWindow::action_ChapterBrowser_Copy, QKeySequence::Copy);
    ui_Act_Copy->setDisabled(true);
    ui_Act_CopyWithRef = editMenu->addAction(QIcon(ICON_COPY_PLUS),
                QString(), this, &MainWindow::action_EditMenu_CopyWithReference);
    ui_Act_CopyWithRef->setDisabled(true);

    editMenu->addSeparator();

    m_actions << editMenu->addAction(QString(), this, &MainWindow::action_ChapterBrowser_SelectAll,
                        QKeySequence("Ctrl+A"));
    m_actions << editMenu->addAction(QIcon(ICON_FIND),
                        QString(), this, &MainWindow::actionFind, QKeySequence("Ctrl+F"));

    QMenu *statisticsMenu = menuBar()->addMenu(QString());
    m_menus << statisticsMenu;
    m_actions << statisticsMenu->addAction(QString(), this, &MainWindow::actionWordFrequency);

    QMenu *optionsMenu = menuBar()->addMenu(QString());
    m_menus << optionsMenu;
    m_actions << optionsMenu->addAction(QString(), this, &MainWindow::actionPreferences);

    ui_Menu_Language = optionsMenu->addMenu(QString());
    QAction *englishAct = ui_Menu_Language->addAction(
               "English", this, &MainWindow::actionEnglish);
    englishAct->setCheckable(true);
    QAction *spanishAct = ui_Menu_Language->addAction(
               "español", this, &MainWindow::actionSpanish);
    spanishAct->setCheckable(true);
    QAction *polishAct = ui_Menu_Language->addAction(
               "polski", this, &MainWindow::actionPolish);
    polishAct->setCheckable(true);

    QMenu *viewMenu = menuBar()->addMenu(QString());
    m_menus << viewMenu;
    ui_ActIncreaseFont = viewMenu->addAction(QIcon(ICON_MAGNIFY), QString(), this,
                &MainWindow::actionIncreaseFontSize, QKeySequence::ZoomIn);
    ui_ActDecreaseFont = viewMenu->addAction(QIcon(ICON_MINIFY), QString(), this,
                &MainWindow::actionDecreaseFontSize, QKeySequence::ZoomOut);

    QMenu *helpMenu = menuBar()->addMenu(QString());
    m_menus << helpMenu;
    m_actions << helpMenu->addAction(QString(), this, &MainWindow::actionHelp, QKeySequence::HelpContents);
    m_actions << helpMenu->addAction(QString(), this, &MainWindow::actionAbout);
    m_actions << helpMenu->addAction(QString(), this, &MainWindow::actionAboutQt);

    setMenuTexts();

    ui_StatusBar_Status = new QStatusBar;
    ui_Label_Status = new QLabel;
    ui_StatusBar_Status->addWidget(ui_Label_Status);
    QMainWindow::setStatusBar(ui_StatusBar_Status);
}

void MainWindow::addMainTabs()
{
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    ui_TabWidget_Main->addTab(new QWidget, QString());
    setMainTabNames();

    ui_TabBar_Main = ui_TabWidget_Main->tabBar();
    setMainTabToolTips();
}

void MainWindow::setMenuTexts()
{
    m_menus[0]->setTitle(tr("File"));
    m_actions[0]->setText(tr("Open Bible Module"));
    m_actions[1]->setText(tr("Exit"));
    m_menus[1]->setTitle(tr("Edit"));
    ui_Act_Back->setText(tr("Back"));
    ui_Act_Forward->setText(tr("Forward"));
    ui_Act_Copy->setText(tr("Copy"));
    ui_Act_CopyWithRef->setText(tr("Copy with Reference"));
    m_actions[2]->setText(tr("Select All"));
    m_actions[3]->setText(tr("Find"));
    m_menus[2]->setTitle(tr("Statistics"));
    m_actions[4]->setText(tr("Word Frequency"));
    m_menus[3]->setTitle(tr("Options"));
    ui_Menu_Language->setTitle(tr("Language"));
    m_actions[5]->setText(tr("Preferences"));
    m_menus[4]->setTitle(tr("View"));
    ui_ActIncreaseFont->setText(tr("Increase Font Size"));
    ui_ActDecreaseFont->setText(tr("Decrease Font Size"));
    m_menus[5]->setTitle(tr("Help"));
    m_actions[6]->setText(tr("Show Help"));
    m_actions[7]->setText(tr("About"));
    m_actions[8]->setText(tr("About Qt"));
}

void MainWindow::setMainTabNames()
{
    ui_TabWidget_Main->setTabText(0, tr("Bible"));
    ui_TabWidget_Main->setTabText(1, tr("Details"));
    ui_TabWidget_Main->setTabText(2, tr("Search"));
    ui_TabWidget_Main->setTabText(3, tr("Compare"));
    ui_TabWidget_Main->setTabText(4, tr("Favorites"));
    ui_TabWidget_Main->setTabText(5, tr("Dictionary"));
    ui_TabWidget_Main->setTabText(6, tr("Topics"));
}

void MainWindow::setLabelTexts(int idx)
{
    if (idx == -1 || idx == 0) {
        m_labels[0]->setText(tr("Book:"));
        m_labels[1]->setText(tr("Chapter:"));
        m_labels[2]->setText(tr("Verses:"));
    }
    if (idx == -1 || idx == 1) {
        if (ui_TabWidget_Main->widget(1)->children().count() > 0) {
            m_labels[3]->setText(tr("Description:"));
            m_labels[4]->setText(tr("Abbreviation:"));
            m_labels[5]->setText(tr("Comments:"));
            m_labels[6]->setText(tr("Version:"));
            m_labels[7]->setText(tr("Version Date:"));
            m_labels[8]->setText(tr("Publish Date:"));
        }
    }
    if (idx == -1 || idx == 2) {
        if (ui_TabWidget_Main->widget(2)->children().count() > 0) {
            m_labels[9]->setText(tr("Enter a Word or Phrase:"));
            m_labels[10]->setText(tr("Bible Section:"));
            m_labels[11]->setText(tr("Results:"));
            m_labels[12]->setText(tr("Search Options:"));
            m_labels[13]->setText(tr("Translation:"));
            m_labels[14]->setText(tr("Results per Page:"));
        }
    }
    if (idx == -1 || idx == 3) {
        if (ui_TabWidget_Main->widget(3)->children().count() > 0) {
            m_labels[15]->setText(tr("Book:"));
            m_labels[16]->setText(tr("Chapter:"));
            m_labels[17]->setText(tr("Verse:"));
        }
    }
    if (idx == -1 || idx == 4) {
        if (ui_TabWidget_Main->widget(4)->children().count() > 0) {
            m_labels[18]->setText(tr("Favorite Passages:"));
            m_labels[19]->setText(tr("Comment:"));
        }
    }
    if (idx == -1 || idx == 5) {
        if (ui_TabWidget_Main->widget(5)->children().count() > 0) {
            m_labels[20]->setText(tr("Available Dictionaries:"));
            m_labels[21]->setText(tr("Number:"));
            m_labels[22]->setText(tr("All Entries:"));
            m_labels[23]->setText(tr("Definition:"));
        }
    }
}

void MainWindow::setButtonTexts(int idx)
{
    if (idx == -1 || idx == 0) {
        ui_Bib_Button_Random->setText(tr("Random"));
        ui_Bib_Button_Random->setToolTip(tr("Go to a random chapter."));
        ui_Bib_Button_Prev->setToolTip(tr("Go to the previous chapter."));
        ui_Bib_Button_Next->setToolTip(tr("Go to the next chapter."));
    }
    if (idx == -1 || idx == 2) {
        if (ui_TabWidget_Main->widget(2)->children().count() > 0) {
            ui_Sea_Button_Search->setText(tr("Search"));
            ui_Sea_Button_Search->setToolTip(tr("Perform a search for the word/phrase."));
            ui_Sea_RadioButton_Exact->setText(tr("Exact Phrase"));
            ui_Sea_RadioButton_Exact->setToolTip(tr("Look for verses that contain the exact sequence of characters."));
            ui_Sea_RadioButton_All->setText(tr("All of the Words"));
            ui_Sea_RadioButton_All->setToolTip(tr("Look for verses that contain all of the words (in any order)."));
            ui_Sea_RadioButton_Any->setText(tr("Any of the Words"));
            ui_Sea_RadioButton_Any->setToolTip(tr("Look for verses that contain at least one of the words."));
            ui_Sea_RadioButton_Strong->setText(tr("By Strong's Number"));
            ui_Sea_RadioButton_Strong->setToolTip(tr("Look for verses containing the specified Strong's Number (if available)."));
            ui_Sea_Button_RandomVerse->setText(tr("Random Verse"));
        }
    }
    if (idx == -1 || idx == 4) {
        if (ui_TabWidget_Main->widget(4)->children().count() > 0) {
            ui_Fav_Button_Delete->setText(tr("Delete"));
            ui_Fav_Button_Save->setText(tr("Save"));
        }
    }
}

void MainWindow::setCheckBoxTexts(int idx)
{
    if (idx == -1 || idx == 1) {
        if (ui_TabWidget_Main->widget(1)->children().count() > 0) {
            ui_Det_CheckBox_RightToLeft->setText(tr("Right to Left"));
            ui_Det_CheckBox_OldTestament->setText(tr("Old Testament"));
            ui_Det_CheckBox_NewTestament->setText(tr("New Testament"));
            ui_Det_CheckBox_StrongsNumbers->setText(tr("Strong's Numbers"));
        }
    }
    if (idx == -1 || idx == 2) {
        if (ui_TabWidget_Main->widget(2)->children().count() > 0) {
            ui_Sea_ComboBox_Section->setToolTip(tr("Limit the search to a specific Bible section."));
            ui_Sea_CheckBox_Case->setText(tr("Case-Sensitive"));
            ui_Sea_CheckBox_Case->setToolTip(tr("Make the search case-sensitive."));
            ui_Sea_CheckBox_WholeWords->setText(tr("Whole Words Only"));
            ui_Sea_CheckBox_WholeWords->setToolTip(tr("Ignore word fragments."));
        }
    }
}

void MainWindow::setMainTabToolTips()
{
    ui_TabBar_Main->setTabToolTip(0, tr("Read different translations of the Bible."));
    ui_TabBar_Main->setTabToolTip(1, tr("View the details of the currently active module."));
    ui_TabBar_Main->setTabToolTip(2, tr("Perform a custom search on one of the available modules."));
    ui_TabBar_Main->setTabToolTip(3, tr("Compare verses from the currently available modules."));
    ui_TabBar_Main->setTabToolTip(4, tr("Manage your favorite passages and add comments."));
}

void MainWindow::generateBibleTabControls()
{
    QWidget *tabBibleWidget = ui_TabWidget_Main->widget(0);

    QHBoxLayout *tabBibleHorLayout = new QHBoxLayout;
    tabBibleHorLayout->setSpacing(5);
    tabBibleHorLayout->setContentsMargins(10, 10, 10, 10);
    tabBibleWidget->setLayout(tabBibleHorLayout);

    QVBoxLayout *bookVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(bookVBoxLayout);

    QLabel *bookLabel = new QLabel;
    m_labels[0] = bookLabel;
    bookVBoxLayout->addWidget(bookLabel);

    ui_Bib_ListWidget_Book = new QListWidget;
    ui_Bib_ListWidget_Book->setMaximumSize(QSize(155, MAX_WIDGET_HEIGHT));
    ui_Bib_ListWidget_Book->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    bookVBoxLayout->addWidget(ui_Bib_ListWidget_Book);

    QVBoxLayout *chapterVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(chapterVBoxLayout);

    QLabel *chapterLabel = new QLabel;
    m_labels[1] = chapterLabel;
    chapterVBoxLayout->addWidget(chapterLabel);

    ui_Bib_ListWidget_Chapter = new QListWidget;
    ui_Bib_ListWidget_Chapter->setMaximumSize(QSize(60, MAX_WIDGET_HEIGHT));
    ui_Bib_ListWidget_Chapter->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    chapterVBoxLayout->addWidget(ui_Bib_ListWidget_Chapter);

    QVBoxLayout *versesVBoxLayout = new QVBoxLayout;
    tabBibleHorLayout->addLayout(versesVBoxLayout);

    QLabel *versesLabel = new QLabel;
    m_labels[2] = versesLabel;
    versesVBoxLayout->addWidget(versesLabel);

    ui_Bib_ComboBox_VerseFrom = new QComboBox;
    ui_Bib_ComboBox_VerseFrom->setMaximumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseFrom->setMinimumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseFrom->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Bib_ComboBox_VerseFrom->setMaxVisibleItems(30);
    ui_Bib_ComboBox_VerseFrom->setStyleSheet(COMBOBOX_STYLE);
    versesVBoxLayout->addWidget(ui_Bib_ComboBox_VerseFrom);

    ui_Bib_ComboBox_VerseTo = new QComboBox;
    ui_Bib_ComboBox_VerseTo->setMaximumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseTo->setMinimumSize(QSize(55, 20));
    ui_Bib_ComboBox_VerseTo->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Bib_ComboBox_VerseTo->setMaxVisibleItems(30);
    ui_Bib_ComboBox_VerseTo->setStyleSheet(COMBOBOX_STYLE);
    versesVBoxLayout->addWidget(ui_Bib_ComboBox_VerseTo);
    versesVBoxLayout->addStretch();

    ui_Bib_Button_Random = new QPushButton;
    ui_Bib_Button_Random->setMaximumSize(QSize(55, 30));
    versesVBoxLayout->addWidget(ui_Bib_Button_Random);

    QHBoxLayout *prevNextHorLayout = new QHBoxLayout;
    versesVBoxLayout->addLayout(prevNextHorLayout);

    ui_Bib_Button_Prev = new QPushButton("<<");
    ui_Bib_Button_Prev->setMaximumWidth(25);
    prevNextHorLayout->addWidget(ui_Bib_Button_Prev);

    ui_Bib_Button_Next = new QPushButton(">>");
    ui_Bib_Button_Next->setMaximumWidth(25);
    prevNextHorLayout->addWidget(ui_Bib_Button_Next);

    ui_Bib_VerLayout_Modules = new QVBoxLayout;
    tabBibleHorLayout->addLayout(ui_Bib_VerLayout_Modules);

    ui_Bib_TabWidget_Modules = new QTabWidget;
    ui_Bib_TabWidget_Modules->setMovable(true);
    ui_Bib_TabWidget_Modules->setTabsClosable(true);
    ui_Bib_TabWidget_Modules->setTabPosition(QTabWidget::TabPosition(m_tabPos));
    ui_Bib_VerLayout_Modules->addWidget(ui_Bib_TabWidget_Modules);

    ui_Bib_TabBar_Modules = ui_Bib_TabWidget_Modules->tabBar();

    setLabelTexts(0);
    setButtonTexts(0);
}

void MainWindow::populateLanguageMap(const QString &lang)
{
    m_languages.insert(0, "EN");
    m_languages.insert(1, "ES");
    m_languages.insert(2, "PL");
    actionCheckLanguage(m_languages.key(lang), true);
}

MainWindow::TabBookChapterVerses MainWindow::loadSettings()
{
    TabBookChapterVerses tbcvv;
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    m_maxRecentPassages = settings.value(SET_MAX_RECENT_PASSAGES).toInt();
    if (m_maxRecentPassages < 2) {
        m_maxRecentPassages = 20;
    }
    settings.beginGroup(GROUP_MAIN_WINDOW);
    const QByteArray geometry = settings.value(SET_GEOMETRY, QByteArray()).toByteArray();
    if (!geometry.isNull() && !geometry.isEmpty()) {
        QWidget::restoreGeometry(geometry);
    } else {
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
        tbcvv = { setIndex,
                  setPassage[0].toInt(),
                  setPassage[1].toInt(),
                  setPassage[2].toInt(),
                  setPassage[3].toInt()};
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
    settings.beginGroup(GROUP_FONT);
    const QString setFontFamily = settings.value(SET_FONT_FAMILY).toString();
    const int setFontSize = settings.value(SET_FONT_SIZE).toInt();
    if (!setFontFamily.isEmpty() && setFontSize != 0) {
        m_currentFont = QFont(setFontFamily, setFontSize);
    } else {
        m_currentFont = QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE);
    }
    settings.endGroup();
    settings.beginGroup(GROUP_APPEARANCE);
    m_useBackground = settings.value(SET_USE_BACKGROUND).toBool();
    m_highlightColor = settings.value(SET_HIGHLIGHT_COLOR).value<QColor>();
    if (!m_highlightColor.isValid()) {
        m_highlightColor = QColor(0, 255, 0, 50);
    }
    m_tabPos = settings.value(SET_TAB_POSITION).toInt();
    settings.endGroup();
    return tbcvv;
}

void MainWindow::generateBibModuleTabs()
{
    const QString moduleDirName = m_executionPath + "/App/modules";
    QString noModule = tr("No module found");
    if (!QDir(moduleDirName).exists()) {
        QDir().mkdir(moduleDirName);
        ui_Bib_TabWidget_Modules->addTab(new QWidget(), noModule);
        ui_Bib_ListWidget_Book->setDisabled(true);
        m_modulesFound = false;
    } else {
        QStringList localModules = getModulePaths(moduleDirName);
        for (const QString &path : localModules) {
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
    ui_Bib_ListWidget_Book->clear();
    ui_Bib_ListWidget_Book->addItems(m_bookNames);
}

void MainWindow::connectBibleTabSignals()
{
    QObject::connect(ui_TabWidget_Main, SIGNAL(currentChanged(int)),
                     this, SLOT(on_CurrentChanged_TabWidget_Main(int)));
    QObject::connect(ui_Bib_ListWidget_Book, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Bib_CurrentRowChanged_ListWidget_Book(int)));
    QObject::connect(ui_Bib_ListWidget_Chapter, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Bib_CurrentRowChanged_ListWidget_Chapter(int)));
    QObject::connect(ui_Bib_ComboBox_VerseFrom, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Bib_CurrentIndexChanged_ComboBox_VerseFrom(int)));
    QObject::connect(ui_Bib_ComboBox_VerseTo, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Bib_CurrentIndexChanged_ComboBox_VerseTo(int)));
    QObject::connect(ui_Bib_Button_Random, SIGNAL(clicked()),
                     this, SLOT(on_Bib_Clicked_PushButton_Random()));
    QObject::connect(ui_Bib_Button_Prev, SIGNAL(clicked()),
                     this, SLOT(on_Bib_Clicked_PushButton_Prev()));
    QObject::connect(ui_Bib_Button_Next, SIGNAL(clicked()),
                     this, SLOT(on_Bib_Clicked_PushButton_Next()));
    QObject::connect(ui_Bib_TabBar_Modules, SIGNAL(tabMoved(int, int)),
                     this, SLOT(on_Bib_TabMoved_Modules(int, int)));
    QObject::connect(ui_Bib_TabWidget_Modules, SIGNAL(currentChanged(int)),
                     this, SLOT(on_Bib_CurrentChanged_TabWidget_Modules(int)));
    QObject::connect(ui_Bib_TabWidget_Modules, SIGNAL(tabCloseRequested(int)),
                     this, SLOT(on_Bib_TabCloseRequested_TabWidget_Modules(int)));
}

void MainWindow::connectDetailsTabSignals()
{
    QObject::connect(ui_Det_TextBrowser_Description, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Det_LineEdit_Abbreviation, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowLineContextMenu(QPoint)));
    QObject::connect(ui_Det_TextBrowser_Comments, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Det_LineEdit_Version, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowLineContextMenu(QPoint)));
    QObject::connect(ui_Det_LineEdit_VersionDate, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowLineContextMenu(QPoint)));
    QObject::connect(ui_Det_LineEdit_PublishDate, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowLineContextMenu(QPoint)));
}

void MainWindow::connectSearchTabSignals()
{
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(returnPressed()),
                     this, SLOT(on_Sea_LineEdit_ReturnPressed_Search()));
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(textChanged(QString)),
                     this, SLOT(on_Sea_LineEdit_TextChanged_Search(QString)));
    QObject::connect(ui_Sea_Button_Search, SIGNAL(clicked()),
                     this, SLOT(on_Sea_Clicked_PushButton_Search()));
    QObject::connect(ui_Sea_ComboBox_SearchFrom, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_SearchFrom(int)));
    QObject::connect(ui_Sea_ComboBox_SearchTo, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_SearchTo(int)));
    QObject::connect(ui_Sea_ComboBox_Section, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_Section(int)));
    QObject::connect(ui_Sea_Button_Prev, SIGNAL(clicked()),
                     this, SLOT(on_Sea_Clicked_PushButton_Prev()));
    QObject::connect(ui_Sea_Button_Next, SIGNAL(clicked()),
                     this, SLOT(on_Sea_Clicked_PushButton_Next()));
    QObject::connect(ui_Sea_Button_RandomVerse, SIGNAL(clicked()),
                     this, SLOT(on_Sea_Clicked_PushButton_RandomVerse()));
    QObject::connect(ui_Sea_LineEdit_Search, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
    QObject::connect(ui_Sea_TextBrowser_Results, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Sea_TextBrowser_Results, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Sea_AnchorClicked_TextBrowser_Results(QUrl)));
    QObject::connect(ui_Sea_ComboBox_Translation, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(on_Sea_ComboBox_CurrentIndexChanged_Translation(int)));
    QObject::connect(ui_Sea_RadioButton_Strong, SIGNAL(toggled(bool)),
                     this, SLOT(on_Sea_Toggled_RadioButton_Strong(bool)));
    QObject::connect(ui_Sea_TextBrowser_RandomVerse, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
}

void MainWindow::connectCompareTabSignals()
{
    QObject::connect(ui_Com_ListWidget_Book, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Com_CurrentRowChanged_ListWidget_Book(int)));
    QObject::connect(ui_Com_ListWidget_Chapter, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Com_CurrentRowChanged_ListWidget_Chapter(int)));
    QObject::connect(ui_Com_ListWidget_Verse, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Com_CurrentRowChanged_ListWidget_Verse(int)));
    QObject::connect(ui_Com_TextBrowser_Compare, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Com_TextBrowser_Compare, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Com_AnchorClicked_TextBrowser_Compare(QUrl)));
}

void MainWindow::connectFavoritesTabSignals()
{
    QObject::connect(ui_Fav_ListWidget_Passages, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Fav_CurrentRowChanged_ListWidget_Passages(int)));
    QObject::connect(ui_Fav_TextBrowser_Passage, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Fav_TextBrowser_Passage, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Fav_AnchorClicked_TextBrowser_Passage(QUrl)));
    QObject::connect(ui_Fav_Button_Delete, SIGNAL(clicked()),
                     this, SLOT(on_Fav_Clicked_PushButton_Delete()));
    QObject::connect(ui_Fav_Button_Save, SIGNAL(clicked()),
                     this, SLOT(on_Fav_Clicked_PushButton_Save()));
    QObject::connect(ui_Fav_TextEdit_Comment, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
}

void MainWindow::connectDictionaryTabSignals()
{
    QObject::connect(ui_Dic_ListWidget_AllEntries, SIGNAL(currentTextChanged(QString)),
                     this, SLOT(on_Dic_TextChanged_ListWidget_AllEntries(QString)));
    QObject::connect(ui_Dic_TextBrowser_Definition, SIGNAL(highlighted(QUrl)),
                     this, SLOT(on_Dic_Highlighted_TextBrowser_Definition(QUrl)));
    QObject::connect(ui_Dic_TextBrowser_Definition, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowBasicContextMenu(QPoint)));
    QObject::connect(ui_Dic_TextBrowser_Definition, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Dic_AnchorClicked_TextBrowser_Definition(QUrl)));
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

void MainWindow::swapTabHistory(int from, int to)
{
    QList<int> indices;
    for (int i = 0; i < m_history.count(); ++i) {
        if (m_history[i].tab == from) {
            indices << i;
        } else if (m_history[i].tab == to) {
            m_history[i].tab = from;
        }
    }
    for (int i = 0; i < indices.count(); ++i) {
        m_history[indices[i]].tab = to;
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
                    "<center><h2>" % tr("Unavailable in this module.") % "</center></h2>");
    }
}

void MainWindow::populateVersesComboBoxes(int verseFrom, int verseTo)
{
    int dbIdx = ui_Bib_TabWidget_Modules->currentIndex();
    int book = ui_Bib_ListWidget_Book->currentRow() + 1;
    int chapter = ui_Bib_ListWidget_Chapter->currentRow() + 1;
    QString queryString = "SELECT Verse FROM Bible WHERE Book = " % QString::number(book) %
                          " AND Chapter = " % QString::number(chapter);
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

void MainWindow::removeTabFromHistory(int idx)
{
    for (int i = 0; i < m_history.size(); ++i) {
        if (m_history[i].tab == idx) {
            m_history.removeAt(i--);
            m_psgIdx--;
        } else if (m_history[i].tab > idx) {
            m_history[i].tab--;
        }
    }
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
    QString dirName = m_executionPath + "/App/user";
    QString filename = dirName + "/fav.bblv";
    if (!QDir(dirName).exists()) {
        QDir().mkdir(dirName);
        createFavDatabase(m_dbUsr, filename);
    } else if (!QDir().exists(filename)) {
        createFavDatabase(m_dbUsr, filename);
    } else {
        m_dbUsr = QSqlDatabase::addDatabase("QSQLITE");
        m_dbUsr.setDatabaseName(filename);
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

void MainWindow::loadPassage()
{
    TabBookChapterVerses tbcvv = getTabBookChapterVerses();
    int idx = tbcvv.tab;
    QObject::disconnect(m_chapterBrowsers[idx], SIGNAL(cursorPositionChanged()),
                        this, SLOT(on_Bib_CursorPositionChanged_chapterBrowser()));
    QString bookStr = QString::number(tbcvv.book);
    QString chapterStr = QString::number(tbcvv.chapter);
    QString queryString = QStringLiteral("SELECT Verse, Scripture FROM Bible "
                                         "WHERE Book = %1 "
                                         "AND Chapter = %2 "
                                         "AND Verse >= %3 "
                                         "AND Verse <= %4")
            .arg(bookStr, chapterStr, QString::number(tbcvv.verseFrom), QString::number(tbcvv.verseTo));
    QSqlQuery query(m_modules[idx].database);
    if (!query.exec(queryString)) {
        return;
    }
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
        QString xRefQueryString = QStringLiteral("SELECT XRefs FROM CrossReferences WHERE BOOK = %1 "
                                                 "AND Chapter = %2 "
                                                 "AND Verse = %3").arg(bookStr, chapterStr, verse);
        m_verseMaps[idx][verseCnt] = verse.toInt();
        if (verseCnt > 0) {
            cursor.insertBlock();
        }
        xRefQuery.exec(xRefQueryString);
        if (xRefQuery.next()) {           
            verse = QStringLiteral("<a href='x:%1:%2'>[%3]</a>")
                    .arg(verse, xRefQuery.record().value(0).toString(), verse);
            formatScripture(scripture, idx, m_modules[idx].hasStrong, noteRgx, strongRgx);
            cursor.insertHtml("<b>" % verse % "</b> " % scripture);
        } else {
            formatScripture(scripture, idx, m_modules[idx].hasStrong, noteRgx, strongRgx);
            cursor.insertHtml("<b>[" % verse % "]</b> " % scripture);
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
    QObject::connect(m_chapterBrowsers[idx], SIGNAL(cursorPositionChanged()),
                     this, SLOT(on_Bib_CursorPositionChanged_chapterBrowser()));
    m_loadedFlags[idx] = true;
}

MainWindow::TabBookChapterVerses MainWindow::getTabBookChapterVerses()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    int book = ui_Bib_ListWidget_Book->currentRow() + 1;
    int chapter = ui_Bib_ListWidget_Chapter->currentRow() + 1;
    int verseFrom = ui_Bib_ComboBox_VerseFrom->currentIndex() + 1;
    int verseTo = ui_Bib_ComboBox_VerseTo->currentIndex() + 1;
    return TabBookChapterVerses(idx, book, chapter, verseFrom, verseTo);
}

void MainWindow::updateHistory(const TabBookChapterVerses &tbcvv)
{
    if (m_history.count() == m_maxRecentPassages) {
        m_history.removeFirst();
    } else if (m_psgIdx < m_history.count() - 1) {
        m_history.remove(m_psgIdx + 1, m_history.count() - 1 - m_psgIdx);
    }
    m_history << tbcvv;
    m_psgIdx = m_history.count() - 1;
    ui_Act_Back->setEnabled(m_history.count() > 1);
    ui_Act_Forward->setDisabled(true);
}


void MainWindow::translateTexts()
{
    setMenuTexts();
    setMainTabNames();
    setMainTabToolTips();

    setLabelTexts(-1);
    setButtonTexts(-1);
    setCheckBoxTexts(-1);

    ui_Bib_ListWidget_Book->blockSignals(true);
    int idx = ui_Bib_ListWidget_Book->currentRow();
    populateBookNames();
    ui_Bib_ListWidget_Book->setCurrentRow(idx);
    ui_Bib_ListWidget_Book->blockSignals(false);

    if (ui_TabWidget_Main->widget(2)->children().count() > 0) {
        ui_Sea_ComboBox_SearchFrom->blockSignals(true);
        idx = ui_Sea_ComboBox_SearchFrom->currentIndex();
        ui_Sea_ComboBox_SearchFrom->clear();
        ui_Sea_ComboBox_SearchFrom->addItems(m_bookNames);
        ui_Sea_ComboBox_SearchFrom->setCurrentIndex(idx);
        ui_Sea_ComboBox_SearchFrom->blockSignals(false);
        ui_Sea_ComboBox_SearchTo->blockSignals(true);
        idx = ui_Sea_ComboBox_SearchTo->currentIndex();
        ui_Sea_ComboBox_SearchTo->clear();
        ui_Sea_ComboBox_SearchTo->addItems(m_bookNames);
        ui_Sea_ComboBox_SearchTo->setCurrentIndex(idx);
        ui_Sea_ComboBox_SearchTo->blockSignals(false);
    }

    if (ui_TabWidget_Main->widget(3)->children().count() > 0) {
        ui_Com_ListWidget_Book->blockSignals(true);
        idx = ui_Com_ListWidget_Book->currentRow();
        ui_Com_ListWidget_Book->clear();
        ui_Com_ListWidget_Book->addItems(m_bookNames);
        ui_Com_ListWidget_Book->setCurrentRow(idx);
        ui_Com_ListWidget_Book->blockSignals(false);
    }
}

void MainWindow::on_Bib_SelectionChanged_ChapterBrowser()
{
    int idx = ui_Bib_TabBar_Modules->currentIndex();
    QTextCursor cursor = m_chapterBrowsers[idx]->textCursor();
    bool isEmpty = cursor.selectionStart() == cursor.selectionEnd();
    ui_Act_Copy->setDisabled(isEmpty);
    ui_Act_CopyWithRef->setDisabled(isEmpty);
}

void MainWindow::on_Sea_Clicked_PushButton_Search()
{
    ui_Sea_RadioButton_Strong->isChecked() ? performSearchByStrong() : performSearch();
}

void MainWindow::on_Sea_ComboBox_CurrentIndexChanged_SearchFrom(int index)
{
    if (index > ui_Sea_ComboBox_SearchTo->currentIndex()) {
        ui_Sea_ComboBox_SearchTo->blockSignals(true);
        ui_Sea_ComboBox_SearchTo->setCurrentIndex(index);
        ui_Sea_ComboBox_SearchTo->blockSignals(false);
    }
}

void MainWindow::on_Sea_ComboBox_CurrentIndexChanged_SearchTo(int index)
{
    if (index < ui_Sea_ComboBox_SearchFrom->currentIndex()) {
        ui_Sea_ComboBox_SearchFrom->blockSignals(true);
        ui_Sea_ComboBox_SearchFrom->setCurrentIndex(index);
        ui_Sea_ComboBox_SearchFrom->blockSignals(false);
    }
}

void MainWindow::on_Sea_Toggled_RadioButton_Strong(bool checked)
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

void MainWindow::displaySearchResults(int startIdx, int endIdx)
{
    ui_Sea_TextBrowser_Results->clear();
    int idx = ui_Sea_ComboBox_Translation->currentIndex();
    int i = startIdx;
    QString resultString;
    QRegExp regex("<RF>.*<Rf>");
    regex.setMinimal(true);
    while (i < m_resVerses.count() && i < endIdx) {
        resultString += formatResult(m_resVerses[i], regex, m_modules[idx].hasStrong) %
                m_resRefs[i] % "</a></b><br><br>";
        i++;
    }
    ui_Sea_TextBrowser_Results->setHtml(resultString);

    QRegExp refRgx("—([1-3](\\w+\\s){1,3}|(\\w+\\s){1,3})\\d{1,3}:\\d{1,3}");
    QList<QTextEdit::ExtraSelection> refExtraSelections;
    while (ui_Sea_TextBrowser_Results->find(refRgx)) {
        QTextEdit::ExtraSelection extra;
        extra.cursor = ui_Sea_TextBrowser_Results->textCursor();
        refExtraSelections << extra;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    while (ui_Sea_TextBrowser_Results->find(m_dispRgx, QTextDocument::FindBackward)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = ui_Sea_TextBrowser_Results->textCursor();
        if (!isContainedInSelections(extra, refExtraSelections)) {
            extraSelections << extra;
        }
    }
    ui_Sea_TextBrowser_Results->setExtraSelections(extraSelections);
    ui_Sea_TextBrowser_Results->moveCursor(QTextCursor::Start);

    QString statusMessage;
    if (m_resVerses.count() == 1) {
        statusMessage = tr("Verse 1 (1 in total); Elapsed time: ") % m_elapsedTime;
    } else {
        statusMessage = tr("Verses ") % QString::number(startIdx + 1) % "-" % QString::number(i) %
                " ("  % QString::number(m_resVerses.count()) + tr(" in total);  Elapsed time: ") %
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

    QVBoxLayout *bookVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(bookVBoxLayout);

    QLabel *bookLabel = new QLabel;
    m_labels[15] = bookLabel;
    bookVBoxLayout->addWidget(bookLabel);

    ui_Com_ListWidget_Book = new QListWidget;
    ui_Com_ListWidget_Book->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Book->setMaximumWidth(155);
    ui_Com_ListWidget_Book->addItems(m_bookNames);
    bookVBoxLayout->addWidget(ui_Com_ListWidget_Book);

    QVBoxLayout *chapterVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(chapterVBoxLayout);

    QLabel *chapterLabel = new QLabel;
    m_labels[16] = chapterLabel;
    chapterVBoxLayout->addWidget(chapterLabel);

    ui_Com_ListWidget_Chapter = new QListWidget;
    ui_Com_ListWidget_Chapter->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Chapter->setMaximumWidth(60);
    chapterVBoxLayout->addWidget(ui_Com_ListWidget_Chapter);

    QVBoxLayout *verseVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(verseVBoxLayout);

    QLabel *verseLabel = new QLabel;
    m_labels[17] = verseLabel;
    verseVBoxLayout->addWidget(verseLabel);

    ui_Com_ListWidget_Verse = new QListWidget;
    ui_Com_ListWidget_Verse->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Com_ListWidget_Verse->setMaximumWidth(60);
    verseVBoxLayout->addWidget(ui_Com_ListWidget_Verse);

    QVBoxLayout *compareVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(compareVBoxLayout);

    ui_Com_TextBrowser_Compare = new QTextBrowser;
    ui_Com_TextBrowser_Compare->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_Com_TextBrowser_Compare->setFont(m_currentFont);
    ui_Com_TextBrowser_Compare->setOpenLinks(false);
    compareVBoxLayout->addWidget(ui_Com_TextBrowser_Compare);
    setBrowserBackground(*ui_Com_TextBrowser_Compare);

    QHBoxLayout *prevNextHLayout = new QHBoxLayout;
    compareVBoxLayout->addLayout(prevNextHLayout);

    ui_Com_Button_Prev = new QPushButton("<<");
    ui_Com_Button_Prev->setToolTip(tr("Go to the previous verse."));
    prevNextHLayout->addWidget(ui_Com_Button_Prev);

    ui_Com_Button_Next = new QPushButton(">>");
    ui_Com_Button_Next->setToolTip(tr("Go to the next verse."));
    prevNextHLayout->addWidget(ui_Com_Button_Next);
    prevNextHLayout->addStretch();

    m_dbCntr = QSqlDatabase::addDatabase("QSQLITE", "Counters");
    m_dbCntr.setDatabaseName(m_executionPath + "/App/data/counters.bblv");
    if (!m_dbCntr.open()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
    }

    connectCompareTabSignals();

    setLabelTexts(3);

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


void MainWindow::on_Com_CurrentRowChanged_ListWidget_Book(int currentRow)
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
    if (!m_firstLoadCompare) {
        ui_Com_ListWidget_Chapter->setCurrentRow(0);
    }
}

void MainWindow::on_Com_CurrentRowChanged_ListWidget_Chapter(int currentRow)
{
    int book = ui_Com_ListWidget_Book->currentRow() + 1;
    QString queryString = "SELECT VerseCount, ChapterNumber FROM Counters WHERE Book = " +
                          QString::number(book) + " AND Chapter = " + QString::number(currentRow + 1);
    QSqlQuery query(m_dbCntr);
    int verseCount = 0;
    if (query.exec(queryString)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            verseCount = record.value(0).toInt();
            ui_Label_Status->setText(tr("Chapter: ") % record.value(1).toString() % "/1189");
        }
    }
    QStringList verseNumbers;
    for (int i = 1; i <= verseCount; ++i) {
        verseNumbers << QString::number(i);
    }
    ui_Com_ListWidget_Verse->blockSignals(true);
    ui_Com_ListWidget_Verse->clear();
    ui_Com_ListWidget_Verse->addItems(verseNumbers);
    ui_Com_ListWidget_Verse->blockSignals(false);
    if (!m_firstLoadCompare) {
        ui_Com_ListWidget_Verse->setCurrentRow(0);
    }
}

void MainWindow::on_Com_CurrentRowChanged_ListWidget_Verse(int currentRow)
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
        if (query.exec(queryString)) {
            if (query.next()) {
                QString scripture = query.record().value(0).toString().trimmed();
                text += QString("<tr><td><b><a href='t:%1'>%2:</a></b> %3</td></tr>")
                        .arg(QString::number(i), m_modules[i].name, formatVerse(scripture, m_modules[i].hasStrong));
            }
        }
    }
    text += QStringLiteral("</table>");
    ui_Com_TextBrowser_Compare->setHtml(text);
    m_comVerse.clear();
    m_comVerse << QString::number(ui_Com_ListWidget_Book->currentRow() + 1)
               << QString::number(ui_Com_ListWidget_Chapter->currentRow() + 1)
               << QString::number(ui_Com_ListWidget_Verse->currentRow() + 1);
    m_firstLoadCompare = false;
}

QString MainWindow::formatVerse(QString text, bool hasStrong)
{
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>")).
            replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(QStringLiteral("<CM>"));
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
                text.replace(original, " <a href='s:" % modified % "'>" % modified % "</a>");
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

    QLabel *descriptionLabel = new QLabel;
    m_labels[3] = descriptionLabel;
    descriptionVBoxLayout->addWidget(descriptionLabel);

    ui_Det_TextBrowser_Description = new QTextBrowser;
    ui_Det_TextBrowser_Description->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_TextBrowser_Description->setFont(m_currentFont);
    ui_Det_TextBrowser_Description->setOpenExternalLinks(false);
    descriptionVBoxLayout->addWidget(ui_Det_TextBrowser_Description);
    setBrowserBackground(*ui_Det_TextBrowser_Description);

    QVBoxLayout *abbreviationVBoxLayout = new QVBoxLayout;
    descriptionAbbreviationVBoxLayout->addLayout(abbreviationVBoxLayout);

    QLabel *abbreviationLabel = new QLabel;
    m_labels[4] = abbreviationLabel;
    abbreviationVBoxLayout->addWidget(abbreviationLabel);

    ui_Det_LineEdit_Abbreviation = new QLineEdit;
    ui_Det_LineEdit_Abbreviation->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_LineEdit_Abbreviation->setReadOnly(true);
    abbreviationVBoxLayout->addWidget(ui_Det_LineEdit_Abbreviation);

    QVBoxLayout *commentsVBoxLayout = new QVBoxLayout;
    detailsHBoxLayout->addLayout(commentsVBoxLayout);

    QLabel *commentsLabel = new QLabel;
    m_labels[5] = commentsLabel;
    commentsVBoxLayout->addWidget(commentsLabel);

    ui_Det_TextBrowser_Comments = new QTextBrowser;
    ui_Det_TextBrowser_Comments->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_TextBrowser_Comments->setFont(m_currentFont);
    ui_Det_TextBrowser_Comments->setOpenExternalLinks(false);
    ui_Det_TextBrowser_Comments->setOpenLinks(false);

    commentsVBoxLayout->addWidget(ui_Det_TextBrowser_Comments);
    setBrowserBackground(*ui_Det_TextBrowser_Comments);

    QHBoxLayout *versionPublishDateHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(versionPublishDateHBoxLayout);

    QVBoxLayout *versionVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(versionVBoxLayout);

    QLabel *versionLabel = new QLabel;
    m_labels[6] = versionLabel;
    versionVBoxLayout->addWidget(versionLabel);

    ui_Det_LineEdit_Version = new QLineEdit;
    ui_Det_LineEdit_Version->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_LineEdit_Version->setReadOnly(true);
    versionVBoxLayout->addWidget(ui_Det_LineEdit_Version);

    QVBoxLayout *versionDateVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(versionDateVBoxLayout);

    QLabel *versionDateLabel = new QLabel;
    m_labels[7] = versionDateLabel;
    versionDateVBoxLayout->addWidget(versionDateLabel);

    ui_Det_LineEdit_VersionDate = new QLineEdit;
    ui_Det_LineEdit_VersionDate->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Det_LineEdit_VersionDate->setReadOnly(true);
    versionDateVBoxLayout->addWidget(ui_Det_LineEdit_VersionDate);

    QVBoxLayout *publishDateVBoxLayout = new QVBoxLayout;
    versionPublishDateHBoxLayout->addLayout(publishDateVBoxLayout);

    QLabel *publishDateLabel = new QLabel;
    m_labels[8] = publishDateLabel;
    publishDateVBoxLayout->addWidget(publishDateLabel);

    ui_Det_LineEdit_PublishDate = new QLineEdit;
    ui_Det_LineEdit_PublishDate->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_Det_LineEdit_PublishDate->setReadOnly(true);
    publishDateVBoxLayout->addWidget(ui_Det_LineEdit_PublishDate);

    QHBoxLayout *rightOldNewStrongHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(rightOldNewStrongHBoxLayout);

    ui_Det_CheckBox_RightToLeft = new QCheckBox;
    ui_Det_CheckBox_RightToLeft->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_RightToLeft->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_RightToLeft->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_RightToLeft);

    ui_Det_CheckBox_OldTestament = new QCheckBox;
    ui_Det_CheckBox_OldTestament->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_OldTestament->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_OldTestament->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_OldTestament);

    ui_Det_CheckBox_NewTestament = new QCheckBox;
    ui_Det_CheckBox_NewTestament->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_NewTestament->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_NewTestament->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_NewTestament);

    ui_Det_CheckBox_StrongsNumbers = new QCheckBox;
    ui_Det_CheckBox_StrongsNumbers->setLayoutDirection(Qt::RightToLeft);
    ui_Det_CheckBox_StrongsNumbers->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_Det_CheckBox_StrongsNumbers->setFocusPolicy(Qt::NoFocus);
    rightOldNewStrongHBoxLayout->addWidget(ui_Det_CheckBox_StrongsNumbers);

    detailsHBoxLayout->setStretchFactor(descriptionAbbreviationVBoxLayout, 1);
    detailsHBoxLayout->setStretchFactor(commentsVBoxLayout, 2);

    connectDetailsTabSignals();
    fillDetailsTab();

    setLabelTexts(1);
    setCheckBoxTexts(1);
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

    QVBoxLayout *dictionariesVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(dictionariesVerLayout);

    QLabel *dictionariesLabel = new QLabel;
    m_labels[20] = dictionariesLabel;
    dictionariesVerLayout->addWidget(dictionariesLabel);

    QListWidget *dictionariesListWidget = new QListWidget;
    dictionariesListWidget->setMaximumWidth(200);
    dictionariesListWidget->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    dictionariesVerLayout->addWidget(dictionariesListWidget);
    QObject::connect(dictionariesListWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(on_Dic_CurrentRowChanged_ListWidget_Dictionaries(int)));

    QVBoxLayout *numberEntriesVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(numberEntriesVerLayout);

    QLabel *numberLabel = new QLabel;
    m_labels[21] = numberLabel;
    numberEntriesVerLayout->addWidget(numberLabel);

    QLineEdit *numberLineEdit = new QLineEdit;
    numberLineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(numberLineEdit, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(actionShowEditContextMenu(QPoint)));
    numberLineEdit->setMaximumWidth(180);
    numberEntriesVerLayout->addWidget(numberLineEdit);
    numberLineEdit->setFocus();
    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
    QValidator *validator = new QRegExpValidator(regex, this);
    numberLineEdit->setValidator(validator);
    QObject::connect(numberLineEdit, SIGNAL(textEdited(QString)),
                     this, SLOT(on_Dic_TextEdited_LineEdit_Number(QString)));

    QLabel *allEntriesLabel = new QLabel;
    m_labels[22] = allEntriesLabel;
    numberEntriesVerLayout->addWidget(allEntriesLabel);

    ui_Dic_ListWidget_AllEntries = new QListWidget;
    ui_Dic_ListWidget_AllEntries->setMaximumWidth(180);
    numberEntriesVerLayout->addWidget(ui_Dic_ListWidget_AllEntries);

    QVBoxLayout *definitionVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(definitionVerLayout);

    QLabel *definitionLabel = new QLabel;
    m_labels[23] = definitionLabel;
    definitionVerLayout->addWidget(definitionLabel);

    ui_Dic_TextBrowser_Definition = new QTextBrowser;
    ui_Dic_TextBrowser_Definition->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Dic_TextBrowser_Definition->setFont(m_currentFont);
    ui_Dic_TextBrowser_Definition->setOpenLinks(false);
    definitionVerLayout->addWidget(ui_Dic_TextBrowser_Definition);
    setBrowserBackground(*ui_Dic_TextBrowser_Definition);

    setLabelTexts(idx);

    connectDictionaryTabSignals();

    QStringList dictNameList;
    for (const QString &path : m_dictPathList) {
        dictNameList << QFileInfo(path).fileName();
    }
    dictionariesListWidget->addItems(dictNameList);
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

    QLabel *passagesLabel = new QLabel;
    m_labels[18] = passagesLabel;
    passagesVBoxLayout->addWidget(passagesLabel);

    ui_Fav_ListWidget_Passages = new QListWidget;
    ui_Fav_ListWidget_Passages->setFont(QFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE));
    ui_Fav_ListWidget_Passages->setMaximumWidth(220);
    passagesVBoxLayout->addWidget(ui_Fav_ListWidget_Passages);

    QVBoxLayout *textCommentVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(textCommentVBoxLayout);

    ui_Fav_TextBrowser_Passage = new QTextBrowser;
    ui_Fav_TextBrowser_Passage->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_Fav_TextBrowser_Passage->setFont(m_currentFont);
    ui_Fav_TextBrowser_Passage->setOpenLinks(false);
    textCommentVBoxLayout->addWidget(ui_Fav_TextBrowser_Passage);
    setBrowserBackground(*ui_Fav_TextBrowser_Passage);

    QHBoxLayout *commentDeleteSaveHBoxLayout = new QHBoxLayout;
    textCommentVBoxLayout->addLayout(commentDeleteSaveHBoxLayout);

    QLabel *commentLabel = new QLabel;
    m_labels[19] = commentLabel;
    commentDeleteSaveHBoxLayout->addWidget(commentLabel);

    commentDeleteSaveHBoxLayout->addStretch();

    ui_Fav_Button_Delete = new QPushButton;
    commentDeleteSaveHBoxLayout->addWidget(ui_Fav_Button_Delete);

    ui_Fav_Button_Save = new QPushButton;
    commentDeleteSaveHBoxLayout->addWidget(ui_Fav_Button_Save);

    ui_Fav_TextEdit_Comment = new QTextEdit;
    ui_Fav_TextEdit_Comment->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Fav_TextEdit_Comment->setFont(m_currentFont);
    textCommentVBoxLayout->addWidget(ui_Fav_TextEdit_Comment);
    setBrowserBackground(*ui_Fav_TextEdit_Comment);

    setLabelTexts(idx);
    setButtonTexts(idx);

    connectFavoritesTabSignals();

    loadFavorites();
}

void MainWindow::generateSearchTabControls(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabSearchWidget = ui_TabWidget_Main->widget(idx);

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout(tabSearchWidget);
    mainVBoxLayout->setSpacing(5);
    mainVBoxLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *enterSearchSectionHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(enterSearchSectionHBoxLayout);

    QVBoxLayout *enterSearchVBoxLayout = new QVBoxLayout;
    enterSearchSectionHBoxLayout->addLayout(enterSearchVBoxLayout);

    QLabel *enterLabel = new QLabel;
    m_labels[9] = enterLabel;
    enterSearchVBoxLayout->addWidget(enterLabel);

    QHBoxLayout *enterHBoxLayout = new QHBoxLayout;
    enterSearchVBoxLayout->addLayout(enterHBoxLayout);
    ui_Sea_LineEdit_Search = new QLineEdit;
    ui_Sea_LineEdit_Search->setContextMenuPolicy(Qt::CustomContextMenu);

    enterHBoxLayout->addWidget(ui_Sea_LineEdit_Search);
    ui_Sea_LineEdit_Search->setFocus();
    ui_Sea_Button_Search = new QPushButton;
    enterHBoxLayout->addWidget(ui_Sea_Button_Search);
    ui_Sea_Button_Search->setDisabled(true);

    QVBoxLayout *rangeVerLayout = new QVBoxLayout;
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

    QVBoxLayout *sectionVBoxLayout = new QVBoxLayout;
    enterSearchSectionHBoxLayout->addLayout(sectionVBoxLayout);

    QLabel *sectionLabel = new QLabel;
    m_labels[10] = sectionLabel;
    sectionVBoxLayout->addWidget(sectionLabel);

    ui_Sea_ComboBox_Section = new QComboBox;
    ui_Sea_ComboBox_Section->setMaxVisibleItems(12);
    ui_Sea_ComboBox_Section->setStyleSheet(COMBOBOX_STYLE);
    ui_Sea_ComboBox_Section->setMinimumWidth(170);
    sectionVBoxLayout->addWidget(ui_Sea_ComboBox_Section);

    populateSectionNames();

    QHBoxLayout *resultsOptionsHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(resultsOptionsHBoxLayout);

    QVBoxLayout *resultsVBoxLayout = new QVBoxLayout;
    resultsOptionsHBoxLayout->addLayout(resultsVBoxLayout);

    QLabel *resultsLabel = new QLabel;
    m_labels[11] = resultsLabel;
    resultsVBoxLayout->addWidget(resultsLabel);

    ui_Sea_TextBrowser_Results = new QTextBrowser;
    ui_Sea_TextBrowser_Results->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Sea_TextBrowser_Results->setFont(m_currentFont);
    ui_Sea_TextBrowser_Results->setOpenLinks(false);
    resultsVBoxLayout->addWidget(ui_Sea_TextBrowser_Results);
    setBrowserBackground(*ui_Sea_TextBrowser_Results);

    QVBoxLayout *optionsVBoxLayout = new QVBoxLayout;
    resultsOptionsHBoxLayout->addLayout(optionsVBoxLayout);

    QLabel *optionsLabel = new QLabel;
    m_labels[12] = optionsLabel;
    optionsVBoxLayout->addWidget(optionsLabel);

    QHBoxLayout *translationHBoxLayout = new QHBoxLayout;
    optionsVBoxLayout->addLayout(translationHBoxLayout);

    QLabel *translationLabel = new QLabel;
    m_labels[13] = translationLabel;
    translationHBoxLayout->addWidget(translationLabel);

    QStringList translationNames;
    for (const ModuleData &md : m_modules) {
        translationNames << md.name;
    }
    ui_Sea_ComboBox_Translation = new QComboBox;
    ui_Sea_ComboBox_Translation->addItems(translationNames);
    ui_Sea_ComboBox_Translation->setCurrentIndex(ui_Bib_TabBar_Modules->currentIndex());
    ui_Sea_ComboBox_Translation->setStyleSheet(COMBOBOX_STYLE);
    translationHBoxLayout->addWidget(ui_Sea_ComboBox_Translation);

    QHBoxLayout *resPerPageHBoxLayout = new QHBoxLayout;
    optionsVBoxLayout->addLayout(resPerPageHBoxLayout);

    QLabel *resPerPageLabel = new QLabel;
    m_labels[14] = resPerPageLabel;
    resPerPageHBoxLayout->addWidget(resPerPageLabel);

    ui_Sea_ComboBox_ResPerPage = new QComboBox;
    QStringList resPerPageOptions;
    resPerPageOptions << "10" << "15" << "25" << "30" << "40" << "50" << "75" << "100";
    ui_Sea_ComboBox_ResPerPage->addItems(resPerPageOptions);
    ui_Sea_ComboBox_ResPerPage->setMaximumWidth(45);
    ui_Sea_ComboBox_ResPerPage->setCurrentIndex(2);
    ui_Sea_ComboBox_ResPerPage->setStyleSheet(COMBOBOX_STYLE);
    resPerPageHBoxLayout->addWidget(ui_Sea_ComboBox_ResPerPage);

    ui_Sea_CheckBox_Case = new QCheckBox;
    optionsVBoxLayout->addWidget(ui_Sea_CheckBox_Case);

    ui_Sea_CheckBox_WholeWords = new QCheckBox;
    optionsVBoxLayout->addWidget(ui_Sea_CheckBox_WholeWords);

    ui_Sea_RadioButton_Exact = new QRadioButton;
    ui_Sea_RadioButton_Exact->setChecked(true);
    optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Exact);

    ui_Sea_RadioButton_All = new QRadioButton;
    optionsVBoxLayout->addWidget(ui_Sea_RadioButton_All);

    ui_Sea_RadioButton_Any = new QRadioButton;
    optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Any);

    ui_Sea_RadioButton_Strong = new QRadioButton;
    ui_Sea_RadioButton_Strong->setEnabled(m_modules[ui_Sea_ComboBox_Translation->currentIndex()].hasStrong);
    optionsVBoxLayout->addWidget(ui_Sea_RadioButton_Strong);

    optionsVBoxLayout->addStretch();

    QHBoxLayout *randomVerseHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(randomVerseHBoxLayout);

    QVBoxLayout *randomVerseVBoxLayout = new QVBoxLayout;
    randomVerseHBoxLayout->addLayout(randomVerseVBoxLayout);

    QHBoxLayout *prevNextHBoxLayout = new QHBoxLayout;
    randomVerseVBoxLayout->addLayout(prevNextHBoxLayout);

    ui_Sea_Button_Prev = new QPushButton("<<");
    ui_Sea_Button_Prev->setDisabled(true);
    prevNextHBoxLayout->addWidget(ui_Sea_Button_Prev);

    ui_Sea_Button_Next = new QPushButton(">>");
    ui_Sea_Button_Next->setDisabled(true);
    prevNextHBoxLayout->addWidget(ui_Sea_Button_Next);

    ui_Sea_Button_RandomVerse = new QPushButton;
    randomVerseVBoxLayout->addWidget(ui_Sea_Button_RandomVerse);

    ui_Sea_TextBrowser_RandomVerse = new QTextBrowser;
    ui_Sea_TextBrowser_RandomVerse->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Sea_TextBrowser_RandomVerse->setFont(m_currentFont);
    ui_Sea_TextBrowser_RandomVerse->setMaximumHeight(80);
    randomVerseHBoxLayout->addWidget(ui_Sea_TextBrowser_RandomVerse);
    setBrowserBackground(*ui_Sea_TextBrowser_RandomVerse);

    connectSearchTabSignals();

    setLabelTexts(2);
    setButtonTexts(2);
    setCheckBoxTexts(2);
}



void MainWindow::generateTopicsTab(int idx)
{
    loadBackgroundPixmap();

    QWidget *tabDictionaryWidget = ui_TabWidget_Main->widget(idx);

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout(tabDictionaryWidget);
    mainHBoxLayout->setSpacing(5);
    mainHBoxLayout->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout *topicsVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(topicsVBoxLayout);

    QLabel *topicsLabel = new QLabel(tr("Topics:"));
    topicsVBoxLayout->addWidget(topicsLabel);

    QListWidget *topicsListWidget = new QListWidget;
    topicsListWidget->setMaximumWidth(220);
    topicsVBoxLayout->addWidget(topicsListWidget);

    QVBoxLayout *versesVBoxLayout = new QVBoxLayout;
    mainHBoxLayout->addLayout(versesVBoxLayout);

    QLabel *versesLabel = new QLabel(tr("Verses:"));
    versesVBoxLayout->addWidget(versesLabel);

    ui_Top_TextBrowser_Verses = new QTextBrowser;
    versesVBoxLayout->addWidget(ui_Top_TextBrowser_Verses);
    setBrowserBackground(*ui_Top_TextBrowser_Verses);
}

void MainWindow::saveSettings()
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.setValue(SET_LANGUAGE, m_language);
    settings.setValue(SET_MAX_RECENT_PASSAGES, m_maxRecentPassages);
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
    settings.beginGroup(GROUP_FONT);
    settings.setValue(SET_FONT_FAMILY, m_currentFont.family());
    settings.setValue(SET_FONT_SIZE, m_currentFont.pointSize());
    settings.endGroup();
    settings.beginGroup(GROUP_APPEARANCE);
    settings.setValue(SET_STYLE, m_style);
    settings.setValue(SET_USE_BACKGROUND, m_useBackground);
    settings.setValue(SET_HIGHLIGHT_COLOR, m_highlightColor);
    settings.setValue(SET_TAB_POSITION, m_tabPos);
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
    text.replace(QStringLiteral("<CM>"), QStringLiteral("<br>"));
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    QRegularExpressionMatchIterator iter = noteRgx.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            m_globalNotes[idx] << original;
            text.replace(original, QStringLiteral("<a href='c:") % QString::number(m_noteCount) %
                         QStringLiteral("' style='text-decoration:none'><b>*</b></a> "));
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
                text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
}

QString MainWindow::formatResult(QString &text, const QRegExp &regex, bool hasStrong)
{
    text.remove(regex);
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(QStringLiteral("<CM>"));
    if (hasStrong) {
        QRegularExpression regex("<W[^<]*>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
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

void MainWindow::on_Bib_CursorPositionChanged_chapterBrowser()
{
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    int idx = ui_Bib_TabBar_Modules->currentIndex();
    if (!m_lastCursors[idx].isNull()) {
        m_lastCursors[idx].setBlockFormat(format);
    }
    QTextCursor cursor = m_chapterBrowsers[idx]->textCursor();
    format.setBackground(m_highlightColor);
    cursor.setBlockFormat(format);
    m_lastCursors[idx] = cursor;
}

void MainWindow::on_CurrentChanged_TabWidget_Main(int index)
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
    for (int i = 0; i < m_loadedFlags.size(); ++i) {
        m_loadedFlags[i] = false;
    }
}

void MainWindow::on_Bib_CurrentIndexChanged_ComboBox_VerseTo(int index)
{
    if (index < ui_Bib_ComboBox_VerseFrom->currentIndex()) {
        ui_Bib_ComboBox_VerseTo->blockSignals(true);
        ui_Bib_ComboBox_VerseTo->setCurrentIndex(ui_Bib_ComboBox_VerseFrom->currentIndex());
        ui_Bib_ComboBox_VerseTo->blockSignals(false);
    }
    loadPassage();
    for (int i = 0; i < m_loadedFlags.size(); ++i) {
        m_loadedFlags[i] = false;
    }
}

void MainWindow::on_Sea_Clicked_PushButton_Prev()
{
    ui_Sea_Button_Next->setEnabled(true);
    m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
    int startIdx = qMax(0, m_crntStartRes - 2 * m_numResPerPage);
    int endIdx = startIdx + m_numResPerPage;
    displaySearchResults(startIdx, endIdx);
    m_crntStartRes = endIdx;
    ui_Sea_Button_Prev->setDisabled(startIdx == 0);
}

void MainWindow::on_Bib_Clicked_PushButton_Prev()
{
    if (ui_Bib_ListWidget_Chapter->currentRow() > 0) {
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->currentRow() - 1);
    } else {
        ui_Bib_ListWidget_Book->setCurrentRow(ui_Bib_ListWidget_Book->currentRow() - 1);
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->count() - 1);
    }
}

void MainWindow::on_Bib_Clicked_PushButton_Next()
{
    if (ui_Bib_ListWidget_Chapter->currentRow() < ui_Bib_ListWidget_Chapter->count() - 1) {
        ui_Bib_ListWidget_Chapter->setCurrentRow(ui_Bib_ListWidget_Chapter->currentRow() + 1);
    } else {
        ui_Bib_ListWidget_Book->setCurrentRow(ui_Bib_ListWidget_Book->currentRow() + 1);
    }
}

void MainWindow::on_Bib_Clicked_PushButton_Random()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    QSqlQuery query(m_modules[idx].database);
    if (query.exec("SELECT * FROM Bible"
                   " ORDER BY RANDOM() LIMIT 1")) {
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
}

void MainWindow::on_Sea_Clicked_PushButton_Next()
{
    ui_Sea_Button_Prev->setEnabled(true);
    m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
    int endIdx = m_crntStartRes + m_numResPerPage;
    displaySearchResults(m_crntStartRes, endIdx);
    m_crntStartRes = endIdx;
    ui_Sea_Button_Next->setDisabled(endIdx >= m_resVerses.count());
}

void MainWindow::on_Sea_Clicked_PushButton_RandomVerse()
{
    int idx = ui_Sea_ComboBox_Translation->currentIndex();
    int min = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
    if (min < 40 && m_modules[idx].hasOldTestament) {
        QString bookFrom = QString::number(ui_Sea_ComboBox_SearchFrom->currentIndex() + 1);
        QString bookTo = QString::number(ui_Sea_ComboBox_SearchTo->currentIndex() + 1);
        QSqlQuery query(m_modules[idx].database);
        if (query.exec("SELECT * FROM Bible"
                       " WHERE Book >= " + bookFrom + " AND Book <= " + bookTo +
                       " ORDER BY RANDOM() LIMIT 1")) {
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
            break;
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
    m_lastCursors.swap(from, to);
    m_loadedFlags.swap(from, to);
    m_verseMaps.swap(from, to);
    swapTabHistory(from, to);
    if (ui_TabWidget_Main->widget(2)->children().count() > 0 && ui_Sea_ComboBox_Translation->count() > 1) {
        QStringList list;
        for (int i = 0; i < ui_Sea_ComboBox_Translation->count(); ++i) {
            list << ui_Sea_ComboBox_Translation->itemText(i);
        }
        QString text = ui_Sea_ComboBox_Translation->currentText();
        ui_Sea_ComboBox_Translation->blockSignals(true);
        list.swap(from, to);
        ui_Sea_ComboBox_Translation->clear();
        ui_Sea_ComboBox_Translation->addItems(list);
        ui_Sea_ComboBox_Translation->setCurrentText(text);
        ui_Sea_ComboBox_Translation->blockSignals(false);
    }
}

void MainWindow::on_Bib_TabCloseRequested_TabWidget_Modules(int index)
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
        m_lastCursors.removeAt(index);
        m_loadedFlags.removeAt(index);
        m_verseMaps.removeAt(index);
        removeTabFromHistory(index);
        ui_Bib_TabWidget_Modules->removeTab(index);
        if (ui_TabWidget_Main->widget(2)->children().count() > 0 && ui_Sea_ComboBox_Translation->count() > 0) {
            ui_Sea_ComboBox_Translation->removeItem(index);
        }
    }
}

void MainWindow::on_Bib_CurrentChanged_TabWidget_Modules(int index)
{
    if (ui_Bib_ComboBox_VerseFrom->count() > 0 && ui_Bib_ComboBox_VerseTo->count() > 0) {
        if (!m_loadedFlags[index]) {
            loadPassage();
        }
    }
    QWidget *wg = ui_TabWidget_Main->widget(4);
    if (wg->children().count() > 0) {
        if (ui_Fav_ListWidget_Passages->count() > 0) {
            int crntSel = ui_Fav_ListWidget_Passages->currentRow();
            on_Fav_CurrentRowChanged_ListWidget_Passages(crntSel);
        }
    }
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
    for (ModuleData &md : m_modules) {
        closeDatabase(md.database);
    }
    closeDatabase(m_dbCntr);
    closeDatabase(m_dbXRef);
    closeDatabase(m_dbDict);
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
    if (m_useBackground) {
        QPalette palette;
        palette.setBrush(browser.viewport()->backgroundRole(), QBrush(m_papyrusBckgrnd));
        browser.viewport()->setPalette(palette);
    }
}

void MainWindow::loadBackgroundPixmap()
{
    if (m_papyrusBckgrnd.isNull()) {
        m_papyrusBckgrnd = QPixmap(IMG_BACKGROUND);
    }
}

void MainWindow::loadDictPaths()
{
    QDir dir(m_executionPath + "/App/dictionaries/");
    QStringList filters;
    filters << "*.dct.mybible";
    dir.setNameFilters(filters);
    QFileInfoList dictList = dir.entryInfoList();
    for (const QFileInfo &file : dictList) {
        m_dictPathList << file.absoluteFilePath();
    }
}

void MainWindow::addModuleLayout(int idx)
{
    QTextBrowser *chapterBrowser = new QTextBrowser(ui_Bib_TabWidget_Modules->widget(idx));
    chapterBrowser->setFont(m_currentFont);
    chapterBrowser->setOpenLinks(false);
    chapterBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    setBrowserBackground(*chapterBrowser);
    QObject::connect(chapterBrowser, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_Bib_AnchorClicked_ChapterBrowser(QUrl)));
    QObject::connect(chapterBrowser, SIGNAL(highlighted(QUrl)),
                     this, SLOT(on_Bib_Highlighted_ChapterBrowser(QUrl)));
    QObject::connect(chapterBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(on_Bib_CustomContextMenuRequested_ChapterBrowser(const QPoint &)));
    QObject::connect(chapterBrowser, SIGNAL(selectionChanged()),
                     this, SLOT(on_Bib_SelectionChanged_ChapterBrowser()));
    QHBoxLayout *chapterLayout = new QHBoxLayout(ui_Bib_TabWidget_Modules->widget(idx));
    chapterLayout->setContentsMargins(5, 5, 5, 5);
    chapterLayout->addWidget(chapterBrowser);
    m_chapterBrowsers << chapterBrowser;
    m_globalNotes << QStringList();
    m_lastCursors << QTextCursor();
    m_loadedFlags << false;
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
                   " Book = " % QString::number(tbcvv.book) %
                   " AND Chapter = " % QString::number(tbcvv.chapter)) ) {
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
            int index = m_favorites.indexOf({ 0,
                                              book.toInt(),
                                              chapter.toInt(),
                                              m_verseRange.first,
                                              m_verseRange.second } );
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

void MainWindow::on_Bib_Clicked_PushButton_Close()
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
        extraSelections << extra;
    }
    m_chapterBrowsers[idx]->setExtraSelections(extraSelections);
    if (!text.isNull() && !text.isEmpty()) {
        extra.cursor.clearSelection();
        m_chapterBrowsers[idx]->setTextCursor(extra.cursor);
    }
}

void MainWindow::on_Com_AnchorClicked_TextBrowser_Compare(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 't') {
        highlightPassage(TabBookChapterVerses ( argString.split(":")[1].toInt(),
                                                m_comVerse[0].toInt(),
                                                m_comVerse[1].toInt(),
                                                m_comVerse[2].toInt(),
                                                m_comVerse[2].toInt() ));
    } else if (firstChar == 's') {
        openStrongDialog(argString.split(":")[1]);
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

void MainWindow::on_Fav_AnchorClicked_TextBrowser_Passage(const QUrl &arg1)
{
    QString argString = arg1.toString();
    if (!argString.isNull() && !argString.isEmpty()) {
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
            openStrongDialog(argString);
       }
    }
}

void MainWindow::on_Dic_TextChanged_ListWidget_AllEntries(const QString &currentText)
{
    QSqlQuery query(m_dbDict);
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
    QString id = argString.left(2);
    if (id == "#d" || id == "#s") {
        QString entry = argString.right(argString.length() - 2);
        QSqlQuery query(m_dbDict);
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
        highlightPassage({ dbIdx,
                           indices[0].toInt(),
                           indices[1].toInt(),
                           indices[2].toInt(),
                           indices[2].toInt() });
    }
}

void MainWindow::on_Dic_Highlighted_TextBrowser_Definition(const QUrl &arg1)
{
    QString argString = arg1.toString();
    if (!argString.isNull() && !argString.isEmpty()) {
        QString id = argString.left(2);
        if (id == "#b") {
            int idx = ui_Bib_TabWidget_Modules->currentIndex();
            QString entry = argString.right(argString.length() - 2);
            QStringList indices = entry.split(".");
            QString queryString = QStringLiteral("SELECT Scripture FROM Bible "
                                                 "WHERE Book = %1 "
                                                 "AND Chapter = %2 "
                                                 "AND Verse = %3").arg(indices[0], indices[1], indices[2]);
            QSqlQuery query(m_modules[idx].database);
            if (query.exec(queryString)) {
                if (query.next()) {
                    QString verse = query.record().value(0).toString();
                    QRegExp rgx("<RF>.*<Rf>");
                    rgx.setMinimal(true);
                    verse = formatResult(verse, rgx, m_modules[idx].hasStrong).trimmed();
                    QString fntFam = "font-family:" % m_currentFont.family();
                    QString fntSiz = "font-size:" % QString::number(QFontInfo(m_currentFont).pixelSize()) % "px";
                    QString note = QString("<p style='%1;%2'>%3</p>")
                            .arg(fntFam, fntSiz, verse);
                    QToolTip::showText(QCursor::pos(), note, nullptr, QRect(), 2147483647);
                }
            }
        }
    } else {
        QToolTip::hideText();
    }
}

void MainWindow::on_Dic_CurrentRowChanged_ListWidget_Dictionaries(int currentRow)
{
    closeDatabase(m_dbDict);
    m_dbDict = QSqlDatabase::addDatabase("QSQLITE", m_dictPathList[currentRow]);
    m_dbDict.setDatabaseName(m_dictPathList[currentRow]);
    if (!m_dbDict.open()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
    }
    QSqlQuery fillQuery(m_dbDict);
    QString queryString = "SELECT word FROM dictionary WHERE relativeorder > 0";
    QStringList dictEntryList;
    if (fillQuery.exec(queryString)) {
        while (fillQuery.next()) {
            dictEntryList << fillQuery.record().value(0).toString();
        }
    }
    ui_Dic_ListWidget_AllEntries->blockSignals(true);
    ui_Dic_ListWidget_AllEntries->clear();
    ui_Dic_ListWidget_AllEntries->addItems(dictEntryList);
    if (ui_Dic_ListWidget_AllEntries->count() > 0) {
        ui_Dic_ListWidget_AllEntries->scrollToTop();
    }
    ui_Dic_ListWidget_AllEntries->blockSignals(false);
}

void MainWindow::on_Sea_AnchorClicked_TextBrowser_Results(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        openStrongDialog(argString);
    } else {
        int dbIdx = ui_Sea_ComboBox_Translation->currentIndex();
        QStringList indices = argString.split(",");
        highlightPassage(TabBookChapterVerses { dbIdx,
                                                indices[0].toInt(),
                                                indices[1].toInt(),
                                                indices[2].toInt(),
                                                indices[2].toInt() });
    }
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
                this, SLOT(on_Bib_Clicked_PushButton_Close()));

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
    if (query.next()) {
        ui_Fav_TextEdit_Comment->setPlainText(query.record().value(0).toString());
    }
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
    passage += "<b>—" % m_bookNames[m_favorites[currentRow].book - 1] % " " % chapter % ":" % verseFirst;
    if (verseFirst != verseLast) {
        passage += "-" % verseLast % "</b>";
    } else {
        passage += "</b>";
    }
    ui_Fav_TextBrowser_Passage->setHtml(passage);
    ui_Fav_Button_Delete->setEnabled(true);
    ui_Fav_Button_Save->setEnabled(true);
}

void MainWindow::on_Fav_Clicked_PushButton_Delete()
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
                on_Fav_CurrentRowChanged_ListWidget_Passages(0);
            }
            ui_Fav_ListWidget_Passages->blockSignals(false);
            ui_Fav_Button_Delete->setEnabled(ui_Fav_ListWidget_Passages->count() > 0);
            ui_Fav_Button_Save->setEnabled(ui_Fav_ListWidget_Passages->count() > 0);
        }
    }
}

void MainWindow::on_Fav_Clicked_PushButton_Save()
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
    for (const QFileInfo &file : moduleList) {
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
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Open MYBIBLE Module"), "/",
                tr("MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)"));
    if (!filename.isNull() && !filename.isEmpty()) {
        if (ui_Bib_TabWidget_Modules->tabText(0) == "No module found") {
            ui_Bib_TabWidget_Modules->removeTab(0);
        }
        if (!m_modulePathsList.contains(filename)) {
            if (loadBibleModule(filename)) {
                m_modulePathsList << filename;
                ui_Bib_TabWidget_Modules->setCurrentIndex(ui_Bib_TabWidget_Modules->count() - 1);
                if (m_removedPathsList.contains(filename)) {
                    m_removedPathsList.removeAt(m_removedPathsList.indexOf(filename));
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
    if (query.exec(queryString)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            moduleName = record.value(0).toString();
            hasOldTestament = record.value(1).toBool();
        }
    }
    queryString = "SELECT Scripture FROM Bible";
    if (query.exec(queryString)) {
        if (query.next()) {
            hasStrong = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
        }
    }
    bool containsModule = false;
    for (const ModuleData &md : m_modules) {
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
    if (!argString.isNull() && !argString.isEmpty()) {
        QStringList argSplit = argString.split(":");
        if (argSplit[0] == "c") {
            int idx = ui_Bib_TabWidget_Modules->currentIndex();
            int subIdx = argSplit[1].toInt();
            QString markupText = m_globalNotes[idx][subIdx];
            QString plainText = markupText.mid(4, markupText.size() - 8);
            plainText.replace("[i]", "<i>").replace("[/i]", "</i>");
            // plainText.replace("[0A]", "<font color=#00000a>").replace("[0a]", "</font>");
            QString fntFam = "font-family:" % m_currentFont.family();
            QString fntSiz = "font-size:" % QString::number(QFontInfo(m_currentFont).pixelSize()) % "px";
            QString note = QString("<p style='white-space:pre;%1;%2'>%3</p>")
                    .arg(fntFam, fntSiz, plainText);
            QToolTip::showText(QCursor::pos(), note, nullptr, QRect(), 2147483647);
        }
    } else {
        QToolTip::hideText();
    }
}

void MainWindow::loadXRefDatabase()
{
    QString path = m_executionPath + "/App/data/xref.bblv";
    if (QFileInfo(path).exists()) {
        if (!m_dbXRef.isOpen()) {
            m_dbXRef = QSqlDatabase::addDatabase("QSQLITE", "CrossReferences");
            m_dbXRef.setDatabaseName(path);
            m_dbXRef.open();
        }
    }
}

void MainWindow::openStrongDialog(const QString &number)
{
    if (!m_dbDict.isOpen()) {
        if (m_dictPathList.count() > 0) {
            m_dbDict = QSqlDatabase::addDatabase("QSQLITE", "Strong");
            m_dbDict.setDatabaseName(m_dictPathList[0]);
            if (!m_dbDict.open()) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open the database."));
            }
        }
    }
    PDialogStrong strongDialog(m_dbDict, number, m_currentFont, m_papyrusBckgrnd, m_useBackground, this);
    strongDialog.exec();
}

void MainWindow::updateFonts()
{
    QList<QTextEdit *> widgets = QMainWindow::centralWidget()->findChildren<QTextEdit *>();
    for (QTextEdit *te : widgets) {
        te->setFont(m_currentFont);
    }
}

void MainWindow::on_Bib_AnchorClicked_ChapterBrowser(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        openStrongDialog(argString);
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
                            m_useBackground,
                            m_currentFont);
        dlgXRef.exec();
    }
}

void MainWindow::actionExit()
{
    QMainWindow::close();
    QApplication::quit();
}

void MainWindow::actionHelp()
{

}

void MainWindow::actionWordFrequency()
{
    int idx = ui_Bib_TabWidget_Modules->currentIndex();
    PWindowHistogram *histogramWindow = new PWindowHistogram(m_modules[idx].database, m_bookNames);
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

void MainWindow::actionCheckLanguage(int idx, bool firstRun)
{
    QList<QAction *> languageActions = ui_Menu_Language->actions();
    for (int i = 0; i < languageActions.count(); ++i) {
        languageActions[i]->setChecked(i == idx);
    }
    m_language = m_languages.value(idx);
    if (!firstRun) {
        if (m_language != "EN") {
            m_tsApp->load(m_language.toLower(), m_executionPath + "/App/lang");
            qApp->installTranslator(m_tsApp);
            m_tsQt->load("qt_" + m_language.toLower(), m_executionPath + "/App/lang");
            qApp->installTranslator(m_tsQt);
        } else {
            qApp->removeTranslator(m_tsApp);
            qApp->removeTranslator(m_tsQt);
        }
        translateTexts();
    }
}

void MainWindow::actionEnglish()
{
    actionCheckLanguage(0);
}

void MainWindow::actionPolish()
{
    actionCheckLanguage(2);
}

void MainWindow::actionSpanish()
{
    actionCheckLanguage(1);
}

void MainWindow::actionPreferences()
{
    PDialogPreferences dlgPreferences(m_languages.key(m_language),
                                      m_maxRecentPassages,
                                      m_style,
                                      m_useBackground,
                                      m_highlightColor,
                                      m_currentFont,
                                      m_tabPos);
    if (dlgPreferences.exec()) {
        int langIdx = dlgPreferences.getLanguageIndex();
        if (m_language != m_languages[langIdx]) {
            actionCheckLanguage(langIdx);
        }
        QString newStyle = dlgPreferences.getWindowStyle();
        if (newStyle != m_style) {
            m_style = newStyle;
            qApp->setStyle(QStyleFactory::create(m_style));
        }
        bool newUseBckgrnd = dlgPreferences.getUseBackground();
        if (newUseBckgrnd != m_useBackground) {
            m_useBackground = newUseBckgrnd;
            if (m_useBackground) {
                QList<QTextEdit *> widgets = QMainWindow::centralWidget()->findChildren<QTextEdit *>();
                for (QTextEdit *te : widgets) {
                    setBrowserBackground(*te);
                }
            } else {
                QList<QTextEdit *> widgets = QMainWindow::centralWidget()->findChildren<QTextEdit *>();
                for (QTextEdit *te : widgets) {
                    removeBrowserBackground(*te);
                }
            }
        }
        m_maxRecentPassages = dlgPreferences.getMaxRecentPassages();
        m_highlightColor = dlgPreferences.getHighlightColor();
        m_currentFont.setFamily(dlgPreferences.getFontFamily());
        m_currentFont.setPointSize(dlgPreferences.getFontSize());
        m_tabPos = dlgPreferences.getTabPosition();
        ui_Bib_TabWidget_Modules->setTabPosition((QTabWidget::TabPosition(m_tabPos)));
        updateFonts();
    }
}

void MainWindow::actionAbout()
{
    QMessageBox::about(this, tr("About Qt Bible Viewer"),
                tr("<p><b>Qt Bible Viewer</b> is an open-source application "
                   "currently developed by Damian Dzienniak. "
                   "The GitHub repositiory for the project can be found at "
                   "<a href='https://github.com/damian-dz/QtBibleViewer'> "
                   "github.com/damian-dz/QtBibleViewer</a>.</p>"));
}

void MainWindow::actionAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

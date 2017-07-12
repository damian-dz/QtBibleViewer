#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/PreferenceDialog.h"

#include <QFileInfo>

MainWindow::MainWindow(QString appDir, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      firstLoadBible(true),
      firstLoadCompare(true)
{
    ui->setupUi(this);
    lockCheckBoxes();
    loadSettings(appDir);
    loadXRefAndDict(appDir);
    QStringList modulePathList = getModuleNames(appDir);
    if (modulePathList.count() > 0) {
        foreach (QString file, modulePathList)
            loadBibleModule(file);
        addTranslationTabs();
    } else
        ui->bookListWidget->setDisabled(true);
    loadSettings(appDir, 1);
    statusLabel = new QLabel(this);
    ui->statusBar->addWidget(statusLabel);
    executionPath = appDir;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings(const QString &path, int counter)
{
    if (counter == 0)
        settingsPath = path + "/config/settings.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);
    if (counter == 0) {
        const QString setLanguage = settings.value("language").toString();
        if (!setLanguage.isEmpty()) {
            if (setLanguage == "ENG")
                changeLanguageToEnglish();
            else if (setLanguage == "PL")
                changeLanguageToPolski();
        } else
            changeLanguageToEnglish();
       loadWhenBookChanged = true;
       return;
    }
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);
    const int setTranslation = settings.value("translation").toInt();
    const QStringList setPassage = settings.value("passage").toStringList();
    if (!setPassage.isEmpty())
        setTabBookChapterVerses(setTranslation,
                                setPassage[0].toInt(),
                                setPassage[1].toInt(),
                                setPassage[2].toInt(),
                                setPassage[3].toInt());
    else
        ui->bookListWidget->setCurrentRow(0);
    const QString setFontFamily = settings.value("fontFamily").toString();
    const int setFontSize = settings.value("fontSize").toInt();
    if (!setFontFamily.isEmpty())
        changeFont(QFont(setFontFamily, setFontSize));
    loadWhenBookChanged = settings.value("loadWhenBookChanged").toBool();
    maxRecentPassages = settings.value("maxRecentPassages").toInt();
    recentVerse = settings.value("recentVerse").toStringList();
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue("geometry", QWidget::saveGeometry());
    settings.setValue("language", currentLanguage);
    settings.setValue("translation", currentTranslationTab);
    QStringList setPassage;
    setPassage << QString::number(ui->bookListWidget->currentRow())
               << QString::number(ui->chapterListWidget->currentRow())
               << QString::number(ui->verseFirstComboBox->currentIndex())
               << QString::number(ui->verseLastComboBox->currentIndex());
    settings.setValue("passage", setPassage);
    settings.setValue("fontFamily", fontFamily);
    settings.setValue("fontSize", fontSize);
    settings.setValue("loadWhenBookChanged", loadWhenBookChanged);
    settings.setValue("maxRecentPassages", maxRecentPassages);
    settings.setValue("recentVerse", recentVerse);
}

void MainWindow::lockCheckBoxes()
{
    ui->rightToLeftCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->rightToLeftCheckBox->setFocusPolicy(Qt::NoFocus);
    ui->oldTestamentCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->oldTestamentCheckBox->setFocusPolicy(Qt::NoFocus);
    ui->newTestamentCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->newTestamentCheckBox->setFocusPolicy(Qt::NoFocus);
    ui->strongsNumbersCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->strongsNumbersCheckBox->setFocusPolicy(Qt::NoFocus);
}

QStringList MainWindow::getModuleNames(QString path)
{
    QDir dir(path + "/modules");
    QStringList filters;
    filters << "*.bbl.mybible";
    dir.setNameFilters(filters);
    QFileInfoList moduleList = dir.entryInfoList();
    QStringList modulePathList;
    foreach (QFileInfo file, moduleList)
        modulePathList << file.absoluteFilePath();
    return modulePathList;
}

void MainWindow::loadBibleModule(const QString &modulePath)
{
    QSqlDatabase dbBbl;
    dbBbl = QSqlDatabase::addDatabase("QSQLITE", modulePath);
    dbBbl.setDatabaseName(modulePath);
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
        // unreliable
        // hasStrong = query.record().value(2).toBool();
    }
    queryString = "SELECT scripture FROM Bible";
    query.exec(queryString);
    // this actually checks whether the module has Strong's numbers
    if (query.next())
        hasStrong = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
    if (hasStrong && moduleName == "KJV")
        indexStrong = translations.count();
    translations.append({ dbBbl, moduleName, hasOldTestament, hasStrong });
}

void MainWindow::addSingleTranslation(int index)
{
    QTextBrowser *chapterBrowser = new QTextBrowser(ui->translationTabWidget->widget(index));
    chapterBrowser->setFont(QFont("", fontSize));
    chapterBrowser->setOpenLinks(false);
    chapterBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chapterBrowser, SIGNAL(highlighted(QUrl)), this, SLOT(chapterBrowser_highlighted(QUrl)));
    connect(chapterBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(chapterBrowser_anchorClicked(QUrl)));
    connect(chapterBrowser, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showBibleContextMenu(const QPoint&)));
    QHBoxLayout *chapterLayout = new QHBoxLayout(ui->translationTabWidget->widget(index));
    chapterLayout->setSpacing(6);
    chapterLayout->setContentsMargins(11, 11, 11, 11);
    chapterLayout->addWidget(chapterBrowser);
    chapterBrowsers.append(chapterBrowser);
    chapterLayouts.append(chapterLayout);
    globalNotes << QStringList();
}

void MainWindow::addTranslationTabs()
{
    for (int i = 0; i < translations.count(); ++i) {
        if (i == 0)
            ui->translationTabWidget->setTabText(i, translations[i].moduleName);
        else
            ui->translationTabWidget->addTab(new QWidget(this), translations[i].moduleName);
       addSingleTranslation(i);
    }
}

void MainWindow::loadXRefAndDict(const QString &path)
{
    dbXRef = QSqlDatabase::addDatabase("QSQLITE", "CrossReferences");
    dbXRef.setDatabaseName(path + "/xref/xref.bblv");
    dbXRef.open();
    dbDct = QSqlDatabase::addDatabase("QSQLITE", "Dictionary");
    dbDct.setDatabaseName(path + "/dict/strong_lite.dct.mybible");
    dbDct.open();
}

void MainWindow::setTabBookChapterVerses(int tab, int book, int chapter, int verseFirst, int verseLast)
{
    ui->translationTabWidget->setCurrentIndex(tab);
    ui->verseFirstComboBox->blockSignals(true);
    ui->verseLastComboBox->blockSignals(true);
    ui->bookListWidget->setCurrentRow(book);
    ui->chapterListWidget->setCurrentRow(chapter);
    ui->verseFirstComboBox->setCurrentIndex(verseFirst);
    ui->verseLastComboBox->setCurrentIndex(verseLast);
    ui->verseFirstComboBox->blockSignals(false);
    ui->verseLastComboBox->blockSignals(false);
    firstLoadBible = false;
}

QString MainWindow::formatText(QString text, bool hasStrong)
{
    int dbIndex = currentTranslationTab;
    text.replace(QStringLiteral("<i>"), QStringLiteral("[i]"))
            .replace(QStringLiteral("</i>"), QStringLiteral("[/i]"));
    text.replace(QStringLiteral("<font color=\"00000a\">"), QStringLiteral("[0A]"))
            .replace(QStringLiteral("</font>"), QStringLiteral("[0a]"));
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.replace("<CM>", "<br>");
    text.replace("<TS>", "<h3>").replace("<Ts>", "</h3>");
    QRegularExpression regex("<RF>[^<]*<Rf>");
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);
    globalNotes[dbIndex].clear();
    int noteCount = 0;
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            globalNotes[dbIndex] << original;
            //text.replace(original, "<a href='c:" % QString::number(noteCount) %
                       //  "' style='text-decoration: none'><b>*</b></a> ");
            text.replace(original, QStringLiteral("<a href='c:%1' style='text-decoration: none'><b>*</b></a> ")
                         .arg(QString::number(noteCount)));
            noteCount++;
        }
    }
    text.replace("[i]", "<i>").replace("[/i]", "</i>");
    if (hasStrong) {
        QRegularExpression regex("<W[HG][0-9]{1,4}>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QStringLiteral(" <a href ='") %
                             modified % QStringLiteral("'>") % modified %
                             QStringLiteral("</a>"));
                //text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
    return text;
}

void MainWindow::on_actionOpen_Bible_Module_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr(openCaption.toUtf8().constData()),
                                                    "/",
                                                    openBblFilter);
    if (filename.isNull() || filename.isEmpty())
        return;
    loadBibleModule(filename);
    int index = translations.count() - 1;
    if (index == 0) {
        ui->bookListWidget->setEnabled(true);
        ui->translationTabWidget->setTabText(index, translations[index].moduleName);
    } else
        ui->translationTabWidget->addTab(new QWidget(), translations[index].moduleName);
    addSingleTranslation(index);
    ui->translationComboBox->addItem(translations[index].moduleName);
    ui->translationTabWidget->setCurrentIndex(index);
    if (ui->bookListWidget->currentRow() == -1)
        ui->bookListWidget->setCurrentRow(0);
}


void MainWindow::on_actionEnglish_triggered()
{
    changeLanguageToEnglish();
}

void MainWindow::on_actionPolski_triggered()
{
    changeLanguageToPolski();
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 1)
        fillDetails();
    else if (index == 2) {
        if (ui->searchFromComboBox->count() == 0 && ui->searchToComboBox->count() == 0) {
            ui->searchFromComboBox->addItems(bookNames);
            ui->searchToComboBox->addItems(bookNames);
            ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
            ui->divisionComboBox->setCurrentIndex(0);
        }
        if (ui->translationComboBox->count() == 0) {
            QStringList translationNames;
            for (int i = 0; i < translations.count(); ++i)
                translationNames << translations[i].moduleName;
            ui->translationComboBox->addItems(translationNames);
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
        } else
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
    } else if (index == 3) {
        if (ui->compareBookListWidget->count() == 0)
            loadCompareTab();
    }  else if (index == 4) {
        if (!dbUsr.isOpen())
            loadFavoritesTab();
    } else if (index == 5) {
        if (ui->entriesListWidget->count() == 0)
            fillDictionaryEntriesWidget();
    }
}

void MainWindow::changeFont(const QFont &font)
{
    for (int i = 0; i < chapterBrowsers.count(); ++i)
        chapterBrowsers[i]->setFont(font);
    ui->descriptionTextBrowser->setFont(font);
    ui->commentsTextBrowser->setFont(font);
    ui->resultsTextBrowser->setFont(font);
    ui->randomVerseTextBrowser->setFont(font);
    ui->definitionTextBrowser->setFont(font);
    ui->compareTextBrowser->setFont(font);
    ui->favoritePassageTextBrowser->setFont(font);
    ui->favoriteCommentTextEdit->setFont(font);
    fontSize = font.pointSize();
    fontFamily = font.toString().split(",")[0];
}

void MainWindow::changeFontSize(bool increase)
{
    if (increase) {
        if (fontSize < 16)
            fontSize++;
        else
            fontSize += 2;
    } else {
        if (fontSize < 16)
            fontSize--;
        else
            fontSize -= 2;
    }
    ui->actionIncrease_Font_Size->setDisabled(fontSize >= 24);
    ui->actionDecrease_Font_Size->setDisabled(fontSize <= 8);
    QFont font;
    font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    changeFont(font);
}

//void MainWindow::fillEntriesWidget()
//{
//    QSqlQuery query(dbDct);
//    QString queryString = "SELECT word FROM dictionary WHERE relativeorder > 0";
//    query.exec(queryString);
//    QStringList dictEntryList;
//    while (query.next())
//        dictEntryList << query.record().value(0).toString();
//    ui->entriesListWidget->addItems(dictEntryList);
//    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
//    QValidator *validator = new QRegExpValidator(regex, this);
//    ui->searchDictionaryLineEdit->setValidator(validator);
//}

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog preferences(fontSize,
                                 fontFamily,
                                 currentLanguage,
                                 loadWhenBookChanged,
                                 maxRecentPassages);
    preferences.setModal(true);
    preferences.setFixedSize(400, 280);
    if (preferences.exec())  {
        changeFont(preferences.getFont());
        loadWhenBookChanged = preferences.loadFirstChapter();
        maxRecentPassages = preferences.getMaxRecentPassages();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::on_actionAbout_triggered()
{
//    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
//    qDebug() << cursor.selectionStart();
//    qDebug() << cursor.selectionEnd();
}

void MainWindow::on_actionCopy_triggered()
{
    on_copyButton_clicked();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_actionIncrease_Font_Size_triggered()
{
    changeFontSize(true);
}

void MainWindow::on_actionDecrease_Font_Size_triggered()
{
    changeFontSize(false);
}

void MainWindow::on_actionSearch_triggered()
{
    ui->tabWidget->setCurrentIndex(2);
}

void MainWindow::on_actionWord_Frequency_triggered()
{
    int index = currentTranslationTab;
    histogramForm = new HistogramForm(translations[index].database, currentLanguage);
    histogramForm->setAttribute(Qt::WA_DeleteOnClose);
    histogramForm->show();
}

void MainWindow::on_actionBack_triggered()
{
    auto indices = historyList[--indexHistory];
    sentByBackForward = true;
    setTabBookChapterVerses(indices.tab,
                            indices.book - 1,
                            indices.chapter - 1,
                            indices.verseFirst - 1,
                            indices.verseLast - 1);
    sentByBackForward = false;
}

void MainWindow::on_actionForward_triggered()
{
    auto indices = historyList[++indexHistory];
    sentByBackForward = true;
    setTabBookChapterVerses(indices.tab,
                            indices.book - 1,
                            indices.chapter - 1,
                            indices.verseFirst - 1,
                            indices.verseLast - 1);
    sentByBackForward = false;
}

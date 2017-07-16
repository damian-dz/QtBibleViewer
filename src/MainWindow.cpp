#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/HistogramForm.h"
#include "hdr/PreferenceDialog.h"

#include <QClipboard>
#include <QFileInfo>

MainWindow::MainWindow(const QString &appDir, const QString &lang, QTranslator &ts, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      firstLoadBible(true),
      firstLoadCompare(true),
      translatorInstalled(false)
{
    ui->setupUi(this);
    translator = &ts;
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
    if (lang == "ENG")
        ui->actionEnglish->setChecked(true);
    else if (lang == "PL")
        ui->actionPolski->setChecked(true);
    currentLanguage = lang;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateBooksAndDivisions();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::loadSettings(const QString &path, int counter)
{
    if (counter == 0)
        settingsPath = path + "/config/settings.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);
    if (counter == 0) {
        if (ui->bookListWidget->count() > 0)
            ui->bookListWidget->clear();
        if (ui->divisionComboBox->count() > 0)
            ui->divisionComboBox->clear();
        populateBookList();
        populateDivisionList();
        ui->bookListWidget->addItems(bookNames);
        ui->divisionComboBox->addItems(divisionNames);
        loadWhenBookChanged = true;
        return;
    }
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);
    const int setTranslation = settings.value("translation").toInt();
    const QStringList setPassage = settings.value("passage").toStringList();
    if (!setPassage.isEmpty()) {
        setTabBookChapterVerses(TabBookChapterVerses{ (uchar)setTranslation,
                                                      (uchar)setPassage[0].toInt(),
                                                      (uchar)setPassage[1].toInt(),
                                                      (uchar)setPassage[2].toInt(),
                                                      (uchar)setPassage[3].toInt() });
    } else
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

QStringList MainWindow::getModuleNames(const QString &path)
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

void MainWindow::loadBibleModule(const QString &path)
{
    QSqlDatabase dbBbl;
    dbBbl = QSqlDatabase::addDatabase("QSQLITE", path);
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
        // unreliable
        // hasStrong = query.record().value(2).toBool();
    }
    queryString = "SELECT scripture FROM Bible";
    query.exec(queryString);
    // this actually checks whether the module has Strong's numbers
    if (query.next())
        hasStrong = query.record().value(0).toString().contains(QRegExp(QStringLiteral("<W[HG][0-9]{1,4}>")));
    if (hasStrong)
        indexStrong = modules.count();
    modules.append({ dbBbl, moduleName, hasOldTestament, hasStrong });
}

void MainWindow::addSingleTranslation(int index)
{
    QTextBrowser *chapterBrowser = new QTextBrowser(ui->translationTabWidget->widget(index));
    chapterBrowser->setFont(QFont("", fontSize));
    chapterBrowser->setOpenLinks(false);
    chapterBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chapterBrowser, SIGNAL(highlighted(QUrl)), this, SLOT(chapterBrowser_highlighted(QUrl)));
    connect(chapterBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(chapterBrowser_anchorClicked(QUrl)));
    connect(chapterBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showBibleContextMenu(const QPoint &)));
    QHBoxLayout *chapterLayout = new QHBoxLayout(ui->translationTabWidget->widget(index));
    chapterLayout->setContentsMargins(5, 5, 5, 5);
    chapterLayout->addWidget(chapterBrowser);
    chapterBrowsers.append(chapterBrowser);
    chapterLayouts.append(chapterLayout);
    globalNotes << QStringList();
}

void MainWindow::addTranslationTabs()
{
    for (int i = 0; i < modules.count(); ++i) {
        ui->translationTabWidget->addTab(new QWidget(ui->tabWidget), modules[i].name);
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

void MainWindow::setTabBookChapterVerses(const TabBookChapterVerses &tbcvs)
{
    ui->translationTabWidget->setCurrentIndex(tbcvs.tab);
    ui->verseFirstComboBox->blockSignals(true);
    ui->verseLastComboBox->blockSignals(true);
    ui->bookListWidget->setCurrentRow(tbcvs.book);
    ui->chapterListWidget->setCurrentRow(tbcvs.chapter);
    ui->verseFirstComboBox->setCurrentIndex(tbcvs.verseFirst);
    ui->verseLastComboBox->setCurrentIndex(tbcvs.verseLast);
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
                                                    tr("Open MYBIBLE Module"),
                                                    "/",
                                                    tr("MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)"));
    if (filename.isNull() || filename.isEmpty())
        return;
    loadBibleModule(filename);
    int index = modules.count() - 1;
    if (index == 0) {
        ui->bookListWidget->setEnabled(true);
        ui->translationTabWidget->setTabText(index, modules[index].name);
    } else
        ui->translationTabWidget->addTab(new QWidget(ui->tabWidget), modules[index].name);
    addSingleTranslation(index);
    ui->translationComboBox->addItem(modules[index].name);
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
    if (index == 1) {
        if (ui->abbreviationLineEdit->text().isEmpty()) {
            connect(ui->descriptionTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showBasicContextMenu(const QPoint &)));
            connect(ui->commentsTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showBasicContextMenu(const QPoint &)));
        }
        fillDetails();
    }
    else if (index == 2) {
        if (ui->searchFromComboBox->count() == 0 && ui->searchToComboBox->count() == 0) {
            ui->searchFromComboBox->addItems(bookNames);
            ui->searchToComboBox->addItems(bookNames);
            ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
            ui->divisionComboBox->setCurrentIndex(0);
            connect(ui->resultsTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showBasicContextMenu(const QPoint &)));
            connect(ui->enterLineEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showEditContextMenu(const QPoint &)));
        }
        if (ui->translationComboBox->count() == 0) {
            QStringList translationNames;
            for (int i = 0; i < modules.count(); ++i)
                translationNames << modules[i].name;
            ui->translationComboBox->addItems(translationNames);
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
        } else
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
    } else if (index == 3) {
        if (ui->compareBookListWidget->count() == 0)
            loadCompareTab();
    } else if (index == 4) {
        if (!dbUsr.isOpen()) {
            loadFavoritesTab();
            connect(ui->favoritePassageTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showBasicContextMenu(const QPoint &)));
            connect(ui->favoriteCommentTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showEditContextMenu(const QPoint &)));
        } else if (ui->favoritePassagesListWidget->count() > 0)
            on_favoritePassagesListWidget_currentRowChanged(ui->favoritePassagesListWidget->currentRow());
    } else if (index == 5) {
        if (ui->entriesListWidget->count() == 0) {
            fillDictionaryEntriesWidget();
            connect(ui->definitionTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(showBasicContextMenu(const QPoint &)));
        }
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

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog preferences(fontSize,
                                 fontFamily,
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
    QScopedPointer<HistogramForm> histogramForm(new HistogramForm(modules[index].database));
    histogramForm->setAttribute(Qt::WA_DeleteOnClose);
    histogramForm.take()->show();
}

void MainWindow::on_actionBack_triggered()
{
    auto indices = history[--indexHistory];
    sentByBackForward = true;
    setTabBookChapterVerses(indices);
    sentByBackForward = false;
}

void MainWindow::on_actionForward_triggered()
{
    auto indices = history[++indexHistory];
    sentByBackForward = true;
    setTabBookChapterVerses(indices);
    sentByBackForward = false;
}

void MainWindow::actionCopy()
{
    if (textBrowser)
        textBrowser->copy();
    else if (textEdit)
        textEdit->copy();
    else if (lineEdit)
        lineEdit->copy();
}

void MainWindow::actionCut()
{
    if (textEdit)
        textEdit->cut();
    else if (lineEdit)
        lineEdit->cut();
}

void MainWindow::actionClear()
{
    if (textEdit)
        textEdit->clear();
    else if (lineEdit)
        lineEdit->clear();
}

void MainWindow::actionPaste()
{
    if (textEdit)
        textEdit->paste();
    else if (lineEdit)
        lineEdit->paste();
}

void MainWindow::actionSelectAll()
{
    if (textBrowser)
        textBrowser->selectAll();
    else if (textEdit)
        textEdit->selectAll();
    else if (lineEdit)
        lineEdit->selectAll();
}

void MainWindow::showBasicContextMenu(const QPoint &pos)
{
    textBrowser = qobject_cast<QTextBrowser *>(QObject::sender());
    textEdit = nullptr;
    lineEdit = nullptr;
    QPoint globalPos = textBrowser->mapToGlobal(pos);
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
    QTextCursor cursor = textBrowser->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[2]->setDisabled(textBrowser->toPlainText().isEmpty());
    contextMenu.exec(globalPos);
}

void MainWindow::showEditContextMenu(const QPoint &pos)
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
        textEdit = nullptr;
        textBrowser = nullptr;
        lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
        globalPos = lineEdit->mapToGlobal(pos);
        bool isSelected = (lineEdit->selectedText().length() > 0);
        contextActions[0]->setEnabled(isSelected);
        contextActions[1]->setEnabled(isSelected);
        bool isEmpty = lineEdit->text().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    } else if (senderName == "QTextEdit") {
        lineEdit = nullptr;
        textBrowser = nullptr;
        textEdit = qobject_cast<QTextEdit *>(QObject::sender());
        globalPos = textEdit->mapToGlobal(pos);
        QTextCursor cursor = textEdit->textCursor();
        contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        contextActions[1]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        bool isEmpty = textEdit->toPlainText().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    }
    contextActions[2]->setDisabled(qApp->clipboard()->text().isEmpty());
    contextMenu.exec(globalPos);
}

#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/CrossReferencePopup.h"
#include "hdr/PreferenceDialog.h"
#include "hdr/StrongPopup.h"

#include <QFileInfo>
#include <QToolTip>


MainWindow::MainWindow(QString appDir, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lockCheckBoxes();
    loadSettings(appDir);
    loadXReferencesAndDict(appDir);
    QStringList modulePathList = getModuleNames(appDir);
    if (modulePathList.count() > 0) {
        foreach (QString file, modulePathList)
            loadBibleModule(file);
        addTranslationTabs();
    } else
        ui->bookListWidget->setDisabled(true);
    loadSettings(appDir, 1);
}

void MainWindow::loadSettings(QString path, int counter)
{
    if (counter == 0)
        settingsPath = path + "/config/settings.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);
    if (counter == 0) {
        const QString setLanguage = settings.value("setLanguage").toString();
        if (!setLanguage.isEmpty()) {
            if (setLanguage == "ENG")
                changeLanguageToEnglish();
            else if (setLanguage == "PL")
                changeLanguageToPolski();
        } else
            changeLanguageToEnglish();
       return;
    }
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);
    const int setTranslation = settings.value("setTranslation").toInt();
    ui->translationTabWidget->setCurrentIndex(setTranslation);
    const QStringList setPassage = settings.value("setPassage").toStringList();
    if (!setPassage.isEmpty()) {
        ui->verseFirstComboBox->blockSignals(true);
        ui->verseLastComboBox->blockSignals(true);
        ui->bookListWidget->setCurrentRow(setPassage[0].toInt());
        ui->chapterListWidget->setCurrentRow(setPassage[1].toInt());
        ui->verseFirstComboBox->setCurrentIndex(setPassage[2].toInt());
        ui->verseLastComboBox->blockSignals(false);
        ui->verseLastComboBox->setCurrentIndex(setPassage[3].toInt());
        ui->verseFirstComboBox->blockSignals(false);
    } else
        ui->bookListWidget->setCurrentRow(0);
    const QString setFontFamily = settings.value("setFontFamily").toString();
    const int setFontSize = settings.value("setFontSize").toInt();
    if (!setFontFamily.isEmpty()) {
        QFont font;
        font.setFamily(setFontFamily);
        font.setPointSize(setFontSize);
        changeFont(font);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("setLanguage", currentLanguage);
    settings.setValue("setTranslation", currentTranslationTab);
    QStringList setPassage;
    setPassage << QString::number(ui->bookListWidget->currentRow())
               << QString::number(ui->chapterListWidget->currentRow())
               << QString::number(ui->verseFirstComboBox->currentIndex())
               << QString::number(ui->verseLastComboBox->currentIndex());
    settings.setValue("setPassage", setPassage);
    settings.setValue("setFontFamily", fontFamily);
    settings.setValue("setFontSize", fontSize);
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


void MainWindow::loadBibleModule(QString modulePath)
{
    QSqlDatabase dbBbl;
    dbBbl = QSqlDatabase::addDatabase("QSQLITE", modulePath);
    dbBbl.setDatabaseName(modulePath);
    dbBbl.open();
    QSqlQuery query(dbBbl);
    QString moduleName;
    bool hasOldTestament;
    bool hasStrong;
    QString queryString = "SELECT Abbreviation, OT, Strong FROM Details";
    query.exec(queryString);
    if (query.next()) {
        moduleName = query.record().value(0).toString();
        hasOldTestament = query.record().value(1).toBool();
        hasStrong = query.record().value(2).toBool();
    }
    databases.append(std::make_tuple(dbBbl, moduleName, hasOldTestament, hasStrong));
}

void MainWindow::addSingleTranslation(int index)
{
    QTextBrowser *chapterBrowser = new QTextBrowser(ui->translationTabWidget->widget(index));
    QFont font;
    font.setPointSize(fontSize);
    chapterBrowser->setFont(font);
    chapterBrowser->setOpenLinks(false);
    connect(chapterBrowser, SIGNAL(highlighted(QUrl)), this, SLOT(chapterBrowser_highlighted(QUrl)));
    connect(chapterBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(chapterBrowser_anchorClicked(QUrl)));
    QHBoxLayout *chapterLayout = new QHBoxLayout(ui->translationTabWidget->widget(index));
    chapterLayout->setSpacing(6);
    chapterLayout->setContentsMargins(11, 11, 11, 11);
    chapterLayout->addWidget(chapterBrowser);
    chapterBrowsers.append(chapterBrowser);
    chapterLayouts.append(chapterLayout);
    QStringList notes;
    globalNotes << notes;
}

void MainWindow::addTranslationTabs()
{
    for (int i = 0; i < databases.count(); ++i) {
        if (i == 0)
            ui->translationTabWidget->setTabText(i, std::get<1>(databases[i]));
        else
            ui->translationTabWidget->addTab(new QWidget(), std::get<1>(databases[i]));
       addSingleTranslation(i);
    }
}

void MainWindow::loadXReferencesAndDict(QString path)
{
    dbXRef = QSqlDatabase::addDatabase("QSQLITE", "CrossReferences");
    dbXRef.setDatabaseName(path + "/xref/xref.bblv");
    dbXRef.open();
    dbDct = QSqlDatabase::addDatabase("QSQLITE", "Dictionary");
    dbDct.setDatabaseName(path + "/dict/strong_lite.dct.mybible");
    dbDct.open();
}

QString MainWindow::formatText(QString text, bool hasStrong)
{
    int dbIndex = currentTranslationTab;
    text.replace("<i>", "[i]").replace("</i>", "[/i]");
    text.replace("<font color=\"00000a\">", "[0A]").replace("</font>", "[0a]");
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    text.replace("<CM>", "<br>");
    text.replace("<TS>", "<h3>").replace("<Ts>", "</h3>");
    QRegularExpression regex("<RF>[^<]*<Rf>");
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);
    int noteCount = 0;
    globalNotes[dbIndex].clear();
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            globalNotes[dbIndex] << original;
            text.replace(original, "<a href=\"c:" + QString::number(noteCount) +
                         "\" style='text-decoration: none'><b>*</b></a> ");
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
                text.replace(original, " <a href=\"" + modified + "\">" + modified + "</a>");
            }
        }
    }
    return text;
}

void MainWindow::on_actionOpen_Bible_Module_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr(openCaption.toUtf8().constData()), "/", openBblFilter);
    if (filename.isNull() || filename.isEmpty())
        return;
    loadBibleModule(filename);
    int index = databases.count() - 1;
    if (index == 0) {
        ui->bookListWidget->setEnabled(true);
        ui->translationTabWidget->setTabText(index, std::get<1>(databases[index]));
    } else
        ui->translationTabWidget->addTab(new QWidget(), std::get<1>(databases[index]));
    addSingleTranslation(index);
    ui->translationComboBox->addItem(std::get<1>(databases[index]));
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

QString formatPolish(QString text)
{
    text.replace("\\'b9", "ą");
    text.replace("\\'e6", "ć");
    text.replace("\\'ea", "ę");
    text.replace("\\'b3", "ł");
    text.replace("\\'f1", "ń");
    text.replace("\\'f3", "ó");
    text.replace("\\'9c", "ś");
    text.replace("\\'bf", "ż");
    text.replace("\\'9f", "ź");
    text.replace("\\'a9", "©");
    return text;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    int dbIndex = currentTranslationTab;
    if (index == 1) {
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
        QSqlQuery query(std::get<0>(databases[dbIndex]));
        query.exec(queryString);
        if (query.next()) {
            ui->descriptionTextBrowser->setHtml(query.record().value(0).toString());
            ui->abbreviationLineEdit->setText(query.record().value(1).toString());
            ui->commentsTextBrowser->setHtml(formatPolish(query.record().value(2).toString()));
            ui->versionLineEdit->setText(query.record().value(3).toString());
            ui->versionDateLineEdit->setText(query.record().value(4).toString());
            ui->publishDateLineEdit->setText(query.record().value(5).toString());
            ui->rightToLeftCheckBox->setChecked(query.record().value(6).toBool());
            ui->oldTestamentCheckBox->setChecked(query.record().value(7).toBool());
            ui->newTestamentCheckBox->setChecked(query.record().value(8).toBool());
            ui->strongsNumbersCheckBox->setChecked(query.record().value(9).toBool());
        }
    }
    else if (index == 2) {
        if (ui->searchFromComboBox->count() == 0 && ui->searchToComboBox->count() == 0) {
            ui->searchFromComboBox->addItems(bookNames);
            ui->searchToComboBox->addItems(bookNames);
            ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
            ui->divisionComboBox->setCurrentIndex(0);
        }
        if (ui->translationComboBox->count() == 0) {
            QStringList translations;
            for (int i = 0; i < databases.count(); i++)
                translations << std::get<1>(databases[i]);
            ui->translationComboBox->addItems(translations);
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
        } else
            ui->translationComboBox->setCurrentIndex(currentTranslationTab);
    }
}

void MainWindow::changeFont(QFont font)
{
    for (int i = 0; i < chapterBrowsers.count(); i++)
        chapterBrowsers[i]->setFont(font);
    ui->descriptionTextBrowser->setFont(font);
    ui->commentsTextBrowser->setFont(font);
    ui->resultsTextBrowser->setFont(font);
    ui->randomVerseTextBrowser->setFont(font);
    fontSize = font.pointSize();
    fontFamily = font.toString().split(",")[0];
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog preferences(fontSize, fontFamily, currentLanguage);
    preferences.setModal(true);
    preferences.setFixedSize(400, 228);
    if (preferences.exec())  {
        QFont font = preferences.getFont();
        changeFont(font);
    }
}

void MainWindow::chapterBrowser_highlighted(const QUrl &arg1)
{
    QString argString = arg1.toString();
    if (!argString.isNull()) {
        QStringList argSplit = argString.split(":");
        if (argSplit[0] == "c") {
            int index = currentTranslationTab;
            QPoint point = chapterBrowsers[index]->mapFromParent(QCursor::pos());
            double offset = 3.4 * fontSize;
            point.setY(point.y() - offset);
            QRect rect;
            int subIndex = argSplit[1].toInt();
            QString markupText = globalNotes[index][subIndex];
            QString plainText = markupText.mid(4, markupText.size() - 8);
            plainText.replace("[i]", "<i>").replace("[/i]", "</i>");
            plainText.replace("[0A]", "<font color=#00000a>").replace("[0a]", "</font>");
            QString note = "<p style='white-space:pre'>" + plainText + "</p>";
            QFont font;
            if (!fontFamily.isEmpty())
                font.setFamily(fontFamily);
            font.setPointSize(fontSize);
            QToolTip::setFont(font);
            QToolTip::showText(point, note, 0, rect, 2147483647);
        }
    } else
        QToolTip::hideText();
}

void MainWindow::chapterBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QFont font;
    if (!fontFamily.isEmpty())
       font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        StrongPopup strongDialog(dbDct, argString, font);
        strongDialog.exec();
    } else if (firstChar == 'x') {
        int dbIndex = currentTranslationTab;
        QStringList verseInfo;
        verseInfo << argString
                  << QString::number(ui->bookListWidget->currentRow())
                  << QString::number(ui->chapterListWidget->currentItem()->text().toInt());
        CrossReferencePopup xRefDialog(std::get<0>(databases[dbIndex]), verseInfo, bookNames, font);
        xRefDialog.exec();
    }
}

void MainWindow::on_resultsTextBrowser_highlighted(const QUrl &arg1)
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}

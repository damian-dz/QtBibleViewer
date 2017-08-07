#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/CrossReferencePopup.h"
#include "hdr/StrongPopup.h"

#include <QClipboard>
#include <QToolTip>

/* Bible Tab */
void MainWindow::loadPassage()
{
    //qDebug() << "loadPassage()";
    if (!loadFirstChapter)
        return;
    int dbIndex = currentTranslationTab;
    int book = ui->bookListWidget->currentRow() + 1;
    int chapter = ui->chapterListWidget->currentRow() + 1;
    int verseFirst = ui->verseFirstComboBox->currentIndex() + 1;
    int verseLast = ui->verseLastComboBox->currentIndex() + 1;
    if (book > 0 && chapter > 0 && verseFirst > 0 && verseLast > 0) {
        QRegExp headerRegex = QRegExp("<TS>.*<Ts>");
        QString bookStr = QString::number(book);
        QString chapterStr = QString::number(chapter);
        QString queryString = "SELECT Verse, Scripture FROM Bible"
                              " WHERE Book = " % bookStr %
                              " AND Chapter = " % chapterStr %
                              " AND Verse >=  " % QString::number(verseFirst) %
                              " AND Verse <= " % QString::number(verseLast);
        QSqlQuery query(modules[dbIndex].database);
        query.exec(queryString);
        QSqlQuery xRefQuery(dbXRef);
        currentPassage.clear();
        clipboardSet = false;
        while (query.next()) {
            QString verseNumber = query.record().value(0).toString();
            QString xRefQueryString = QStringLiteral("SELECT XRefs FROM CrossReferences WHERE BOOK = ") % bookStr %
                                      QStringLiteral(" AND Chapter = ") % chapterStr %
                                      QStringLiteral(" AND Verse = ") % verseNumber;
            xRefQuery.exec(xRefQueryString);
            if (xRefQuery.next())
                verseNumber = "<a href='x:" % verseNumber % ":" +
                        xRefQuery.record().value(0).toString() % "'>" + verseNumber % "</a>";
            QString currentVerse = "<b>" % verseNumber % "</b> " % query.record().value(1).toString() % "<br>";
            if (currentVerse.contains("<TS>")) {
                headerRegex.indexIn(currentVerse);
                currentVerse.remove(headerRegex);
                currentVerse = headerRegex.cap(0) % currentVerse;
            }
            currentPassage += currentVerse;
        }
        if (currentPassage.isNull()) {
            chapterBrowsers[dbIndex]->setHtml("<center><h2>" % tr("Unavailable in this module.") % "</center></h2>");
            return;
        }
        currentPassage = formatText(currentPassage, modules[dbIndex].hasStrong);
        chapterBrowsers[dbIndex]->setHtml(currentPassage);
    }
    ui->actionBack->setEnabled(history.count() > 1 && indexHistory > 0);
    ui->actionForward->setEnabled(history.count() > 1 && indexHistory < history.count() - 1);
    if (sentByBackForward)
        return;
    history.append({ (uchar)dbIndex,
                     uchar(book - 1),
                     uchar(chapter - 1),
                     uchar(verseFirst - 1),
                     uchar(verseLast - 1) });
    if (history.count() > maxRecentPassages)
        history.removeFirst();
    indexHistory = history.count() - 1;
    ui->actionBack->setEnabled(history.count() > 1 && indexHistory > 0);
    ui->actionForward->setDisabled(true);
}

void MainWindow::on_bookListWidget_currentRowChanged(int currentRow)
{
    if (languageChanging)
        return;
    int dbIndex = currentTranslationTab;
    QString queryString = QStringLiteral("SELECT Chapter FROM Bible WHERE Book = ") %
                          QString::number(currentRow + 1) %
                          QStringLiteral(" AND Verse = 1");
    loadFirstChapter = loadWhenBookChanged;
    ui->chapterListWidget->blockSignals(true);
    ui->chapterListWidget->clear();
    ui->chapterListWidget->blockSignals(false);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    QStringList chapterNumbers;
    while (query.next())
        chapterNumbers << query.record().value(0).toString();
    ui->chapterListWidget->addItems(chapterNumbers);
    if (chapterNumbers.count() == 0) {
        chapterBrowsers[dbIndex]->setHtml("<center><h2>" % tr("Unavailable in this module.") % "</center></h2>");
        return;
    }
    if (!firstLoadBible)
        ui->chapterListWidget->setCurrentRow(0);
    loadFirstChapter = true;
}

void MainWindow::on_chapterListWidget_currentRowChanged(int currentRow)
{
    ui->verseFirstComboBox->blockSignals(true);
    ui->verseLastComboBox->blockSignals(true);
    int book = ui->bookListWidget->currentRow() + 1;
    int chapter = currentRow + 1;
    ui->verseFirstComboBox->clear();
    ui->verseLastComboBox->clear();
    int dbIndex = currentTranslationTab;
    QSqlQuery query(modules[dbIndex].database);
    QString queryString = QStringLiteral("SELECT Verse FROM Bible WHERE Book = ") % QString::number(book) %
                          QStringLiteral(" AND Chapter = ") % QString::number(chapter);
    query.exec(queryString);
    QStringList verseNumbers;
    while (query.next())
        verseNumbers << query.record().value(0).toString();
    ui->verseFirstComboBox->addItems(verseNumbers);
    ui->verseLastComboBox->addItems(verseNumbers);
    if (!firstLoadBible)
        ui->verseFirstComboBox->setCurrentIndex(0);
    ui->verseFirstComboBox->blockSignals(false);
    ui->verseLastComboBox->blockSignals(false);
    ui->verseLastComboBox->setCurrentIndex(ui->verseLastComboBox->count() - 1);
    ui->prevChapterButton->setDisabled(
                ui->bookListWidget->currentRow() == 0 &&
                ui->chapterListWidget->currentRow() == 0);
    ui->nextChapterButton->setDisabled(
                ui->bookListWidget->currentRow() ==
                ui->bookListWidget->count() - 1 &&
                ui->chapterListWidget->currentRow() ==
                ui->chapterListWidget->count() - 1);
}

void MainWindow::on_verseFirstComboBox_currentIndexChanged(int index)
{
    if (index > ui->verseLastComboBox->currentIndex()) {
        ui->verseLastComboBox->setCurrentIndex(index);
        return;
    }
    loadPassage();
}

void MainWindow::on_verseLastComboBox_currentIndexChanged(int index)
{
    if (index < ui->verseFirstComboBox->currentIndex()) {
        ui->verseFirstComboBox->setCurrentIndex(index);
        return;
    }
    loadPassage();
}

void MainWindow::chapterBrowser_actionCopy()
{
    chapterBrowsers[currentTranslationTab]->copy();
}

void MainWindow::chapterBrowser_actionCopyFormatted()
{
    QString plainText = chapterBrowsers[currentTranslationTab]->toPlainText();
    QString start = QString::number(range.first) + "\\b";
    if (range.first > ui->verseFirstComboBox->currentText().toInt())
        start.prepend("(\n");
    else
        start.prepend("(");
    QString end;
    if (range.second ==
            ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt())
        end = ")\n";
    else
        end = ")\n" + QString::number(range.second + 1) + "\\b";
    QRegExp regex(start + ".*" + end);
    int pos = regex.indexIn(plainText);
    QString textToCopy;
    if (pos > -1)
        textToCopy = regex.cap(1);
    textToCopy.replace(QRegExp("\n"), " ");
    textToCopy.remove(QRegExp("\\*| \\*| \\* |\\* "));
    if (modules[currentTranslationTab].hasStrong) {
        textToCopy.remove(QRegExp(" [HG][0-9]{1,4}"));
        textToCopy.replace("  ", " ");
    }
    textToCopy = textToCopy.trimmed();
    textToCopy += "—" + bookNames[ui->bookListWidget->currentRow()] +
            " " + ui->chapterListWidget->currentItem()->text() +
            ":" + QString::number(range.first);
    if (range.first != range.second)
        textToCopy += "-" + QString::number(range.second);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(textToCopy);
    ui->statusBar->showMessage(tr("Text copied to clipboard."), 2500);
}

void MainWindow::chapterBrowser_actionSelectAll()
{
    chapterBrowsers[currentTranslationTab]->selectAll();
}

void MainWindow::getVerseRange()
{
    QString plainText = chapterBrowsers[currentTranslationTab]->toPlainText();
    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    int prev = 0;
    int crnt = 0;
    QRegularExpression regex("\n[0-9]{1,3}\\b");
    QRegularExpressionMatchIterator iter = regex.globalMatch(plainText);
    bool found = false;
    int firstVerse = ui->verseFirstComboBox->currentIndex() + 1;
    int lastVerse = ui->verseFirstComboBox->currentIndex() + 1;
    while (iter.hasNext() && !found) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString crntMatch = match.captured();
            crnt = plainText.indexOf(crntMatch);
            if (start > prev && start <= crnt)
                firstVerse = crntMatch.right(crntMatch.size() - 1).toInt() - 1;
            if (end > prev && end <= crnt) {
                lastVerse = crntMatch.right(crntMatch.size() - 1).toInt() - 1;
                found = true;
            }
            prev = crnt;
        }
    }
    crnt = plainText.lastIndexOf("\n");
    if (start > prev && start <= crnt)
        firstVerse = ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt();
    if (end > prev && end <= crnt + 2)
        lastVerse = ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt();
    range = qMakePair(firstVerse, lastVerse);
}


void MainWindow::chapterBrowser_actionAddToFavorites()
{
    ui->tabWidget->setCurrentIndex(4);
    insertIntoFavorites();
}

void MainWindow::showBibleContextMenu(const QPoint &pos)
{
    QPoint globalPos = chapterBrowsers[currentTranslationTab]->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(chapterBrowser_actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addAction(tr("Copy with Reference"),
                          this,
                          SLOT(chapterBrowser_actionCopyFormatted()));
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(chapterBrowser_actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    contextMenu.addSeparator();
    getVerseRange();
    QString addVerseMsg;
    if (range.first == range.second)
        addVerseMsg = tr("Add Verse ") + QString::number(range.first) + tr(" to Favorites");
    else
        addVerseMsg = tr("Add Verses ") + QString::number(range.first) +
                "-" + QString::number(range.second) + tr(" to Favorites");
    contextMenu.addAction(addVerseMsg,
                          this,
                          SLOT(chapterBrowser_actionAddToFavorites()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Back"),
                          this,
                          SLOT(on_actionBack_triggered()),
                          QKeySequence(tr("Ctrl+Left")));
    contextMenu.addAction(tr("Forward"),
                          this,
                          SLOT(on_actionForward_triggered()),
                          QKeySequence(tr("Ctrl+Right")));
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[1]->setEnabled(contextActions[0]->isEnabled());
    contextActions[4]->setDisabled(range.first == 0 && range.second == 0);
    contextActions[6]->setEnabled(ui->actionBack->isEnabled());
    contextActions[7]->setEnabled(ui->actionForward->isEnabled());
    contextMenu.exec(globalPos);
}

void cleanBeforeCopying(QString &text, bool hasStrong)
{
    if (hasStrong)
        text.replace(QRegExp(" <a href[^<]*</a>"), "");
    text.replace("<br><br>", " ");
    text.replace("<br>", " ");
    text.remove("*");
    text.replace("  ", " ");
    text.remove(QRegExp("<RF>.*<Rf>"));
    text.remove(QRegExp("<[^<]*>"));
    text.remove(QRegExp("<..>"));
}

void MainWindow::on_copyButton_clicked()
{
    if (clipboardSet) {
        QApplication::clipboard()->setText(currentPassage);
        return;
    }
    int index = currentTranslationTab;
    cleanBeforeCopying(currentPassage, modules[index].hasStrong);
    QString bookName = ui->bookListWidget->currentItem()->text();
    QString chapterNumber = ui->chapterListWidget->currentItem()->text();
    QString firstVerseNumber = ui->verseFirstComboBox->currentText();
    QString lastVerseNumber = ui->verseLastComboBox->currentText();
    int firstVerseIndex = ui->verseFirstComboBox->currentIndex();
    int lastVerseIndex = ui->verseLastComboBox->currentIndex();
    QString textToAppend;
    if (firstVerseIndex == 0 && lastVerseIndex + 1 == ui->verseLastComboBox->count()) {
        textToAppend = "—" + bookName +
                       " " + chapterNumber;
    } else if (firstVerseNumber != lastVerseNumber) {
        textToAppend = "—" + bookName +
                       " " + chapterNumber +
                       ":" + firstVerseNumber +
                       "-" + lastVerseNumber;
    } else {
        textToAppend = "—" + bookName +
                       " " + chapterNumber +
                       ":" + firstVerseNumber;
        currentPassage = currentPassage.right(currentPassage.length() - firstVerseNumber.length() - 1);
    }
    currentPassage.resize(currentPassage.length() - 1);
    currentPassage += textToAppend;
    QApplication::clipboard()->setText(currentPassage);
    ui->verseLineEdit->setText(currentPassage);
    ui->statusBar->showMessage(tr("Text copied to clipboard."), 2500);
    clipboardSet = true;
}

void MainWindow::on_prevChapterButton_clicked()
{
    if (ui->chapterListWidget->currentRow() > 0)
        ui->chapterListWidget->setCurrentRow(ui->chapterListWidget->currentRow() - 1);
    else {
        ui->bookListWidget->setCurrentRow(ui->bookListWidget->currentRow() - 1);
        ui->chapterListWidget->setCurrentRow(ui->chapterListWidget->count() - 1);
    }
}

void MainWindow::on_nextChapterButton_clicked()
{
    if (ui->chapterListWidget->currentRow() < ui->chapterListWidget->count() - 1)
        ui->chapterListWidget->setCurrentRow(ui->chapterListWidget->currentRow() + 1);
    else
        ui->bookListWidget->setCurrentRow(ui->bookListWidget->currentRow() + 1);
}


void MainWindow::on_translationTabWidget_currentChanged(int index)
{
    currentTranslationTab = index;
    if (!firstLoadBible)
        loadPassage();
    if (ui->chapterListWidget->count() == 0) {
        int currentIndex = ui->bookListWidget->currentRow();
        ui->bookListWidget->blockSignals(true);
        ui->bookListWidget->setCurrentRow(-1);
        ui->bookListWidget->blockSignals(false);
        ui->bookListWidget->setCurrentRow(currentIndex);
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
        StrongPopup strongDialog(dbDct, argString, font, this);
        strongDialog.exec();
    } else if (firstChar == 'x') {
        int dbIndex = currentTranslationTab;
        QStringList verseInfo;
        verseInfo << argString
                  << QString::number(ui->bookListWidget->currentRow())
                  << QString::number(ui->chapterListWidget->currentItem()->text().toInt());
        CrossReferencePopup xRefDialog(qMakePair(modules[dbIndex].database, dbDct),
                                       verseInfo, bookNames, font, this);
        xRefDialog.exec();
    }
}



#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include <QClipboard>

/* Bible Tab */
void MainWindow::loadPassage()
{
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
                              " WHERE Book = " + bookStr +
                              " AND Chapter = " + chapterStr  +
                              " AND Verse >=  " + QString::number(verseFirst) +
                              " AND Verse <= " + QString::number(verseLast);
        QSqlQuery query(std::get<0>(databases[dbIndex]));
        query.exec(queryString);
        QSqlQuery xRefQuery(dbXRef);
        currentPassage.clear();
        while (query.next()) {
            QString verseNumber = query.record().value(0).toString();



            QString xRefQueryString = "SELECT XRefs FROM CrossReferences"
                                      " WHERE BOOK = " + bookStr +
                                      " AND Chapter = " + chapterStr  +
                                      " AND Verse = " + verseNumber;
            xRefQuery.exec(xRefQueryString);
            if (xRefQuery.next())
                verseNumber = "<a href=\"x:" + verseNumber + ":" +
                        xRefQuery.record().value(0).toString() + "\">" + verseNumber + "</a>";
            QString currentVerse = "<b>" + verseNumber + "</b> " + query.record().value(1).toString() + "<br>";
            if (currentVerse.contains("<TS>")) {
                headerRegex.indexIn(currentVerse);
                currentVerse.remove(headerRegex);
                currentVerse = headerRegex.cap(0) + currentVerse;
            }
            currentPassage += currentVerse;
        }
        if (currentPassage.isNull()) {
            chapterBrowsers[dbIndex]->setHtml(unavailable);
            return;
        }
        currentPassage = formatText(currentPassage, std::get<3>(databases[dbIndex]));
        chapterBrowsers[dbIndex]->setHtml(currentPassage);
    }
}

void MainWindow::on_bookListWidget_currentRowChanged(int currentRow)
{
    if (languageChanging)
        return;
    int dbIndex = currentTranslationTab;
    QString queryString = "SELECT Chapter FROM Bible"
                          " WHERE Book = " + QString::number(currentRow + 1) +
                          " AND Verse = 1";
    ui->chapterListWidget->clear();
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    query.exec(queryString);
    while (query.next())
        ui->chapterListWidget->addItem(query.record().value(0).toString());
    if (ui->chapterListWidget->count() == 0) {
        chapterBrowsers[dbIndex]->setHtml(unavailable);
        return;
    }
    ui->chapterListWidget->setCurrentRow(0);
}

void MainWindow::on_chapterListWidget_currentRowChanged(int currentRow)
{
    ui->verseFirstComboBox->blockSignals(true);
    ui->verseLastComboBox->blockSignals(true);
    int bookNumber = ui->bookListWidget->currentRow() + 1;
    int chapterNumber = currentRow + 1;
    ui->verseFirstComboBox->clear();
    ui->verseLastComboBox->clear();
    int dbIndex = currentTranslationTab;
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    QString queryString = "SELECT Verse FROM Bible"
                          " WHERE Book = " + QString::number(bookNumber) +
                          " AND Chapter = " + QString::number(chapterNumber);
    query.exec(queryString);
    while (query.next()) {
        QString verseNumber = query.record().value(0).toString();
        ui->verseFirstComboBox->addItem(verseNumber);
        ui->verseLastComboBox->addItem(verseNumber);
    }
    ui->verseFirstComboBox->setCurrentIndex(0);
    ui->verseFirstComboBox->blockSignals(false);
    ui->verseLastComboBox->blockSignals(false);
    ui->verseLastComboBox->setCurrentIndex(ui->verseLastComboBox->count() - 1);
    ui->nextChapterButton->setDisabled(
                ui->bookListWidget->currentRow()
                == ui->bookListWidget->count() - 1
                && ui->chapterListWidget->currentRow()
                == ui->chapterListWidget->count() - 1);
    ui->prevChapterButton->setDisabled(
                ui->bookListWidget->currentRow() == 0
                && ui->chapterListWidget->currentRow() == 0);
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

QString cleanBeforeCopying(QString text, bool hasStrong)
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
    return text;
}

void MainWindow::on_copyButton_clicked()
{
    int dbIndex = currentTranslationTab;
    currentPassage = cleanBeforeCopying(currentPassage, std::get<3>(databases[dbIndex]));
    QString textToAppend;
    QString bookName = ui->bookListWidget->currentItem()->text();
    QString chapterNumber = ui->chapterListWidget->currentItem()->text();
    QString firstVerseNumber = ui->verseFirstComboBox->currentText();
    QString lastVerseNumber = ui->verseLastComboBox->currentText();
    int firstVerseIndex = ui->verseFirstComboBox->currentIndex();
    int lastVerseIndex = ui->verseLastComboBox->currentIndex();
    if (firstVerseIndex == 0 && (lastVerseIndex + 1) == ui->verseLastComboBox->count()) {
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
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(currentPassage);
    ui->verseLineEdit->setText(currentPassage);
    ui->statusBar->showMessage(copyMessage, 2000);
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
    loadPassage();
}


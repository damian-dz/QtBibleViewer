#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/StrongPopup.h"

/* Compare Tab */
void MainWindow::loadCompareTab()
{
    dbCnt = QSqlDatabase::addDatabase("QSQLITE", "Counters");
    dbCnt.setDatabaseName(executionPath + "/xref/counters.bblv");
    dbCnt.open();
    connect(ui->compareTextBrowser, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showBasicContextMenu(const QPoint &)));
    ui->compareBookListWidget->addItems(bookNames);
    if (!recentVerse.isEmpty()) {
        int book = recentVerse[0].toInt();
        int chapter = recentVerse[1].toInt();
        int verse = recentVerse[2].toInt();
        ui->compareBookListWidget->setCurrentRow(book - 1);
        ui->compareChapterListWidget->setCurrentRow(chapter - 1);
        ui->compareVerseListWidget->setCurrentRow(verse - 1);
    } else
        ui->compareBookListWidget->setCurrentRow(0);
    firstLoadCompare = false;
}

void MainWindow::on_compareBookListWidget_currentRowChanged(int currentRow)
{
    QString queryString = QStringLiteral("SELECT Chapter FROM Counters WHERE Book = ") %
                          QString::number(currentRow + 1);
    loadFirstChapter = loadWhenBookChanged;
    QSqlQuery query(dbCnt);
    query.exec(queryString);
    QStringList chapterNumbers;
    while (query.next())
        chapterNumbers << query.record().value(0).toString();
    ui->compareChapterListWidget->blockSignals(true);
    ui->compareChapterListWidget->clear();
    ui->compareChapterListWidget->addItems(chapterNumbers);
    ui->compareChapterListWidget->blockSignals(false);
    if (!firstLoadCompare)
        ui->compareChapterListWidget->setCurrentRow(0);
}

void MainWindow::on_compareChapterListWidget_currentRowChanged(int currentRow)
{
    int book = ui->compareBookListWidget->currentRow() + 1;
    QString queryString = QStringLiteral("SELECT VerseCount FROM Counters WHERE Book = ") % QString::number(book) %
                          QStringLiteral(" AND Chapter = ") % QString::number(currentRow + 1);
    QSqlQuery query(dbCnt);
    query.exec(queryString);
    int verseCount = 0;
    if (query.next())
        verseCount = query.record().value(0).toInt();
    QStringList verseNumbers;
    for (int i = 1; i <= verseCount; ++i)
        verseNumbers << QString::number(i);
    ui->compareVerseListWidget->blockSignals(true);
    ui->compareVerseListWidget->clear();
    ui->compareVerseListWidget->addItems(verseNumbers);
    ui->compareVerseListWidget->blockSignals(false);
    if (!firstLoadCompare)
        ui->compareVerseListWidget->setCurrentRow(0);
}

QString MainWindow::formatScripture(QString text, bool hasStrong)
{
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(QStringLiteral("<CM>"));
    QRegExp rgxNotesHeadings(QStringLiteral("<RF>[^<]*<Rf>|<TS>[^<]*<Ts>"));
    text.remove(rgxNotesHeadings);
    if (hasStrong) {
        QRegularExpression regex(QStringLiteral("<W[HG][0-9]{1,4}>"));
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() - 3);
                text.replace(original, QStringLiteral(" <a href='s:") %
                             modified % QStringLiteral("'>") % modified %
                             QStringLiteral("</a>"));
            }
        }
    }
    return text;
}

void MainWindow::on_compareVerseListWidget_currentRowChanged(int currentRow)
{
    QString book = QString::number(ui->compareBookListWidget->currentRow() + 1);
    QString chapter = QString::number(ui->compareChapterListWidget->currentRow() + 1);
    QString verse = QString::number(currentRow + 1);
    QString queryString = QStringLiteral("SELECT Scripture FROM Bible WHERE Book = ") % book %
                          QStringLiteral(" AND Chapter = ") % chapter %
                          QStringLiteral(" AND Verse = ") % verse;
    QString text = QStringLiteral("<table border='1' cellpadding='8' width='100%'>");
    for (int i = 0; i < modules.count(); ++i) {
        QSqlQuery query(modules[i].database);
        query.exec(queryString);
        if (query.next()) {
            text += QStringLiteral("<tr><td><b><a href='t:") % QString::number(i) % QStringLiteral("'>") +
                    modules[i].name % QStringLiteral(":</a></b>");
            QString scripture = query.record().value(0).toString().trimmed();
            text += QStringLiteral(" ") % formatScripture(scripture, modules[i].hasStrong);
            text += QStringLiteral("</td></tr>");
        }
    }
    text += QStringLiteral("</table>");
    ui->compareTextBrowser->setHtml(text);
    recentVerse.clear();
    recentVerse << book << chapter << verse;
    ui->comparePrevButton->setDisabled(ui->compareBookListWidget->currentRow() == 0 &&
                                       ui->compareChapterListWidget->currentRow() == 0 &&
                                       ui->compareVerseListWidget->currentRow() == 0);
    ui->compareNextButton->setDisabled(ui->compareBookListWidget->currentRow() ==
                                       ui->compareBookListWidget->count() - 1 &&
                                       ui->compareChapterListWidget->currentRow() ==
                                       ui->compareChapterListWidget->count() - 1  &&
                                       ui->compareVerseListWidget->currentRow() ==
                                       ui->compareVerseListWidget->count() - 1 );
}

void MainWindow::on_compareTextBrowser_anchorClicked(const QUrl &arg1)
{
    QStringList argSplit = arg1.toString().split(":");
    if (argSplit[0] == "t") {
        int book = ui->compareBookListWidget->currentRow() + 1;
        int chapter = ui->compareChapterListWidget->currentRow() + 1;
        int verse = ui->compareVerseListWidget->currentRow() + 1;
        ui->tabWidget->setCurrentIndex(0);
        int dbIndex = argSplit[1].toInt();
        ui->translationTabWidget->setCurrentIndex(dbIndex);
        QStringList indices;
        indices << QString::number(book)
                << QString::number(chapter)
                << QString::number(verse);
        highlightPassage(indices, dbIndex);
    } else if (argSplit[0] == "s") {
        QFont font;
        if (!fontFamily.isEmpty())
           font.setFamily(fontFamily);
        font.setPointSize(fontSize);
        StrongPopup strongDialog(dbDct, argSplit[1], font, this);
        strongDialog.exec();
    }
}

void MainWindow::on_comparePrevButton_clicked()
{
    if (ui->compareVerseListWidget->currentRow() > 0)
        ui->compareVerseListWidget->setCurrentRow(ui->compareVerseListWidget->currentRow() - 1);
    else if (ui->compareChapterListWidget->currentRow() > 0) {
        ui->compareChapterListWidget->setCurrentRow(ui->compareChapterListWidget->currentRow() - 1);
        ui->compareVerseListWidget->setCurrentRow(ui->compareVerseListWidget->count() - 1);
    } else {
        ui->compareBookListWidget->setCurrentRow(ui->compareBookListWidget->currentRow() - 1);
        ui->compareChapterListWidget->setCurrentRow(ui->compareChapterListWidget->count() - 1);
        ui->compareVerseListWidget->setCurrentRow(ui->compareVerseListWidget->count() - 1);
    }
}

void MainWindow::on_compareNextButton_clicked()
{
    if (ui->compareVerseListWidget->currentRow() < ui->compareVerseListWidget->count() - 1)
        ui->compareVerseListWidget->setCurrentRow(ui->compareVerseListWidget->currentRow() + 1);
    else if (ui->compareChapterListWidget->currentRow() < ui->compareChapterListWidget->count() - 1)
        ui->compareChapterListWidget->setCurrentRow(ui->compareChapterListWidget->currentRow() + 1);
    else
        ui->compareBookListWidget->setCurrentRow(ui->compareBookListWidget->currentRow() + 1);
}

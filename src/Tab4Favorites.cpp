#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/StrongPopup.h"

/* Favorites Tab */
void createFavDatabase(QSqlDatabase &db, const QString &fileName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);
    db.open();
    QSqlQuery query(db);
    query.exec("CREATE TABLE Favorites (Book INT, Chapter INT, VerseFirst INT, VerseLast INT, Comment TEXT)");
    query.exec("CREATE UNIQUE INDEX \"fav_key\" ON \"Favorites\" "
               "(\"Book\" ASC, \"Chapter\" ASC, \"VerseFirst\" ASC, \"VerseLast\" ASC)");
}

void MainWindow::loadFavoritesTab()
{
    QString dirName = executionPath + "/user";
    QString fileName = dirName + "/fav.bblv";
    if (!QDir(dirName).exists()) {
        QDir().mkdir(dirName);
        createFavDatabase(dbUsr, fileName);
    } else if (!QDir().exists(fileName)) {
        createFavDatabase(dbUsr, fileName);
    } else {
        dbUsr = QSqlDatabase::addDatabase("QSQLITE");
        dbUsr.setDatabaseName(fileName);
        dbUsr.open();
        QSqlQuery query(dbUsr);
        query.exec("SELECT * FROM Favorites");
        QStringList favList;
        while (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verseFirst = record.value(2).toInt();
            int verseLast = record.value(3).toInt();
            QString passageId = bookNames[book - 1] % " " % QString::number(chapter) %
                    ":" % QString::number(verseFirst);
            if (verseFirst != verseLast)
                passageId += "-" % QString::number(verseLast);
            favList << passageId;
            favoriteList.append({ (uchar)0, (uchar)book, (uchar)chapter, (uchar)verseFirst, (uchar)verseLast });
        }
        if (favList.count() > 0) {
            ui->favoritePassagesListWidget->addItems(favList);
            ui->favoritePassagesListWidget->setCurrentRow(0);
        }
    }
}

void MainWindow::insertIntoFavorites()
{
    QSqlQuery query(dbUsr);
    QString book = QString::number(ui->bookListWidget->currentRow() + 1);
    QString chapter = QString::number(ui->chapterListWidget->currentRow() + 1);
    QString verseFirst = QString::number(range.first);
    QString verseLast = QString::number(range.second);
    QString queryString = "SELECT Comment FROM Favorites WHERE Book = " % book %
                          " AND Chapter = " % chapter %
                          " AND VerseFirst = " % verseFirst %
                          " AND VerseLast = " % verseLast;
    query.exec(queryString);
    if (query.next()) {
        QMessageBox::critical(this, titleError, entryExists);
        int index = favoriteList.indexOf(TabBookChapterVerses({ 0,
                                                                (uchar)book.toInt(),
                                                                (uchar)chapter.toInt(),
                                                                range.first,
                                                                range.second }));
        ui->favoritePassagesListWidget->setCurrentRow(index);
        return;
    }
    queryString = "INSERT INTO Favorites (Book, Chapter, VerseFirst, VerseLast)"
                  "VALUES (" % book % ", " % chapter % ", " % verseFirst % ", " % verseLast % ")";
    query.exec(queryString);
    QString passageId = bookNames[ui->bookListWidget->currentRow()] % " " % chapter % ":" % verseFirst;
    if (range.first != range.second)
        passageId += "-" % verseLast;
    ui->favoritePassagesListWidget->addItem(passageId);
    favoriteList.append({ 0, (uchar)book.toInt(), (uchar)chapter.toInt(), range.first, range.second });
    ui->favoritePassagesListWidget->setCurrentRow(ui->favoritePassagesListWidget->count() - 1);
}

void MainWindow::on_deleteButton_clicked()
{
    int index = ui->favoritePassagesListWidget->currentRow();
    QString passageId = ui->favoritePassagesListWidget->currentItem()->text();
    QMessageBox questionMsgBox(QMessageBox::Question,
                               confirmDeletion,
                               areYouSure + passageId + "?",
                               QMessageBox::Yes | QMessageBox::No);
    questionMsgBox.setButtonText(QMessageBox::Yes, replyYes);
    questionMsgBox.setButtonText(QMessageBox::No, replyNo);
    if (questionMsgBox.exec() == QMessageBox::Yes) {
        QSqlQuery query(dbUsr);
        QString book = QString::number(favoriteList[index].book);
        QString chapter = QString::number(favoriteList[index].chapter);
        QString verseFirst = QString::number(favoriteList[index].verseFirst);
        QString verseLast = QString::number(favoriteList[index].verseLast);
        QString queryString = "DELETE FROM Favorites WHERE Book = " % book %
                             " AND Chapter = " % chapter %
                             " AND VerseFirst = " % verseFirst %
                             " AND VerseLast = " % verseLast;
        if (query.exec(queryString)) {
            favoriteList.removeAt(index);
            delete ui->favoritePassagesListWidget->currentItem();
        }
    }
}

void MainWindow::on_saveButton_clicked()
{
    int index = ui->favoritePassagesListWidget->currentRow();
    QString book = QString::number(favoriteList[index].book);
    QString chapter = QString::number(favoriteList[index].chapter);
    QString verseFirst = QString::number(favoriteList[index].verseFirst);
    QString verseLast = QString::number(favoriteList[index].verseLast);
    QString comment = ui->favoriteCommentTextEdit->toPlainText();
    QString queryString = "UPDATE Favorites SET Comment = '" % comment % "'" %
                          " WHERE Book = " % book %
                          " AND Chapter = " % chapter %
                          " AND VerseFirst = " % verseFirst %
                          " AND VerseLast = " % verseLast;
    QSqlQuery query(dbUsr);
    if (query.exec(queryString))
        ui->statusBar->showMessage(entryUpdated, 2500);
}

void MainWindow::on_favoritePassagesListWidget_currentRowChanged(int currentRow)
{
    QString book = QString::number(favoriteList[currentRow].book);
    QString chapter = QString::number(favoriteList[currentRow].chapter);
    QString verseFirst = QString::number(favoriteList[currentRow].verseFirst);
    QString verseLast = QString::number(favoriteList[currentRow].verseLast);
    QString queryString = "SELECT Comment FROM Favorites WHERE Book = " % book %
                         " AND Chapter = " % chapter %
                         " AND VerseFirst = " % verseFirst %
                         " AND VerseLast = " % verseLast;
    QSqlQuery query(dbUsr);
    if (!query.exec(queryString))
        return;
    if (query.next())
        ui->favoriteCommentTextEdit->setText(query.record().value(0).toString());
    query = QSqlQuery(translations[currentTranslationTab].database);
    QString passage;
    queryString = "SELECT Verse, Scripture FROM Bible WHERE Book = " % book %
                  " AND Chapter = " % chapter %
                  " AND Verse >= " % verseFirst %
                  " AND Verse <= " % verseLast;
    if (!query.exec(queryString))
        return;
    while (query.next()) {
        QSqlRecord record = query.record();
        QString verse = record.value(0).toString();
        QString scripture = record.value(1).toString();
        passage += " <b>" % verse % "</b>" % " " % scripture;
    }
    passage = formatScripture(passage, translations[currentTranslationTab].hasStrong).trimmed();
    ui->favoritePassageTextBrowser->setHtml(passage);
}

void MainWindow::on_favoritePassageTextBrowser_anchorClicked(const QUrl &arg1)
{
    QStringList argSplit = arg1.toString().split(":");
    QFont font;
    if (!fontFamily.isEmpty())
       font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    StrongPopup strongDialog(dbDct, argSplit[1], font, this);
    strongDialog.exec();
}

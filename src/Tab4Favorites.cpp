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
            favorites.append({ 0, (uchar)book, (uchar)chapter, (uchar)verseFirst, (uchar)verseLast });
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
        QMessageBox::critical(this, tr("Error"), tr("An entry for this passage already exists."));
        int index = favorites.indexOf(TabBookChapterVerses({ 0,
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
    favorites.append({ 0, (uchar)book.toInt(), (uchar)chapter.toInt(), range.first, range.second });
    ui->favoritePassagesListWidget->setCurrentRow(ui->favoritePassagesListWidget->count() - 1);
}

void MainWindow::on_deleteButton_clicked()
{
    int index = ui->favoritePassagesListWidget->currentRow();
    QString passageId = ui->favoritePassagesListWidget->currentItem()->text();
    QMessageBox questionMsgBox(QMessageBox::Question,
                               tr("Confirm Deletion"),
                               tr("Delete entry ") + passageId + "?",
                               QMessageBox::Yes | QMessageBox::No);
    questionMsgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    questionMsgBox.setButtonText(QMessageBox::No, tr("No"));
    if (questionMsgBox.exec() == QMessageBox::Yes) {
        QSqlQuery query(dbUsr);
        QString book = QString::number(favorites[index].book);
        QString chapter = QString::number(favorites[index].chapter);
        QString verseFirst = QString::number(favorites[index].verseFirst);
        QString verseLast = QString::number(favorites[index].verseLast);
        QString queryString = "DELETE FROM Favorites WHERE Book = " % book %
                             " AND Chapter = " % chapter %
                             " AND VerseFirst = " % verseFirst %
                             " AND VerseLast = " % verseLast;
        if (query.exec(queryString)) {
            favorites.removeAt(index);
            delete ui->favoritePassagesListWidget->currentItem();
        }
    }
}

void MainWindow::on_saveButton_clicked()
{
    int index = ui->favoritePassagesListWidget->currentRow();
    QString book = QString::number(favorites[index].book);
    QString chapter = QString::number(favorites[index].chapter);
    QString verseFirst = QString::number(favorites[index].verseFirst);
    QString verseLast = QString::number(favorites[index].verseLast);
    QString comment = ui->favoriteCommentTextEdit->toPlainText();
    QString queryString = "UPDATE Favorites SET Comment = '" % comment % "'" %
                          " WHERE Book = " % book %
                          " AND Chapter = " % chapter %
                          " AND VerseFirst = " % verseFirst %
                          " AND VerseLast = " % verseLast;
    QSqlQuery query(dbUsr);
    if (query.exec(queryString))
        ui->statusBar->showMessage(tr("Entry updated."), 2500);
}

void MainWindow::on_favoritePassagesListWidget_currentRowChanged(int currentRow)
{
    QString book = QString::number(favorites[currentRow].book);
    QString chapter = QString::number(favorites[currentRow].chapter);
    QString verseFirst = QString::number(favorites[currentRow].verseFirst);
    QString verseLast = QString::number(favorites[currentRow].verseLast);
    QString queryString = "SELECT Comment FROM Favorites WHERE Book = " % book %
                         " AND Chapter = " % chapter %
                         " AND VerseFirst = " % verseFirst %
                         " AND VerseLast = " % verseLast;
    QSqlQuery query(dbUsr);
    if (!query.exec(queryString))
        return;
    if (query.next())
        ui->favoriteCommentTextEdit->setText(query.record().value(0).toString());
    int index = currentTranslationTab;
    query = QSqlQuery(modules[index].database);
    queryString = "SELECT Verse, Scripture FROM Bible WHERE Book = " % book %
                  " AND Chapter = " % chapter %
                  " AND Verse >= " % verseFirst %
                  " AND Verse <= " % verseLast;
    if (!query.exec(queryString))
        return;
    QString passage;
    while (query.next()) {
        QSqlRecord record = query.record();
        QString verse = record.value(0).toString();
        QString scripture = record.value(1).toString();
        passage += " <b>" % verse % "</b>" % " " % scripture;
    }
    if (passage.isNull()) {
        ui->favoritePassageTextBrowser->setHtml("<center><h2>" % tr("Unavailable in this module.") % "</center></h2>");
        return;
    }
    passage = formatScripture(passage, modules[index].hasStrong).trimmed();
    passage += "<b>—" % bookNames[favorites[currentRow].book - 1] % " " %
            chapter % ":" % verseFirst;
    if (verseFirst != verseLast)
        passage += "-" % verseLast % "</b>";
    else
        passage += "</b>";
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

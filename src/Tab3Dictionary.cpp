#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

/* Dictionary Tab */
void MainWindow::on_openDictionaryButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr(openCaption.toUtf8().constData()), "/", openDctFilter);
    if (filename.isNull())
        return;
    if (dbDct.isOpen()) {
        QString connection;
        connection = dbDct.connectionName();
        dbDct.close();
        dbDct = QSqlDatabase();
        QSqlDatabase::removeDatabase(connection);
    }
    dbDct = QSqlDatabase::addDatabase("QSQLITE", "DictionaryConnection");
    dbDct.setDatabaseName(filename);
    dbDct.open();
    QSqlQuery query(dbDct);
    QString queryString = "SELECT data FROM dictionary WHERE relativeorder = 1";
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        ui->definitionTextBrowser->setHtml(record.value(0).toString());
    }
    QStringList wordList;
    queryString = "SELECT word FROM dictionary";
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        wordList << record.value(0).toString();
    }
}

void MainWindow::on_searchDictionaryButton_clicked()
{
    QString entry = ui->searchDictionaryLineEdit->text();
    QSqlQuery query(dbDct);
    QString queryString = "SELECT data FROM dictionary WHERE word = '" +  entry + "'";
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        ui->definitionTextBrowser->setHtml(record.value(0).toString());
    }
}

void MainWindow::on_searchDictionaryLineEdit_textChanged(const QString &arg1)
{

}

void MainWindow::on_definitionTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString entry = arg1.toString();
    entry = entry.right(entry.length() - 2);
    QSqlQuery query(dbDct);
    QString queryString = "SELECT data FROM dictionary WHERE word = '" +  entry + "'";
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        ui->definitionTextBrowser->setHtml(record.value(0).toString());
    }
}

void MainWindow::on_searchDictionaryLineEdit_returnPressed()
{
    on_searchDictionaryButton_clicked();
}


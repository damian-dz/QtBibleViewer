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

void MainWindow::on_searchDictionaryLineEdit_textEdited(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui->entriesListWidget->setCurrentRow(-1);
        return;
    }
    QList<QListWidgetItem *> matchList;
    matchList = ui->entriesListWidget->findItems(arg1, Qt::MatchContains);
    if (matchList.count() > 0)
        ui->entriesListWidget->setCurrentItem(matchList[0]);
}


void MainWindow::on_definitionTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QString id = argString.left(2);
    if (id == "#d" || id == "#s") {
        QString entry = argString.right(argString.length() - 2);
        QSqlQuery query(dbDct);
        QString queryString = "SELECT relativeorder, data "
                              "FROM dictionary "
                              "WHERE word = '" +  entry + "'";
        query.exec(queryString);
        if (query.next()) {
            QSqlRecord record = query.record();
            ui->definitionTextBrowser->setHtml(record.value(1).toString());
            ui->entriesListWidget->setCurrentRow(record.value(0).toInt() - 1);
        }
    } else if (id == "#b") {
        int dbIndex = currentTranslationTab;
        ui->tabWidget->setCurrentIndex(0);
        QString entry = argString.right(argString.length() - 2);
        qDebug () << entry;
        QStringList indices = entry.split(".");
        highlightPassage(indices, dbIndex);

    } else {
        ui->tabWidget->setCurrentIndex(2);
        ui->translationComboBox->setCurrentIndex(indexStrong);
        ui->byStrongsNumberRadioButton->setChecked(true);
        QString entry = argString.mid(1, argString.length() - 1);
        ui->enterLineEdit->setText(entry);
        on_searchButton_clicked();
    }
}

void MainWindow::on_entriesListWidget_currentTextChanged(const QString &currentText)
{
    QSqlQuery query(dbDct);
    QString queryString = "SELECT data FROM dictionary WHERE word = '" +  currentText + "'";
    QString definition = "<center><h2><a class='dict' href='S" + currentText +
            "' style='text-decoration:none'>" + currentText + "</a></h2></center>";
    query.exec(queryString);
    if (query.next())
        definition += query.record().value(0).toString();
    ui->definitionTextBrowser->setHtml(definition);
}


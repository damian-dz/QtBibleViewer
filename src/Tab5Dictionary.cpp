#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

/* Dictionary Tab */
void MainWindow::on_openDictionaryButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open MYBIBLE Module"),
                                                    "/",
                                                    tr("MYBIBLE Modules (*.dct.mybible);;All Files (*.*)"));
    if (filename.isNull() || filename.isEmpty())
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

QSqlQuery fillQuery;
bool fillFlag = false;
QTimer *fillTimer;

void MainWindow::fillDictionaryEntriesWidget()
{
    if (!fillFlag) {
        ui->dictMainHorizontalLayout->setStretchFactor(ui->entriesColumnVerticalLayout, 2);
        ui->dictMainHorizontalLayout->setStretchFactor(ui->definitionColumnVerticalLayout, 8);
        fillQuery = QSqlQuery(dbDct);
        QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
        QValidator *validator = new QRegExpValidator(regex, this);
        ui->searchDictionaryLineEdit->setValidator(validator);
        QString queryString = "SELECT word FROM dictionary WHERE relativeorder > 0";
        fillQuery.exec(queryString);
        fillTimer = new QTimer(this);
        connect(fillTimer, SIGNAL(timeout()), this, SLOT(fillDictionaryEntriesWidget()));
        fillTimer->start(20);
        fillFlag = true;
    }
    QStringList dictEntryList;
    int counter = 0;
    while (fillQuery.next() && counter < 2500) {
        dictEntryList << fillQuery.record().value(0).toString();
        counter++;
    }
    ui->entriesListWidget->addItems(dictEntryList);
    if (fillQuery.next()) {
        fillQuery.previous();
        fillQuery.previous();
    } else {
        disconnect(fillTimer, SIGNAL(timeout()), this, SLOT(fillDictionaryEntriesWidget()));
        fillTimer->stop();
        delete fillTimer;
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


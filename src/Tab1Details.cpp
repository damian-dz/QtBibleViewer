#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

/* Details Tab */
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

void MainWindow::fillDetails()
{
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
    int index = currentTranslationTab;
    QSqlQuery query(modules[index].database);
    query.exec(queryString);
    if (query.next()) {
        QSqlRecord record = query.record();
        ui->descriptionTextBrowser->setHtml(record.value(0).toString());
        ui->abbreviationLineEdit->setText(record.value(1).toString());
        ui->commentsTextBrowser->setHtml(formatPolish(record.value(2).toString()));
        ui->versionLineEdit->setText(record.value(3).toString());
        ui->versionDateLineEdit->setText(record.value(4).toString());
        ui->publishDateLineEdit->setText(record.value(5).toString());
        ui->rightToLeftCheckBox->setChecked(record.value(6).toBool());
        ui->oldTestamentCheckBox->setChecked(record.value(7).toBool());
        ui->newTestamentCheckBox->setChecked(record.value(8).toBool());
        ui->strongsNumbersCheckBox->setChecked(record.value(9).toBool());
    }
}

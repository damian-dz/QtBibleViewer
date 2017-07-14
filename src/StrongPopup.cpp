#include "hdr/StrongPopup.h"
#include "ui_StrongPopup.h"

StrongPopup::StrongPopup(const QSqlDatabase &db,
                         const QString &number,
                         const QFont &font,
                         QWidget* parent)
    : QDialog(parent),
      ui(new Ui::StrongPopup)
{
    ui->setupUi(this);
    ui->definitionTextBrowser->setFont(font);
    QDialog::setWindowTitle(number);
    dbDct = &db;
    loadDefinition(number);
}

StrongPopup::~StrongPopup()
{
    delete ui;
}

void StrongPopup::on_definitionTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QString number = argString.mid(2, argString.size() - 1);
    QDialog::setWindowTitle(number);
    loadDefinition(number);
}

void StrongPopup::loadDefinition(const QString &number)
{
    QSqlQuery query(*dbDct);
    QString queryString =  "SELECT data FROM dictionary WHERE word = '" + number + "'";
    query.exec(queryString);
    if (query.next())
        ui->definitionTextBrowser->setHtml(query.record().value(0).toString());
}

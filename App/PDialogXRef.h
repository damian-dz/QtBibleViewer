#ifndef PDIALOGXREF_H
#define PDIALOGXREF_H

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

class PDialogXRef : public QDialog
{
    Q_OBJECT

public:
    PDialogXRef(const QSqlDatabase &dbBib,
                const QStringList &verseInfo,
                const QStringList &bookNames,
                const QPixmap &background,
                const QFont &font,
                QWidget *parent = 0);
    ~PDialogXRef();

private:
    QTextBrowser *ui_TextBrowser_Main;

    void setBrowserBackground(const QPixmap &background);
    void generateMainLayout(const QFont &font);
    void loadPassages(const QSqlDatabase &db, const QString &passageStr, const QStringList &bookNames);
};

#endif // PDIALOGXREF_H

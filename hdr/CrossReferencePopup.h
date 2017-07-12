#ifndef CROSSREFERENCEPOPUP_H
#define CROSSREFERENCEPOPUP_H

#include <QDialog>
#include <QtSql>
#include <QPair>
#include <QString>

namespace Ui {
class CrossReferencePopup;
}

class CrossReferencePopup : public QDialog
{
    Q_OBJECT

public:
    explicit CrossReferencePopup(const QPair<QSqlDatabase, QSqlDatabase> &dbs,
                             const QStringList &hrefBookChapter,
                             const QStringList &books,
                             const QFont &font,
                             QWidget* parent = 0);
    ~CrossReferencePopup();

private slots:
    void on_textBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::CrossReferencePopup *ui;
    QFont font;
    QSqlDatabase dbDct;
    void loadPassages(const QSqlDatabase &db, QString passageString, QStringList bookNames);
};

#endif // CROSSREFERENCEPOPUP_H

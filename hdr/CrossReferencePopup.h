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
    explicit CrossReferencePopup(QSqlDatabase db,
                             QStringList hrefBookChapter,
                             QStringList books,
                             QFont font,
                             QWidget* parent = 0);
    ~CrossReferencePopup();

private:
    Ui::CrossReferencePopup *ui;
    QSqlDatabase dbBbl;
    void loadPassages(QSqlDatabase db, QString passageString, QStringList bookNames);
};

#endif // CROSSREFERENCEPOPUP_H

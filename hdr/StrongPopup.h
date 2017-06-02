#ifndef STRONGPOPUP_H
#define STRONGPOPUP_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class StrongPopup;
}

class StrongPopup : public QDialog
{
    Q_OBJECT

public:
    explicit StrongPopup(QSqlDatabase db, QString number, const QFont &font, QWidget *parent = 0);
    ~StrongPopup();

private slots:
    void on_definitionTextBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::StrongPopup *ui;
    QSqlDatabase dbDct;
    QSqlDatabase dbXRef;
    void loadDefinition(QString number);
    void loadCrossReferences(QString numbers);
};

#endif // STRONGPOPUP_H

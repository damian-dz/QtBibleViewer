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
    explicit StrongPopup(const QSqlDatabase &db,
                         const QString &number,
                         const QFont &font,
                         QWidget *parent = 0);
    ~StrongPopup();

private slots:
    void on_definitionTextBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::StrongPopup *ui;
    QSqlDatabase dbDct;
    QSqlDatabase dbXRef;
    void loadCrossReferences(const QString &numbers);
    void loadDefinition(const QString &number);
};

#endif // STRONGPOPUP_H

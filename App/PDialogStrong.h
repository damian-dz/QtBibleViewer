#ifndef PDIALOGSTRONG_H
#define PDIALOGSTRONG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextBrowser>
#include <QVBoxLayout>

class PDialogStrong : public QDialog
{
    Q_OBJECT

public:
    PDialogStrong(const QSqlDatabase &db,
                  const QString &number,
                  const QFont &font,
                  const QPixmap &background,
                  bool useBckgrnd,
                  QWidget *parent = 0);
    ~PDialogStrong();

private slots:
     void on_definitionTextBrowser_anchorClicked(const QUrl &arg1) ;

private:
    const QSqlDatabase *dbDct;
    QTextBrowser *m_defTextBrowser;

    void loadDefinition(const QString &number);
    void setBrowserBackground(const QPixmap &background);
};

#endif // PDIALOGSTRONG_H

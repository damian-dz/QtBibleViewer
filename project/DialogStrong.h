#ifndef PDIALOGSTRONG_H
#define PDIALOGSTRONG_H

#include "precomp.h"

class DialogStrong : public QDialog
{
    Q_OBJECT
public:
    DialogStrong(const QSqlDatabase &db,
                 const QString &number,
                 const QFont &font,
                 const QPixmap &background,
                 bool useBckgrnd,
                 QWidget *parent = nullptr);
    ~DialogStrong();

private slots:
     void on_definitionTextBrowser_anchorClicked(const QUrl &arg1) ;

private:
    const QSqlDatabase *m_dbDct;
    QTextBrowser *m_defTextBrowser;

    void loadDefinition(const QString &number);
    void setBrowserBackground(const QPixmap &background);
};

#endif // PDIALOGSTRONG_H

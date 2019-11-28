#ifndef DIALOGINFO_H
#define DIALOGINFO_H

#include "precomp.h"

class DialogInfo : public QDialog
{
    Q_OBJECT
public:
    DialogInfo(const QSqlDatabase &db, QFont font, const QPixmap &background, bool useBckgrnd,
               QWidget *parent = nullptr);
    ~DialogInfo();

private:
    void setBrowserBackground(QTextBrowser &browser, const QPixmap &background);

};

#endif // DIALOGINFO_H

#ifndef FINDONPAGEBOX_H
#define FINDONPAGEBOX_H

#include "precomp.h"

class FindOnPageBox : public QWidget
{
    Q_OBJECT
public:
    FindOnPageBox(QWidget *parent = nullptr);

    void SelectAllText() const;

private:
    QLabel *ui_Label_Find;
    QLineEdit *ui_LineEdit_Find;
    QPushButton *ui_Button_Close;

signals:
    void TextChanged(const QString &text);
};

#endif // FINDONPAGEBOX_H

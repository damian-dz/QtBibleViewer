#include "FindOnPageBox.h"

FindOnPageBox::FindOnPageBox(QWidget *parent) :
    QWidget(parent),
    ui_Label_Find(new QLabel(tr("Find:"))),
    ui_LineEdit_Find(new QLineEdit),
    ui_Button_Close(new QPushButton)
{
    ui_LineEdit_Find->setClearButtonEnabled(true);

    ui_Button_Close->setIcon(QIcon(":/img/img_res/close.svg"));
    ui_Button_Close->setMaximumSize(QSize(22, 22));

    QHBoxLayout *findHorLayout = new QHBoxLayout;
    findHorLayout->setContentsMargins(0, 0, 0, 0);
    findHorLayout->addWidget(ui_Label_Find);
    findHorLayout->addWidget(ui_LineEdit_Find);
    findHorLayout->addWidget(ui_Button_Close);

    QObject::connect(ui_LineEdit_Find, QOverload<const QString &>::of(&QLineEdit::textChanged),
                     this, [=] (const QString &text) { emit TextChanged(text); });
    QObject::connect(ui_Button_Close, QOverload<bool>::of(&QPushButton::clicked), this, [=] { QWidget::hide(); });

    QWidget::setLayout(findHorLayout);
    QWidget::hide();
}

void FindOnPageBox::SelectAllText() const
{
    ui_LineEdit_Find->setFocus();
    ui_LineEdit_Find->selectAll();
}

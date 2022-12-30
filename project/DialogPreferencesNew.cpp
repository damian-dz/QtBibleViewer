#include "DialogPreferencesNew.h"

DialogPreferencesNew::DialogPreferencesNew(AppConfig *config, QWidget *parent) :
    QDialog(parent),
    m_pConfig(config)
{
    QListWidget *listWidget = new QListWidget;
    listWidget->setFont(QFont(qApp->font().family(), 10));
    listWidget->setMaximumWidth(180);
    listWidget->addItem(tr("General"));
    listWidget->addItem(tr("Fonts"));
    listWidget->addItem(tr("Appearance"));
    listWidget->addItem(tr("Formatting"));
    listWidget->setCurrentRow(0);

    ui_stackedWidget = new QStackedWidget;

    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addWidget(listWidget);
    mainHorLayout->addWidget(ui_stackedWidget);

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *mainVerLayout = new QVBoxLayout(this);
    mainVerLayout->addLayout(mainHorLayout);
    mainVerLayout->addWidget(dialogButtonBox);

    QWidget::resize(480, 320);
    QWidget::setLayout(mainVerLayout);
    QWidget::setWindowTitle(tr("Preferences"));
}

DialogPreferencesNew::~DialogPreferencesNew()
{

}

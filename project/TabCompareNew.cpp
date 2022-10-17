#include "TabCompareNew.h"

TabCompareNew::TabCompareNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(config, databaseService, parent)
{

}

void TabCompareNew::SetUiTexts()
{

}

void TabCompareNew::ConnectSignals()
{

}

void TabCompareNew::AddControls()
{
    ui_NavPanel = new NavPanel(*m_pConfig, *m_pDatabaseService);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addLayout(ui_NavPanel);

    QWidget::setLayout(mainLayout);
}

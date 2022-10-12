#include "TabSearchNew.h"

TabSearchNew::TabSearchNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService)
{
    ui_SearchResultsArea = new SearchResultArea;

    ui_SearchOptionsPanel = new SearchOptionsPanelNew(config, databaseService);
    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addLayout(ui_SearchResultsArea);
    mainHorLayout->addWidget(ui_SearchOptionsPanel);
    QWidget::setLayout(mainHorLayout);
    ui_SearchResultsArea->setNumResultsPerPage(25);
}

void TabSearchNew::SetUiTexts()
{
    ui_SearchResultsArea->SetUiTexts();
    ui_SearchOptionsPanel->SetUiTexts();
}

void TabSearchNew::AddControls()
{

}

void TabSearchNew::ConnectSignals()
{

}

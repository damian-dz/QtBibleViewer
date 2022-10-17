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
    QObject::connect(ui_NavPanel, QOverload<qbv::Location>::of(&NavPanel::LocationChanged),
                     [=] (qbv::Location loc) { OnLocationChanged(loc); });
}

void TabCompareNew::AddControls()
{
    ui_NavPanel = new NavPanel(*m_pConfig, *m_pDatabaseService);
    ui_NavPanel->HideVerseTo();

    ui_CompareVerseBrowser = new CompareVerseBrowser;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addLayout(ui_NavPanel);
    mainLayout->addWidget(ui_CompareVerseBrowser);

    QWidget::setLayout(mainLayout);
}

void TabCompareNew::OnLocationChanged(qbv::Location loc)
{
    QStringList scriptures = m_pDatabaseService->GetScriptures(loc);
    QStringList shortNames = m_pDatabaseService->BibleShortNames();
    ui_CompareVerseBrowser->SetScriptures(scriptures, shortNames);

}

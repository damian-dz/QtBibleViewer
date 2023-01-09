#include "TabCompareNew.h"

TabCompareNew::TabCompareNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(config, databaseService, parent)
{

}

void TabCompareNew::SetUiTexts()
{
    ui_NavPanel->SetUiTexts();
}

void TabCompareNew::SetFontFromConfig()
{
    QFont font(m_pConfig->fonts.family, m_pConfig->fonts.size);
    ui_CompareVerseBrowser->setFont(font);
}

void TabCompareNew::ConnectSignals()
{
    QObject::connect(ui_NavPanel, QOverload<qbv::Location>::of(&NavPanel::LocationChanged),
                     this, [=] (qbv::Location loc) { OnLocationChanged(loc); });
    QObject::connect(ui_CompareVerseBrowser, QOverload<QString>::of(&CompareVerseBrowser::BibleNameClicked),
                     this, [=] (QString name) { OnBibleNameClicked(name); });
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

void TabCompareNew::OnBibleNameClicked(const QString &name)
{
    qbv::Location loc = ui_NavPanel->GetLocation();
    emit BibleNameClicked(name, loc);
}

void TabCompareNew::ReloadBookNames()
{
    ui_NavPanel->ReloadBookNames();
}

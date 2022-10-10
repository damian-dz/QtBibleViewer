#include "TabBibleNew.h"

TabBibleNew::TabBibleNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService)
{


}

void TabBibleNew::ConnectSignals()
{
    QObject::connect(ui_NavPanel, QOverload<qbv::Location>::of(&NavPanel::LocationChanged),
                     [=] ( qbv::Location loc ) { OnLocationChanged(loc); } );
    QObject::connect(ui_TabWidget_Bibles, QOverload<int>::of(&QTabWidget::currentChanged),
                     [=] (int idx) { OnTabChanged(idx); } );
    QObject::connect(ui_TabWidget_Bibles->tabBar(), QOverload<int, int>::of(&QTabBar::tabMoved),
                     [=] (int from, int to) { OnTabMoved(from, to); } );
}

void TabBibleNew::AddControls()
{
    ui_NavPanel = new NavPanel(*m_pConfig, *m_pDatabaseService);
    ui_TabWidget_Bibles = new QTabWidget;

    ui_TabWidget_Bibles->setMovable(true);
    ui_TabWidget_Bibles->setTabsClosable(true);
    ui_TabWidget_Bibles->setStyleSheet("QTabBar::close-button { "
                              "image: url(:/img/img_res/close-button.svg);"
                              "margin: 2px;"
                              "subcontrol-position: right; }"
                              "QTabBar::close-button:hover {"
                              "image: url(:/img/img_res/close-button-active.svg); }");

    for (int i = 0; i <  m_pDatabaseService->NumBibles(); i++) {
        PassageBrowserNew *passageBrowser = new PassageBrowserNew(*m_pConfig, *m_pDatabaseService);
        m_passageBrowsers.append(passageBrowser);
        m_locations << qbv::Location();
        ui_TabWidget_Bibles->addTab(passageBrowser, m_pDatabaseService->ShortName(i));
    }

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addLayout(ui_NavPanel);

    mainLayout->addWidget(ui_TabWidget_Bibles);
    QWidget::setLayout(mainLayout);
}

void TabBibleNew::SetUiTexts()
{

}

void TabBibleNew::OnLocationChanged(qbv::Location loc)
{
    int idx = ui_TabWidget_Bibles->currentIndex();
    auto passageWithNotes = m_pDatabaseService->PassageWithNotesAndMissingVerses(idx, loc);
    m_passageBrowsers[idx]->SetAndLoadPassageWithNotes(passageWithNotes);
    m_passageBrowsers[idx]->SetLocation(loc);
}

void TabBibleNew::OnTabChanged(int idx)
{
    auto loc = ui_NavPanel->Location();
    if (!m_passageBrowsers[idx]->HasText() || m_passageBrowsers[idx]->Location() != loc) {
        auto passageWithNotes = m_pDatabaseService->PassageWithNotesAndMissingVerses(idx, loc);
        m_passageBrowsers[idx]->SetAndLoadPassageWithNotes(passageWithNotes);
        m_passageBrowsers[idx]->SetLocation(loc);
    }
}

void TabBibleNew::OnTabMoved(int from, int to)
{
    m_pDatabaseService->SwapDbBibles(from, to);
    m_pDatabaseService->SetActiveIdx(to);
    m_pConfig->module_data.paths.swapItemsAt(from, to);
    m_passageBrowsers.swapItemsAt(from, to);
}

void TabBibleNew::SetLocationFromConfig()
{
    if (m_pConfig->module_data.last_passage.count() == 4) {
        qbv::Location loc(m_pConfig->module_data.last_passage[0].toInt(),
                          m_pConfig->module_data.last_passage[1].toInt(),
                          m_pConfig->module_data.last_passage[2].toInt(),
                          m_pConfig->module_data.last_passage[3].toInt());
        ui_NavPanel->SetLocation(loc, true);
    }
}

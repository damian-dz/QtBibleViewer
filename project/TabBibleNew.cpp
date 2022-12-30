#include "TabBibleNew.h"

TabBibleNew::TabBibleNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(config, databaseService, parent)
{

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
        AddNewPassageBrowser(i);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addLayout(ui_NavPanel);
    mainLayout->addWidget(ui_TabWidget_Bibles);
    QWidget::setLayout(mainLayout);
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

void TabBibleNew::SetUiTexts()
{

}

void TabBibleNew::AddNewPassageBrowser(int idx)
{
    PassageBrowserNew *passageBrowser = new PassageBrowserNew(*m_pConfig, *m_pDatabaseService);
    m_passageBrowsers.append(passageBrowser);
    QObject::connect(passageBrowser, QOverload<qbv::Location>::of(&PassageBrowserNew::AddNoteRequested),
                     [=] (qbv::Location loc) { emit AddNoteRequested(loc); });
    ui_TabWidget_Bibles->addTab(passageBrowser, m_pDatabaseService->BibleShortName(idx));
}

void TabBibleNew::OnLocationChanged(qbv::Location loc)
{
    int idx = ui_TabWidget_Bibles->currentIndex();
    UpdatePassageBrowser(idx, loc);
}

void TabBibleNew::OnTabChanged(int idx)
{
    auto loc = ui_NavPanel->GetLocation();
    if (!m_passageBrowsers[idx]->HasText() || m_passageBrowsers[idx]->Location() != loc) {
        UpdatePassageBrowser(idx, loc);
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
    QStringList slLoc = m_pConfig->module_data.last_passage;
    if (slLoc.count() == 4) {
        qbv::Location loc(slLoc[0].toInt(),
                          slLoc[1].toInt(),
                          slLoc[2].toInt(),
                          slLoc[3].toInt());
        ui_NavPanel->SetLocation(loc, true);
    }
}

void TabBibleNew::SaveLocationToConfig()
{
    qbv::Location loc = ui_NavPanel->GetLocation();
    QStringList slLoc;
    slLoc << QString::number(loc.book)
          << QString::number(loc.chapter)
          << QString::number(loc.verse1)
          << QString::number(loc.verse2);
    m_pConfig->module_data.last_passage = slLoc;
}

void TabBibleNew::SetTabIndexFromConfig()
{
    int idx = m_pConfig->module_data.index;
    if (idx < ui_TabWidget_Bibles->count()) {
        ui_TabWidget_Bibles->blockSignals(true);
        ui_TabWidget_Bibles->setCurrentIndex(idx);
        ui_TabWidget_Bibles->blockSignals(false);
    }
}

void TabBibleNew::SaveTabIndexToConfig()
{
    m_pConfig->module_data.index = ui_TabWidget_Bibles->currentIndex();
}

void TabBibleNew::SetFontFromConfig()
{
    QFont font(m_pConfig->fonts.family, m_pConfig->fonts.size);
    for (auto browser : m_passageBrowsers) {
        browser->setFont(font);
    }
}

void TabBibleNew::SetLocation(qbv::Location loc, bool emitSignal)
{
    ui_NavPanel->SetLocation(loc, emitSignal);
}

void TabBibleNew::SetBibleIndex(int idx)
{
    ui_TabWidget_Bibles->blockSignals(true);
    ui_TabWidget_Bibles->setCurrentIndex(idx);
    ui_TabWidget_Bibles->blockSignals(false);
}

void TabBibleNew::UpdatePassageBrowser(int idx, qbv::Location loc)
{
    bool hasStrong = m_pDatabaseService->HasStrong(idx);
    QStringList scriptures = m_pDatabaseService->GetScripturesWithMissing(idx, loc);
    m_passageBrowsers[idx]->SetLocation(loc);
    m_passageBrowsers[idx]->SetScriptures(scriptures, hasStrong);
}

void TabBibleNew::HighlightBlock(int browserIdx, int blockIdx)
{
    m_passageBrowsers[browserIdx]->HiglightBlock(blockIdx);
}

void TabBibleNew::ReloadBookNames()
{
    ui_NavPanel->ReloadBookNames();
}

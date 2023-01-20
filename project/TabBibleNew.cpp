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

    ui_XRefBrowser = new SearchResultsBrowser(*m_pConfig, *m_pDatabaseService);
    ui_FindOnPageBox = new FindOnPageBox;





    ui_Splitter_ModuleCrossRef = new QSplitter(Qt::Vertical);
    ui_Splitter_ModuleCrossRef->addWidget(ui_TabWidget_Bibles);


    ui_TabWidget_Tools = new QTabWidget;
    auto ui_StrongsBrowser = new QTextBrowser;
    ui_TabWidget_Tools->addTab(ui_XRefBrowser, QString());
    ui_TabWidget_Tools->addTab(ui_StrongsBrowser, QString());


    ui_Splitter_ModuleCrossRef->addWidget(ui_TabWidget_Tools);
    ui_Splitter_ModuleCrossRef->restoreState(m_pConfig->general.splitter_layout);


    ui_VerLayout_Modules = new QVBoxLayout;
   // ui_VerLayout_Modules->addWidget(ui_CrossRefBox);

    ui_VerLayout_Modules->addWidget(ui_Splitter_ModuleCrossRef);
    ui_VerLayout_Modules->addWidget(ui_FindOnPageBox);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addLayout(ui_NavPanel);
    mainLayout->addLayout(ui_VerLayout_Modules);
    QWidget::setLayout(mainLayout);
}

void TabBibleNew::ConnectSignals()
{
    QObject::connect(ui_NavPanel, QOverload<qbv::Location>::of(&NavPanel::LocationChanged),
        this, [=] ( qbv::Location loc ) { OnLocationChanged(loc); } );
    QObject::connect(ui_TabWidget_Bibles, QOverload<int>::of(&QTabWidget::currentChanged),
        this, [=] (int idx) { OnTabChanged(idx); } );
    QObject::connect(ui_TabWidget_Bibles->tabBar(), QOverload<int, int>::of(&QTabBar::tabMoved),
        this, [=] (int from, int to) { OnTabMoved(from, to); } );
    QObject::connect(ui_FindOnPageBox, QOverload<const QString &>::of(&FindOnPageBox::TextChanged),
        this, [=] (const QString &text) { m_passageBrowsers[ui_TabWidget_Bibles->currentIndex()]->HighlightText(text); });
}

void TabBibleNew::SetUiTexts()
{
    ui_TabWidget_Tools->setTabText(0, tr("Cross-references"));
    ui_TabWidget_Tools->setTabText(1, tr("Strong's"));
    ui_NavPanel->SetUiTexts();
}

void TabBibleNew::AddNewPassageBrowser(int idx)
{
    PassageBrowserNew *passageBrowser = new PassageBrowserNew(*m_pConfig, *m_pDatabaseService);
    m_passageBrowsers.append(passageBrowser);
    QObject::connect(passageBrowser, QOverload<qbv::Location>::of(&PassageBrowserNew::AddNoteRequested),
        this, [=] (qbv::Location loc) { emit AddNoteRequested(loc); });
    QObject::connect(passageBrowser, QOverload<qbv::Location>::of(&PassageBrowserNew::VerseSelected),
        this, [=] (qbv::Location loc) { OnVerseSelected(loc); });
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

void TabBibleNew::OnVerseSelected(qbv::Location loc)
{
    QList<qbv::Location> locations = m_pDatabaseService->GetXRefLocations(loc);

    int idx = ui_TabWidget_Bibles->currentIndex();
    QList<QString> passages;
    for (const qbv::Location &location : locations) {
        int verseId =  m_pDatabaseService->VerseId(location.book, location.chapter, location.verse);
        int endVerseId = location.IsSingleVerse() ?
            verseId : m_pDatabaseService->VerseId(location.endBook, location.endChapter, location.endVerse);
        passages << m_pDatabaseService->GetScriptures(idx, verseId, endVerseId).join(" ");
    }
    QList<qbv::PassageWithLocation> results;
    for (int i = 0; i < locations.count(); ++i) {
        results << qbv::PassageWithLocation { passages[i], locations[i] };
    }

    ui_XRefBrowser->SetResults(results);
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
          << QString::number(loc.verse)
          << QString::number(loc.endVerse);
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

void TabBibleNew::FindPhrase()
{
    ui_FindOnPageBox->show();
    ui_FindOnPageBox->SelectAllText();
}

#include "TabBible.h"

/*!
 * \brief A class that represents the tab for browsing different Bible translations.
 * \param verseData
 * \param bookNames a list of the names of the biblical books.
 * \param modules
 * \param parent specifies the parent widget for this object
 */
TabBible::TabBible(const QSqlDatabase &verseData, const QSqlDatabase &crossRefs, const QStringList &bookNames,
                   const QStringList &shortBookNames, const QList<qbv::Module> &modules, AppConfig *config,
                   QWidget *parent) :
    AbstractTab(parent),
    ui_CrossRefBox(new CrossReferenceBox(modules, crossRefs, bookNames)),
    ui_FindOnPageBox(new FindOnPageBox),
    m_pConfig(config),
    m_pBookNames(&bookNames),
    m_pBookShortNames(&shortBookNames),
    m_pModules(&modules),
    m_pVerseData(&verseData),
    m_pCrossRefs(&crossRefs)
{

}

void TabBible::AddControls()
{
    ui_NavigationPanel = new NavigationPanel(*m_pVerseData, *m_pBookNames, *m_pBookShortNames);
    ui_ModuleTabWidget = new ModuleTabWidget(*m_pModules, *m_pBookNames, m_pConfig);
    ui_TabBar_Modules = ui_ModuleTabWidget->tabBar();

    ui_VerLayout_Modules = new QVBoxLayout;
   // ui_VerLayout_Modules->addWidget(ui_ModuleTabWidget);

    ui_Splitter_ModuleCrossRef = new QSplitter(Qt::Vertical);
    ui_Splitter_ModuleCrossRef->addWidget(ui_ModuleTabWidget);
    ui_Splitter_ModuleCrossRef->addWidget(ui_CrossRefBox);
    ui_Splitter_ModuleCrossRef->restoreState(m_pConfig->general.splitter_layout);

   // ui_VerLayout_Modules->addWidget(ui_CrossRefBox);
    ui_VerLayout_Modules->addWidget(ui_Splitter_ModuleCrossRef);
    ui_VerLayout_Modules->addWidget(ui_FindOnPageBox);

    QHBoxLayout *tabBibleHorLayout = new QHBoxLayout;
    tabBibleHorLayout->setSpacing(5);
    tabBibleHorLayout->setContentsMargins(5, 5, 5, 5);
    tabBibleHorLayout->addLayout(ui_NavigationPanel);
    tabBibleHorLayout->addLayout(ui_VerLayout_Modules);

    QWidget::setLayout(tabBibleHorLayout);
}

void TabBible::ConnectSignals()
{
    QObject::connect(ui_NavigationPanel, QOverload<int, int, int, int>::of(&NavigationPanel::LocationChanged),
                     [=] (int book, int chapter, int verse1, int verse2)
                         { OnPassageChanged(book, chapter, verse1, verse2); });
    QObject::connect(ui_FindOnPageBox, QOverload<const QString &>::of(&FindOnPageBox::TextChanged),
                     [=] (const QString &text) { ui_ModuleTabWidget->HighlightText(text); });
    QObject::connect(ui_ModuleTabWidget, QOverload<int>::of(&ModuleTabWidget::CrossReferenceRequested),
                     [=] (int verse) { OnCrossReferenceRequested(verse); } );
    QObject::connect(ui_ModuleTabWidget, QOverload<qbv::Location>::of(&ModuleTabWidget::AddToFavoritesRequested),
                     [=] (qbv::Location loc) { emit AddToNotesRequested(loc); } );
    QObject::connect(ui_ModuleTabWidget, QOverload<int>::of(&QTabWidget::currentChanged),
                     [=] (int index) { OnModuleTabWidgetTabChanged(index); });
//  QObject::connect(ui_NavigationPanel, SIGNAL(randomChapterRequested()),
//                   this, SLOT(onRandomChapterRequested()));
}

void TabBible::SetUiTexts()
{
    qDebug() << "TabBible::SetUiTexts()";
    ui_NavigationPanel->SetUiTexts();
}

//void TabBible::AddModule(const QString &name, const QString &filePath, bool hasOT, bool hasStrong)
//{
//    qDebug() << "TabBible::addModule(const QString &name, const QString &filePath, bool hasOT, bool hasStrong)";
//    ui_ModuleTabWidget->AddModule(name, filePath, hasOT, hasStrong);
//    // ui_TabWidget_Modules->addTab(new PassageTextBrowser, name);
//    // ui_TabWidget_Modules->setTabToolTip(ui_TabWidget_Modules->count() - 1, filePath);
//}

void TabBible::AddModule(const qbv::Module &module)
{
    qDebug() << "TabBible::AddModule(const qbv::Module &module)" << module.shortName();
    ui_ModuleTabWidget->AddModule(module);
}

int TabBible::GetSelectedBook() const
{
    return ui_NavigationPanel->GetSelectedBook();
}

int TabBible::GetSelectedChapter() const
{
    return ui_NavigationPanel->GetSelectedChapter();
}

int TabBible::GetCurrentIndex() const
{
    return ui_ModuleTabWidget->currentIndex();
}

int TabBible::GetModuleCount() const
{
    return ui_ModuleTabWidget->count();
}

int TabBible::GetSelectedVerseFrom() const
{
    return ui_NavigationPanel->GetSelectedVerseFrom();
}

int TabBible::GetSelectedVerseTo() const
{
    return ui_NavigationPanel->GetSelectedVerseTo();
}

QByteArray TabBible::GetSplitterLayout() const
{
    return ui_Splitter_ModuleCrossRef->saveState();
}

void TabBible::HighlightVerse(int verse)
{
    ui_ModuleTabWidget->HighlightVerse(verse);
}

void TabBible::LoadPassageInCurrentTab()
{
    ui_ModuleTabWidget->LoadPassageInCurrentTab();
}

void TabBible::ReloadBookNames()
{
    ui_NavigationPanel->reloadBookNames();
}

void TabBible::SelectAllText()
{
    ui_ModuleTabWidget->SelectAllText();
}

void TabBible::SetActiveModule(int idx, bool blockSignals)
{
    ui_ModuleTabWidget->SetActiveModule(idx, blockSignals);
}

void TabBible::SetNavigationLocation(int book, int chapter, bool load)
{
    qDebug() << "TabBible::SetNavigationLocation(int book, int chapter)";
    ui_NavigationPanel->SetLocation(book, chapter, load);
}

//void TabBible::SelectPassage(int book, int chapter, int verse1, int verse2, bool sendSignal)
//{
//    qDebug() << "TabBible::SelectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal)";
//    ui_NavigationPanel->SetLocation({ book, chapter, verse1, verse2 }, sendSignal);
//}

void TabBible::SelectPassageFromConfig()
{
    qDebug() << "TabBible::SelectPassageFromConfig()";
    qbv::Location loc {m_pConfig->module_data.last_passage[0].toInt(),
                       m_pConfig->module_data.last_passage[1].toInt(),
                       m_pConfig->module_data.last_passage[2].toInt(),
                       m_pConfig->module_data.last_passage[3].toInt()};
    ui_NavigationPanel->SetLocation(loc);
    ui_ModuleTabWidget->SetLocation(loc);
}

void TabBible::SetPassageBrowserFont(const QFont &font)
{
    ui_ModuleTabWidget->SetBiblePassageBrowserFont(font);
}

void TabBible::SetPassageBrowserHighlightColor(QColor &color)
{
    ui_ModuleTabWidget->SetHighlightColor(color);
}

void TabBible::UpdateFromConfig()
{
    ui_ModuleTabWidget->UpdateFromConfig();
}

QString TabBible::getBookName(int idx) const
{
    qDebug() << "TabBible::getBookName(int idx)";
    return m_pBookNames->at(idx);
}

void TabBible::FindPhrase()
{
    ui_FindOnPageBox->show();
    ui_FindOnPageBox->SelectAllText();
}

void TabBible::OnPassageChanged(int book, int chapter, int verse1, int verse2)
{
    qDebug() << "TabBible::OnPassageChanged(int book, int chapter, int verse1, int verse2)";
    m_lastStatusMsg = QString("%1 %2:%3–%4").arg(m_pBookNames->at(book - 1)).arg(chapter)
            .arg(verse1).arg(verse2);
    emit StatusMsgSet(m_lastStatusMsg);
    int idx = ui_ModuleTabWidget->currentIndex();
    qbv::TabbedLocation tabbedLocation = { idx, book, chapter, verse1, verse2 };
    m_tabbedLocationHistory.append(tabbedLocation);
    if (m_tabbedLocationHistory.count() > 100) {
        m_tabbedLocationHistory.removeFirst();
    }
    ui_ModuleTabWidget->SetLocation(tabbedLocation);
    ui_ModuleTabWidget->LoadPassage(m_pModules->at(idx).translation);
}

void TabBible::onRandomChapterRequested()
{
}

void TabBible::OnModuleTabWidgetTabChanged(int index)
{
    qDebug() << ui_NavigationPanel->GetLocation().ToQString();
    qDebug() << ui_ModuleTabWidget->GetLocation().ToQString();
    if (ui_NavigationPanel->GetLocation() != ui_ModuleTabWidget->GetLocation()) {
        ui_ModuleTabWidget->LoadPassageInCurrentTab();
    }
}

void TabBible::OnCrossReferenceRequested(int verse)
{
    int idx = ui_ModuleTabWidget->currentIndex();
    int book = ui_NavigationPanel->GetSelectedBook();
    int chapter = ui_NavigationPanel->GetSelectedChapter();
    ui_CrossRefBox->LoadCrossReferences(idx, book, chapter, verse);
}

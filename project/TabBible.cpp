#include "TabBible.h"

TabBible::TabBible(const QSqlDatabase &verseData, const QStringList &bookNames, const QList<ModuleData> &modules,
                   QWidget *parent) :
    AbstractTab(parent),
    m_bookNames(&bookNames),
    m_modules(&modules),
    m_verseData(&verseData)
{

}

void TabBible::addControls()
{
    ui_NavigationPanel = new BibleNavigationPanel(*m_verseData, *m_bookNames);

    ui_ModuleTabWidget = new BibleModuleTabWidget;
    ui_TabBar_Modules = ui_ModuleTabWidget->tabBar();

    QHBoxLayout *tabBibleHorLayout = new QHBoxLayout;
    tabBibleHorLayout->setSpacing(5);
    tabBibleHorLayout->setContentsMargins(5, 5, 5, 5);
    tabBibleHorLayout->addLayout(ui_NavigationPanel);
    tabBibleHorLayout->addWidget(ui_ModuleTabWidget);

    QWidget::setLayout(tabBibleHorLayout);
}

void TabBible::connectSignals()
{
    QObject::connect(ui_NavigationPanel, SIGNAL(passageChanged(int, int, int, int)),
                     this, SLOT(onPassageChanged(int, int, int, int)));
//    QObject::connect(ui_NavigationPanel, SIGNAL(randomChapterRequested()),
//                     this, SLOT(onRandomChapterRequested()));
}

void TabBible::setUiTexts()
{
    qDebug() << "TabBible::setUiTexts()";
    ui_NavigationPanel->setUiTexts();
}

void TabBible::addModule(const QString& name, const QString& filePath, bool hasOT, bool hasStrong)
{
    ui_ModuleTabWidget->addModule(name, filePath, hasOT, hasStrong);
   // ui_TabWidget_Modules->addTab(new PassageTextBrowser, name);
    // ui_TabWidget_Modules->setTabToolTip(ui_TabWidget_Modules->count() - 1, filePath);
}

int TabBible::getCurrentIndex() const
{
    return ui_ModuleTabWidget->currentIndex();
}

int TabBible::getModuleCount() const
{
    return ui_ModuleTabWidget->count();
}

void TabBible::reloadBookNames()
{
    ui_NavigationPanel->reloadBookNames();
}

void TabBible::selectModule(int idx)
{
    ui_ModuleTabWidget->selectModule(idx);
}

void TabBible::selectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal)
{
    ui_NavigationPanel->selectPassage(book, chapter, verseFrom, verseTo, sendSignal);
}

void TabBible::setBiblePassageBrowserFont(const QFont &font)
{
    ui_ModuleTabWidget->setPassageTextBrowserFont(font);
}

void TabBible::onPassageChanged(int book, int chapter, int verseFrom, int verseTo)
{
    ui_ModuleTabWidget->loadPassage(book, chapter, verseFrom, verseTo,
                                    m_modules->at(ui_ModuleTabWidget->currentIndex()).database);
}

void TabBible::onRandomChapterRequested()
{
}

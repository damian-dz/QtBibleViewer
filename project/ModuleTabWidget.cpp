#include "ModuleTabWidget.h"
#include "TabBible.h"

ModuleTabWidget::ModuleTabWidget(const QList<qbv::Module> &modules, const QStringList &bookNames,
                                 AppConfig *config, QWidget *parent) :
    QTabWidget(parent),
    m_pConfig(config),
    m_pModules(&modules),
    m_pBookNames(&bookNames)

{
    QTabWidget::setMovable(true);
    QTabWidget::setTabsClosable(true);
    QTabWidget::setStyleSheet("QTabBar::close-button { "
                              "image: url(:/img/img_res/close-button.svg);"
                              "margin: 2px;"
                              "subcontrol-position: right; }"
                              "QTabBar::close-button:hover {"
                              "image: url(:/img/img_res/close-button-active.svg); }");
    UpdateFromConfig();
    ConnectSignals();
}

//void BibleModuleTabWidget::AddModule(const QString &name, const QString &filePath, bool hasOT, bool hasStrong)
//{
//    BiblePassageBrowser *passageTextBrowser = new BiblePassageBrowser(*m_pBookNames, m_pConfig);
//    passageTextBrowser->setHasOldTestament(hasOT);
//    passageTextBrowser->setHasStrong(hasStrong);
//    m_passageBrowers.append(passageTextBrowser);
//    QTabWidget::addTab(passageTextBrowser, name);
//    QTabWidget::setTabToolTip(QTabWidget::count() - 1, filePath);
//    QObject::connect(passageTextBrowser, QOverload<int>::of(&BiblePassageBrowser::CrossReferenceRequested),
//                     [=] (int verse) { emit CrossReferenceRequested(verse); });
//}

void ModuleTabWidget::AddModule(const qbv::Module &module)
{
    qDebug() << "ModuleTabWidget::AddModule(const qbv::Module &module)" << module.shortName();;
    PassageBrowser *passageTextBrowser = new PassageBrowser(*m_pBookNames, m_pConfig);
    passageTextBrowser->SetHasOldTestament(module.hasOT());
    passageTextBrowser->SetHasStrong(module.hasStrong());
    m_passageBrowers.append(passageTextBrowser);
    if (QTabWidget::count() == 0)  {
        QTabWidget::blockSignals(true);
    }
    QTabWidget::addTab(passageTextBrowser, module.shortName());
    if (QTabWidget::count() == 1)  {
        QTabWidget::blockSignals(false);
    }
    QTabWidget::setTabToolTip(QTabWidget::count() - 1, module.filePath());
    QObject::connect(passageTextBrowser, QOverload<int>::of(&PassageBrowser::CrossReferenceRequested),
                     [=] (int verse) { emit CrossReferenceRequested(verse); });
    QObject::connect(passageTextBrowser, QOverload<qbv::Location>::of(&PassageBrowser::AddToFavoritesRequested),
                     [=] (qbv::Location loc) { emit AddToFavoritesRequested(loc); });
}

qbv::Location ModuleTabWidget::GetLocation() const
{
    int idx = QTabWidget::currentIndex();
    return m_passageBrowers[idx]->GetLocation();
}

bool ModuleTabWidget::HasModules() const
{
    return QTabWidget::count() > 0;
}

void ModuleTabWidget::HighlightText(const QString &text)
{
    m_passageBrowers[QTabWidget::currentIndex()]->HighlightText(text);
}

void ModuleTabWidget::HighlightVerse(int verse)
{
    m_passageBrowers[QTabWidget::currentIndex()]->HighlightVerse(verse);
}

void ModuleTabWidget::LoadPassage(const QSqlDatabase &module)
{
    qDebug() << "ModuleTabWidget::LoadPassage(const QSqlDatabase &module)" << module.databaseName();
    if (HasModules()) {
        int idx = QTabWidget::currentIndex();
        m_passageBrowers[idx]->SetLocation(m_location);
        m_passageBrowers[idx]->LoadPassage_New(module);
    }
}

void ModuleTabWidget::LoadPassage(int book, int chapter, int verseFrom, int verseTo, const QSqlDatabase &module)
{
    qDebug() << "ModuleTabWidget::LoadPassage(int book, int chapter, int verseFrom, int verseTo, const QSqlDatabase &module)";
    if (HasModules()) {
        int idx = QTabWidget::currentIndex();
        m_location = qbv::Location(book, chapter, verseFrom, verseTo);
        m_passageBrowers[idx]->SetLocation(book, chapter, verseFrom, verseTo);
        m_passageBrowers[idx]->LoadPassage_New(module);
    }
}

void ModuleTabWidget::SetActiveModule(int idx, bool blockSignals)
{
    qDebug() << "ModuleTabWidget::SetActiveModule(int idx)";
    QTabWidget::blockSignals(blockSignals);
    QTabWidget::setCurrentIndex(idx);
    if (blockSignals) {
        QTabWidget::blockSignals(false);
    }
}

void ModuleTabWidget::SelectAllText()
{
    m_passageBrowers[QTabWidget::currentIndex()]->selectAll();
}

void ModuleTabWidget::SetHighlightColor(QColor &color)
{
    for (PassageBrowser *passageBrowser : m_passageBrowers) {
        passageBrowser->SetHighlightColor(color);
    }
}

void ModuleTabWidget::SetLocation(qbv::Location location)
{
    qDebug() << "ModuleTabWidget::SetLocation(qbv::Location location)";
    m_location = location;
}

void ModuleTabWidget::SetBiblePassageBrowserFont(const QFont &font)
{
    for (PassageBrowser *passageBrowser : m_passageBrowers) {
        passageBrowser->setFont(font);
    }
}

void ModuleTabWidget::UpdateFromConfig()
{
    QTabWidget::setTabPosition(QTabWidget::TabPosition(m_pConfig->appearance.module_tab_position));
}

void ModuleTabWidget::ConnectSignals()
{
    QObject::connect(this, QOverload<int>::of(&QTabWidget::tabCloseRequested),
                     [=] (int index) { OnTabCloseRequested(index); });
    QObject::connect(QTabWidget::tabBar(), QOverload<int, int>::of(&QTabBar::tabMoved),
                     [=] (int from, int to) { OnModuleMoved(from, to); });
}

void ModuleTabWidget::OnModuleMoved(int from, int to)
{
    m_passageBrowers.swapItemsAt(from, to);
}

void ModuleTabWidget::LoadPassageInCurrentTab()
{
    qDebug() << "ModuleTabWidget::LoadPassageInCurrentTab()";
    int idx = QTabWidget::currentIndex();
    LoadPassage(m_pModules->at(idx).translation);
}

void ModuleTabWidget::OnTabCloseRequested(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Removal"),
                                  tr("Are you sure you want to remove this module?\n"
                                     "It will not be visible unless you re-add it manually."),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QTabWidget::removeTab(index);
        m_passageBrowers.removeAt(index);
    }
}

#include "BibleModuleTabWidget.h"

BibleModuleTabWidget::BibleModuleTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    QTabWidget::setMovable(true);
    // setTabPosition(QTabWidget::TabPosition(m_pConfig->appearance.module_tab_position));
    QTabWidget::setTabsClosable(true);
    QTabWidget::setStyleSheet("QTabBar::close-button { "
                              "image: url(:/img/img_res/close-button.svg);"
                              "margin: 2px;"
                              "subcontrol-position: right; }"
                              "QTabBar::close-button:hover {"
                              "image: url(:/img/img_res/close-button-active.svg); }");
    connectSignals();
}

void BibleModuleTabWidget::addModule(const QString &name, const QString &filePath, bool hasOT, bool hasStrong)
{
    BiblePassageBrowser *passageTextBrowser = new BiblePassageBrowser;
    passageTextBrowser->setHasOldTestament(hasOT);
    passageTextBrowser->setHasStrong(hasStrong);
    m_passageTextBrowers.append(passageTextBrowser);
    QTabWidget::addTab(passageTextBrowser, name);
    QTabWidget::setTabToolTip(QTabWidget::count() - 1, filePath);
}

bool BibleModuleTabWidget::hasModules() const
{
    return QTabWidget::count() > 0;
}

void BibleModuleTabWidget::loadPassage(int book, int chapter, int verseFrom, int verseTo, const QSqlDatabase& module)
{
    if (hasModules()) {
        int idx = QTabWidget::currentIndex();
        m_passageTextBrowers[idx]->setPassage(book, chapter, verseFrom, verseTo);
        m_passageTextBrowers[idx]->loadPassageV1(module);
    }
}

void BibleModuleTabWidget::selectModule(int idx)
{
    QTabWidget::setCurrentIndex(idx);
}

void BibleModuleTabWidget::setPassageTextBrowserFont(const QFont &font)
{
    for (BiblePassageBrowser* passageTextBrowser : m_passageTextBrowers) {
        passageTextBrowser->setFont(font);
    }
}

void BibleModuleTabWidget::connectSignals()
{
    QObject::connect(this, QOverload<int>::of(&QTabWidget::tabCloseRequested),
                     [=] (int index) { onTabCloseRequested(index); });
    QObject::connect(QTabWidget::tabBar(), QOverload<int, int>::of(&QTabBar::tabMoved),
                     [=] (int from, int to) { onModuleMoved(from, to); });
}

void BibleModuleTabWidget::onModuleMoved(int from, int to)
{
    m_passageTextBrowers.swapItemsAt(from, to);
}

void BibleModuleTabWidget::onTabCloseRequested(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Removal"),
                                  tr("Are you sure you want to remove this module?\n"
                                     "It will not be visible unless you re-add it manually."),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QTabWidget::removeTab(index);
        m_passageTextBrowers.removeAt(index);
    }
}

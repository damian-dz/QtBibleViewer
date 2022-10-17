#include "TabCompare.h"

TabCompare::TabCompare(const QStringList &bookNames, const QStringList &shortBookNames, const QList<qbv::Module> &modules, const QSqlDatabase &verseData,
                       QWidget *parent) :
    AbstractTab(parent),
    ui_NavigationPanel(new NavigationPanel(verseData, bookNames, shortBookNames)),
    ui_CompareTextBrowser(new CompareVerseBrowser),
    m_pBookNames(&bookNames),
    m_pModules(&modules)
{
    ui_NavigationPanel->SetMoveByVerse(true);

   // table->cellAt(0, 0).firstCursorPosition().insertText("First");
   // table->cellAt(1, 0).firstCursorPosition().insertText("Second");
   // table->cellAt(2, 0).firstCursorPosition().insertText("Third");

}

void TabCompare::ConnectSignals()
{
    QObject::connect(ui_NavigationPanel, QOverload<int, int, int, int>::of(&NavigationPanel::LocationChanged),
                     [=] (int book, int chapter, int verse1, int verse2) { OnVerseChanged(book, chapter, verse1); });
    QObject::connect(ui_CompareTextBrowser,
                     QOverload<int, int, int, int>::of(&CompareVerseBrowser::TranslationNameClicked),
                     [=] (int idx, int book, int chapter, int verse)
                         { emit TranslationNameClicked(idx, book, chapter, verse ); });
    //QObject::connect(ui_FindOnPageBox, QOverload<const QString &>::of(&FindOnPageBox::TextChanged),
    //                 [=] (const QString &text) { ui_ModuleTabWidget->HighlightText(text); });
}

void TabCompare::AddControls()
{
    ui_NavigationPanel->HideVerseTo();
    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->setSpacing(5);
    mainHorLayout->setContentsMargins(5, 5, 5, 5);
    mainHorLayout->addLayout(ui_NavigationPanel);
    mainHorLayout->addWidget(ui_CompareTextBrowser);
    QWidget::setLayout(mainHorLayout);
}

void TabCompare::SetUiTexts()
{
    ui_NavigationPanel->SetUiTexts();
}

void TabCompare::SetFont(const QFont &font)
{
    ui_CompareTextBrowser->setFont(font);
}

void TabCompare::OnVerseChanged(int book, int chapter, int verse)
{
    ui_CompareTextBrowser->SetVerseLocation(book, chapter, verse);
   // ui_CompareTextBrowser->LoadNamesAndVerses(*m_pModules);
    m_lastStatusMsg = QString("%1 %2:%3").arg(m_pBookNames->at(book - 1)).arg(chapter).arg(verse);
    emit StatusMsgSet(m_lastStatusMsg);
}

#include "TabSearchNew.h"

TabSearchNew::TabSearchNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService)
{
    QGridLayout *searchResultsGridLayout = new QGridLayout;

    ui_Label_Enter = new QLabel;

    ui_LineEdit_Search = new QLineEdit;
    ui_LineEdit_Search->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_LineEdit_Search->setFixedHeight(21);
    ui_LineEdit_Search->setFocus();

    ui_Button_Search = new QPushButton;
    ui_Button_Search->setDisabled(true);
    ui_Button_Search->setFixedHeight(23);

    ui_Label_Results = new QLabel;
    ui_searchResultsBrowser = new SearchResultsBrowser(config, databaseService);


    QHBoxLayout *randomVerseHorLayout = new QHBoxLayout;

    QVBoxLayout *navVerLayout = new QVBoxLayout;
    randomVerseHorLayout->addLayout(navVerLayout);

    QHBoxLayout *prevNextHorLayout = new QHBoxLayout;
    navVerLayout->addLayout(prevNextHorLayout);

    ui_Button_First = new QPushButton("|<");
    ui_Button_First->setDisabled(true);
    ui_Button_First->setMaximumWidth(40);
    prevNextHorLayout->addWidget(ui_Button_First);

    ui_Button_Prev = new QPushButton("<<");
    ui_Button_Prev->setDisabled(true);
    ui_Button_Prev->setMaximumWidth(50);
    prevNextHorLayout->addWidget(ui_Button_Prev);

    ui_Button_Next = new QPushButton(">>");
    ui_Button_Next->setDisabled(true);
    ui_Button_Next->setMaximumWidth(50);
    prevNextHorLayout->addWidget(ui_Button_Next);

    ui_Button_Last = new QPushButton(">|");
    ui_Button_Last->setDisabled(true);
    ui_Button_Last->setMaximumWidth(40);
    prevNextHorLayout->addWidget(ui_Button_Last);

    QHBoxLayout *goToHorLayout = new QHBoxLayout;
    navVerLayout->addLayout(goToHorLayout);

    ui_Label_GoToPage = new QLabel;

    goToHorLayout->addWidget(ui_Label_GoToPage);

    ui_SpinBox_PageNum = new QSpinBox;
    ui_SpinBox_PageNum->setDisabled(true);
    ui_SpinBox_PageNum->setRange(0, 0);
    ui_SpinBox_PageNum->setFixedWidth(55);
    goToHorLayout->addWidget(ui_SpinBox_PageNum);

    ui_Label_Of = new QLabel;

    goToHorLayout->addWidget(ui_Label_Of);

    ui_Button_GoTo = new QPushButton;
    ui_Button_GoTo->setIcon(QIcon(":/img/img_res/arrow_right.svg"));
    ui_Button_GoTo->setFixedWidth(35);
    ui_Button_GoTo->setDisabled(true);
    goToHorLayout->addWidget(ui_Button_GoTo);

    ui_Button_RandomVerse = new QPushButton;
    navVerLayout->addWidget(ui_Button_RandomVerse);

    ui_randomVerseBrowser = new SearchResultsBrowser(config, databaseService);
    ui_randomVerseBrowser->setMaximumHeight(80);

    randomVerseHorLayout->addWidget(ui_randomVerseBrowser);



    searchResultsGridLayout->addWidget(ui_Label_Enter, 0, 0);
    searchResultsGridLayout->addWidget(ui_LineEdit_Search, 1, 0);
    searchResultsGridLayout->addWidget(ui_Button_Search, 1, 1);
    searchResultsGridLayout->addWidget(ui_Label_Results, 2, 0);
    searchResultsGridLayout->addWidget(ui_searchResultsBrowser, 3, 0, 1, 2);
    searchResultsGridLayout->addLayout(randomVerseHorLayout, 4, 0, 1, 2);

    ui_SearchOptionsPanel = new SearchOptionsPanelNew(config, databaseService);

    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addLayout(searchResultsGridLayout);
    mainHorLayout->addWidget(ui_SearchOptionsPanel);
    QWidget::setLayout(mainHorLayout);
   // ui_SearchResultsArea->setNumResultsPerPage(25);
}

void TabSearchNew::SetUiTexts()
{
    ui_Label_Enter->setText(tr("Enter a Word or a Phrase:"));
    ui_Button_Search->setText(tr("Search"));
    ui_Label_Results->setText(tr("Results:"));

    ui_Label_GoToPage->setText(tr("Go to Page:"));
    ui_Label_Of->setText(tr("of"));

     ui_Button_RandomVerse->setText(tr("Random Verse"));

    ui_SearchOptionsPanel->SetUiTexts();
}

void TabSearchNew::AddControls()
{

}

void TabSearchNew::ConnectSignals()
{
    QObject::connect(ui_LineEdit_Search, QOverload<const QString &>::of(&QLineEdit::textChanged),
                     [=] (const QString &text) { ui_Button_Search->setEnabled(text.trimmed().length() > 0); });
    QObject::connect(ui_Button_Search, QOverload<bool>::of(&QPushButton::clicked), [=] { OnSearchButtonClicked(); });
}

void TabSearchNew::OnSearchButtonClicked()
{
    SearchOptions options = ui_SearchOptionsPanel->GetSearchOptions();
    m_lastIdx = ui_SearchOptionsPanel->GetBibleIndex();

    QElapsedTimer timer;
    timer.start();
    auto highlightRgx = m_pDatabaseService->Search(m_lastIdx, ui_LineEdit_Search->text(), options);
    int numResults = m_pDatabaseService->GetNumLastSearchResults(m_lastIdx);
    QString statusMsg = QString(tr("The search took %1 s. ")).arg(timer.elapsed() * 0.001);
    statusMsg += QString(tr("Number of verses found: %1.")).arg(numResults);
    emit StatusMsgSet(statusMsg);
    SetLastStatusMsg(statusMsg);

    auto searchResults = m_pDatabaseService->GetLastSearchResults(m_lastIdx, 0, 25);
    bool hasStrong = m_pDatabaseService->HasStrong(m_lastIdx);
    ui_searchResultsBrowser->SetResults(searchResults, hasStrong);
    ui_searchResultsBrowser->HighlightKeywords(highlightRgx);
}

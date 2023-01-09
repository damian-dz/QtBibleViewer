#include "Formatting.h"
#include "SearchResultArea.h"

SearchResultArea::SearchResultArea(QWidget *parent) :
    QVBoxLayout(parent),
    ui_LineEdit_Search(new QLineEdit),
    ui_Label_Enter(new QLabel),
    m_currentIdx(0),
    m_numResPerPage(25)
{
    ui_LineEdit_Search->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_LineEdit_Search->setFixedHeight(21);
    ui_LineEdit_Search->setFocus();

    ui_Button_Search = new QPushButton;
    ui_Button_Search->setDisabled(true);
    ui_Button_Search->setFixedHeight(23);

    QHBoxLayout *enterSearchHorLayout = new QHBoxLayout;
    enterSearchHorLayout->addWidget(ui_LineEdit_Search);
    enterSearchHorLayout->addWidget(ui_Button_Search);

    ui_Label_Results = new QLabel;

    ui_SearchResultsBrowser = new SearchResultBrowser;

    QHBoxLayout *randomVerseHorLayout = new QHBoxLayout;

    QVBoxLayout *randomVerseVerLayout = new QVBoxLayout;
    randomVerseHorLayout->addLayout(randomVerseVerLayout);

    QHBoxLayout *prevNextHorLayout = new QHBoxLayout;
    randomVerseVerLayout->addLayout(prevNextHorLayout);

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
    randomVerseVerLayout->addLayout(goToHorLayout);

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
    randomVerseVerLayout->addWidget(ui_Button_RandomVerse);

    ui_RandomVerseBrowser = new SearchResultBrowser;
    ui_RandomVerseBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    // ui_Sea_TextBrowser_RandomVerse->setFont(m_currentFont);
    ui_RandomVerseBrowser->setMaximumHeight(80);
    ui_RandomVerseBrowser->SetIncludeResultNumber(false);
    randomVerseHorLayout->addWidget(ui_RandomVerseBrowser);
    // setBrowserBackground(*ui_Sea_TextBrowser_RandomVerse);

    QBoxLayout::addWidget(ui_Label_Enter);
    QBoxLayout::addLayout(enterSearchHorLayout);
    QBoxLayout::addWidget(ui_Label_Results);
    QBoxLayout::addWidget(ui_SearchResultsBrowser);
    QBoxLayout::addLayout(randomVerseHorLayout);

    SetUiTexts();
    ConnectSignals();
}

SearchResultArea::~SearchResultArea()
{

}

void SearchResultArea::ConnectSignals()
{
    QObject::connect(ui_LineEdit_Search, QOverload<const QString &>::of(&QLineEdit::textChanged),
                     [=] (const QString &text) { ui_Button_Search->setEnabled(text.trimmed().length() > 0); });
    QObject::connect(ui_LineEdit_Search, QOverload<>::of(&QLineEdit::returnPressed),
                     [=] { ui_Button_Search->click(); });
    QObject::connect(ui_Button_Search, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { emit SearchButtonClicked(ui_LineEdit_Search->text()); });
    QObject::connect(ui_Button_GoTo, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onGoToButtonClicked(); });
    QObject::connect(ui_Button_Prev, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onPrevButtonClicked(); });
    QObject::connect(ui_Button_Next, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onNextButtonClicked(); });
    QObject::connect(ui_Button_RandomVerse, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { emit RandomVerseRequested(); });
    QObject::connect(ui_SearchResultsBrowser, QOverload<int, int, int>::of(&SearchResultBrowser::ReferenceClicked),
                     [=] (int book, int chapter, int verse) { emit ReferenceClicked(book, chapter, verse); } );
}

void SearchResultArea::DisplayResults()
{
    int endIdx = qMin(m_currentIdx + m_numResPerPage, m_pResults->count());
    ui_SearchResultsBrowser->SetResults(*m_pResults, *m_pRefs, m_currentIdx, endIdx);
    ui_SearchResultsBrowser->HighlightKeywords(m_highlightRegex);
}

void SearchResultArea::SetFont(const QFont &font)
{
    ui_SearchResultsBrowser->setFont(font);
    ui_RandomVerseBrowser->setFont(font);
}

void SearchResultArea::SetHighlightRegex(const QRegularExpression &regex)
{
    m_highlightRegex = regex;
}

void SearchResultArea::SetUiTexts()
{
    ui_Label_Enter->setText(tr("Enter a Word or a Phrase:"));
    ui_Label_Results->setText(tr("Results:"));
    ui_Label_GoToPage->setText(tr("Go to Page:"));
    ui_Label_Of->setText(tr("of"));

    ui_Button_Search->setText(tr("Search"));
    ui_Button_RandomVerse->setText(tr("Random Verse"));
}

void SearchResultArea::setNumResultsPerPage(int numResults)
{
    m_numResPerPage = numResults;
}

void SearchResultArea::setRandomVerse(const QString &bookName, int chapter, int verse, const QString &scripture)
{
    ui_RandomVerseBrowser->SetResult(scripture, QString("<b>—%1 %2:%3</b>")
                                     .arg(bookName, QString::number(chapter), QString::number(verse)));
}

void SearchResultArea::SetResults(const QStringList &results, const QStringList &refs)
{
    ui_Button_Prev->setDisabled(true);
    m_pResults = &results;
    m_pRefs = &refs;
    m_currentIdx = 0;
    ui_Button_Next->setEnabled(results.count() > m_numResPerPage);
    int numPages = results.count() / m_numResPerPage;
    if (results.count() % m_numResPerPage) {
        ++numPages;
    }
    ui_Label_Of->setText(tr("of") % " " % QString::number(numPages));
    if (results.count() > 0) {
        ui_SpinBox_PageNum->setEnabled(true);
        ui_SpinBox_PageNum->setRange(1, numPages);
        ui_SpinBox_PageNum->setValue(1);
        ui_Button_GoTo->setEnabled(true);
    } else {
        ui_SpinBox_PageNum->setDisabled(true);
        ui_SpinBox_PageNum->setRange(0, 0);
        ui_SpinBox_PageNum->setValue(0);
        ui_Button_GoTo->setDisabled(true);
    }
}

void SearchResultArea::onRandomVerseButtonClicked()
{
    qDebug() << "onRandomVerseButtonClicked()";
}

void SearchResultArea::setResultsAndButtons()
{
    ui_Button_Next->setDisabled(m_currentIdx + m_numResPerPage >= m_pResults->count());
    DisplayResults();
    ui_Button_Prev->setEnabled(m_currentIdx >= m_numResPerPage);
}

void SearchResultArea::onGoToButtonClicked()
{
    int pageNumber = ui_SpinBox_PageNum->value();
    m_currentIdx = (pageNumber - 1) * m_numResPerPage;
    setResultsAndButtons();
}

void SearchResultArea::onNextButtonClicked()
{
    m_currentIdx += m_numResPerPage;
    setResultsAndButtons();
}

void SearchResultArea::onPrevButtonClicked()
{
    m_currentIdx -= m_numResPerPage;
    setResultsAndButtons();
}

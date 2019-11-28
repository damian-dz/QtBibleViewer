#include "SearchResultsArea.h"

SearchResultsArea::SearchResultsArea(QWidget *parent) :
    QVBoxLayout(parent),
    m_currentIdx(0),
    m_numResPerPage(25)
{
    ui_Label_Enter = new QLabel;

    ui_LineEdit_Search = new QLineEdit;
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

    ui_TextBrowser_Results = new QTextBrowser;

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

    ui_TextBrowser_RandomVerse = new QTextBrowser;
    ui_TextBrowser_RandomVerse->setContextMenuPolicy(Qt::CustomContextMenu);
    // ui_Sea_TextBrowser_RandomVerse->setFont(m_currentFont);
    ui_TextBrowser_RandomVerse->setMaximumHeight(80);
    randomVerseHorLayout->addWidget(ui_TextBrowser_RandomVerse);
    // setBrowserBackground(*ui_Sea_TextBrowser_RandomVerse);

    QBoxLayout::addWidget(ui_Label_Enter);
    QBoxLayout::addLayout(enterSearchHorLayout);
    QBoxLayout::addWidget(ui_Label_Results);
    QBoxLayout::addWidget(ui_TextBrowser_Results);
    QBoxLayout::addLayout(randomVerseHorLayout);

    setUiTexts();
    connectSignals();
}

void SearchResultsArea::connectSignals()
{
    QObject::connect(ui_LineEdit_Search, QOverload<const QString &>::of(&QLineEdit::textChanged),
                     [=] (const QString &text) { ui_Button_Search->setEnabled(text.trimmed().length() > 0); });
    QObject::connect(ui_LineEdit_Search, QOverload<>::of(&QLineEdit::returnPressed),
                     [=] { ui_Button_Search->click(); });
    QObject::connect(ui_Button_Search, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { emit searchButtonClicked(ui_LineEdit_Search->text()); });
    QObject::connect(ui_Button_Prev, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onPrevButtonClicked(); });
    QObject::connect(ui_Button_Next, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onNextButtonClicked(); });
}

void SearchResultsArea::displayResults()
{
    qDebug() << "num res:" << m_results->count();
    ui_TextBrowser_Results->clear();
    int endIdx = std::min(m_currentIdx + m_numResPerPage, m_results->count());
    for (int i = m_currentIdx; i < endIdx; i++) {
        QTextCursor cursor = ui_TextBrowser_Results->textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml("[" % QString::number(i + 1) % "] " % m_results->at(i));
        cursor.insertBlock();
        ui_TextBrowser_Results->moveCursor(QTextCursor::Start);
    }
}

void SearchResultsArea::setUiTexts()
{
    ui_Label_Enter->setText(tr("Enter a Word or a Phrase:"));
    ui_Label_Results->setText(tr("Results:"));
    ui_Label_GoToPage->setText(tr("Go to Page:"));
    ui_Label_Of->setText(tr("of"));

    ui_Button_Search->setText(tr("Search"));
    ui_Button_RandomVerse->setText(tr("Random Verse"));
}

void SearchResultsArea::setNumResultsPerPage(int numResults)
{
    m_numResPerPage = numResults;
}

void SearchResultsArea::setResults(const QStringList &results)
{
    m_results = &results;
    m_currentIdx = 0;
    ui_Button_Next->setEnabled(results.count() > m_numResPerPage);
}

void SearchResultsArea::onNextButtonClicked()
{
    m_currentIdx += m_numResPerPage;
    displayResults();
}

void SearchResultsArea::onPrevButtonClicked()
{
    m_currentIdx -= m_numResPerPage;
    displayResults();
}

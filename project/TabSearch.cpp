#include "TabSearch.h"
#include "SearchEngine.h"

TabSearch::TabSearch(const QStringList &bookNames, QList<qbv::Module> &moduleData, QWidget *parent) :
    AbstractTab(parent),
    m_bookNames(&bookNames),
    m_moduleData(&moduleData)
{
}

int TabSearch::GetNumResults() const
{
    return m_results.count();
}

int TabSearch::GetTranslationIndex() const
{
    return ui_SearchOptionsPanel->GetTranslationIndex();
}

void TabSearch::SetFont(const QFont &font)
{
    ui_SearchResultsArea->SetFont(font);
}

void TabSearch::ConnectSignals()
{
    QObject::connect(ui_SearchResultsArea, QOverload<const QString &>::of(&SearchResultArea::SearchButtonClicked),
                     [=] (const QString &text) { OnSearchButtonClicked(text); });
    QObject::connect(ui_SearchOptionsPanel, QOverload<int>::of(&SearchOptionsPanel::numResultsPerPageChanged),
                     [=] (int numResults) { ui_SearchResultsArea->setNumResultsPerPage(numResults); });
    QObject::connect(ui_SearchResultsArea, QOverload<>::of(&SearchResultArea::RandomVerseRequested),
                     [=] { OnRandomVerseRequested(); });
    QObject::connect(ui_SearchResultsArea, QOverload<int, int, int>::of(&SearchResultArea::ReferenceClicked),
                     [=] (int book, int chapter, int verse) { emit ReferenceClicked(book, chapter, verse); } );
}

void TabSearch::AddControls()
{
    ui_SearchResultsArea = new SearchResultArea;
    QStringList translations;
    for (int i = 0; i < m_moduleData->count(); ++i) {
        translations.append(m_moduleData->at(i).shortName());
    }
    ui_SearchOptionsPanel = new SearchOptionsPanel(*m_bookNames, translations);
    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addLayout(ui_SearchResultsArea);
    mainHorLayout->addWidget(ui_SearchOptionsPanel);
    QWidget::setLayout(mainHorLayout);
    ui_SearchResultsArea->setNumResultsPerPage(ui_SearchOptionsPanel->GetNumResultsPerPage());
}

void TabSearch::SetUiTexts()
{
    ui_SearchResultsArea->SetUiTexts();
    ui_SearchOptionsPanel->SetUiTexts();
}

void TabSearch::OnSearchButtonClicked(const QString &text)
{
    SearchOptions searchOptions = ui_SearchOptionsPanel->getSearchOptions();
    qDebug() << text;
    qDebug() << searchOptions.translation;
    qDebug() << searchOptions.bookTo;
    qbv::Module module = m_moduleData->at(searchOptions.translation);
    m_results.clear();
    m_refs.clear();

//    QString command = "SELECT * FROM Bible WHERE  Book = 1 AND Chapter = 1 AND Verse = 1";
//    QSqlQuery query(module.translation);
//    if (query.exec(command)) {
//        while (query.next()) {
//            QSqlRecord record = query.record();
//            qDebug() << "-----------";
//            qDebug() << record.value(4).toString();
//        }

//    }

    QElapsedTimer timer;
    timer.start();
    QRegularExpression dispRegex =  SearchEngine::search(text, searchOptions, module.translation, module.hasStrong(),
                                                         m_refs, *m_bookNames, m_results);

    qDebug() << m_results.count();
    //emit SearchTimeElapsed(timer.elapsed());

    QString statusMsg = QString(tr("The search took %1 s. ")).arg(timer.elapsed() / 1000.0);
    statusMsg += QString(tr("Number of verses found: %1.")).arg(m_results.count());
    m_lastStatusMsg = statusMsg;
    emit StatusMsgSet(statusMsg);

    ui_SearchResultsArea->SetResults(m_results, m_refs);
    ui_SearchResultsArea->SetHighlightRegex(dispRegex);
    ui_SearchResultsArea->DisplayResults();
}

void TabSearch::OnRandomVerseRequested()
{
    int idx = ui_SearchOptionsPanel->GetTranslationIndex();
    QSqlQuery query(m_moduleData->at(idx).translation);
    int bookFrom = ui_SearchOptionsPanel->GetBookFrom();
    int bookTo = ui_SearchOptionsPanel->GetBookTo();
    if (query.exec(QString("SELECT * FROM Bible WHERE Book>=%1 AND Book<=%2 ORDER BY RANDOM() LIMIT 1")
                   .arg(QString::number(bookFrom), QString::number(bookTo)))) {
        if (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value("book").toInt() - 1;
            QString bookName = m_bookNames->at(book);
            int chapter = record.value("chapter").toInt();
            int verse = record.value("verse").toInt();
            QString scripture = record.value("scripture").toString();
            ui_SearchResultsArea->setRandomVerse(bookName, chapter, verse, scripture);
        }
    }
}

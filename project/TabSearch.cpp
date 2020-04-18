#include "TabSearch.h"
#include "SearchEngine.h"

TabSearch::TabSearch(const QStringList &bookNames, QList<Module> &moduleData, QWidget *parent) :
    AbstractTab(parent),
    m_bookNames(&bookNames),
    m_moduleData(&moduleData)
{
}

void TabSearch::setResultsAreaFont(const QFont &font)
{
    ui_SearchResultsArea->setFonts(font);
}

void TabSearch::connectSignals()
{
    QObject::connect(ui_SearchResultsArea, QOverload<const QString &>::of(&SearchResultsArea::searchButtonClicked),
                     [=] (const QString &text) { onSearchButtonClicked(text); });
    QObject::connect(ui_SearchOptionsPanel, QOverload<int>::of(&SearchOptionsPanel::numResultsPerPageChanged),
                     [=] (int numResults) { ui_SearchResultsArea->setNumResultsPerPage(numResults); });
}

void TabSearch::addControls()
{
    ui_SearchResultsArea = new SearchResultsArea;
    QStringList translations;
    for (int i = 0; i < m_moduleData->count(); ++i) {
        translations.append(m_moduleData->at(i).name);
    }
    ui_SearchOptionsPanel = new SearchOptionsPanel(*m_bookNames, translations);
    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addLayout(ui_SearchResultsArea);
    mainHorLayout->addWidget(ui_SearchOptionsPanel);
    QWidget::setLayout(mainHorLayout);
    ui_SearchResultsArea->setNumResultsPerPage(ui_SearchOptionsPanel->getNumResultsPerPage());
}

void TabSearch::setUiTexts()
{
    ui_SearchResultsArea->setUiTexts();
    ui_SearchOptionsPanel->setUiTexts();
}

void TabSearch::onSearchButtonClicked(const QString &text)
{
    SearchOptions searchOptions = ui_SearchOptionsPanel->getSearchOptions();
    qDebug() << text;
    qDebug() << searchOptions.bookFrom;
    Module module = m_moduleData->at(searchOptions.translation);
    m_results.clear();
    m_refs.clear();
    m_results = SearchEngine::search(text, searchOptions, module.database, module.hasStrong, m_refs, *m_bookNames);
    ui_SearchResultsArea->setResults(m_results, m_refs);
    ui_SearchResultsArea->displayResults();
}

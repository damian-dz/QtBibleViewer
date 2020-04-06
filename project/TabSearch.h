#ifndef TABSEARCH_H
#define TABSEARCH_H

#include "AbstractTab.h"
#include "Location.h"
#include "ModuleData.h"
#include "SearchOptionsPanel.h"
#include "SearchResultsArea.h"

class TabSearch : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabSearch(const QStringList &bookNames, QList<ModuleData> &moduleData, QWidget *parent = nullptr);

    void setResultsAreaFont(const QFont& font);

    virtual void connectSignals() override;
    virtual void addControls() override;
    virtual void setUiTexts() override;

signals:

public slots:
    void onSearchButtonClicked(const QString &text);

private:
    SearchOptionsPanel *ui_SearchOptionsPanel;
    SearchResultsArea *ui_SearchResultsArea;

    const QStringList *m_bookNames;
    QList<ModuleData> *m_moduleData;
    QHash<QString, QLabel *> m_labels;
    QStringList m_results;
    QStringList m_refs;
};

#endif // TABSEARCH_H

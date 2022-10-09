#ifndef TABSEARCH_H
#define TABSEARCH_H

#include "AbstractTab.h"
#include "TabbedLocation.h"
#include "Module.h"
#include "SearchOptionsPanel.h"
#include "SearchResultArea.h"

class TabSearch : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabSearch(const QStringList &bookNames, QList<qbv::Module> &moduleData, QWidget *parent = nullptr);

    int GetNumResults() const;
    int GetTranslationIndex() const;
    void SetFont(const QFont& font);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;

signals:
    void ReferenceClicked(int book, int chapter, int verse);
    void SearchTimeElapsed(qint64 msElapsed);
    void StatusMsgSet(QString msg);

public slots:
    void OnSearchButtonClicked(const QString &text);

private:
    SearchOptionsPanel *ui_SearchOptionsPanel;
    SearchResultArea *ui_SearchResultsArea;

    const QStringList *m_bookNames;
    QList<qbv::Module> *m_moduleData;
    QHash<QString, QLabel *> m_labels;
    QStringList m_results;
    QStringList m_refs;

    void OnRandomVerseRequested();
};

#endif // TABSEARCH_H

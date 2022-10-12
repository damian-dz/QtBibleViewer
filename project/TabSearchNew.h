#ifndef TABSEARCHNEW_H
#define TABSEARCHNEW_H

#include "AbstractTab.h"
#include "AppConfig.h"
#include "DatabaseService.h"
#include "SearchOptionsPanelNew.h"
#include "SearchResultArea.h"

class TabSearchNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabSearchNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);


    virtual void SetUiTexts() override;

private:
    SearchOptionsPanelNew *ui_SearchOptionsPanel;
    SearchResultArea *ui_SearchResultsArea;

    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;


    virtual void AddControls() override;
    virtual void ConnectSignals() override;
};

#endif // TABSEARCHNEW_H

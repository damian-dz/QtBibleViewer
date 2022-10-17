#ifndef TABCOMPARENEW_H
#define TABCOMPARENEW_H

#include "AbstractTab.h"
#include "NavPanel.h"

class TabCompareNew : public AbstractTab
{
public:
    explicit TabCompareNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void SetUiTexts() override;

private:
    NavPanel *ui_NavPanel;

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
};

#endif // TABCOMPARENEW_H

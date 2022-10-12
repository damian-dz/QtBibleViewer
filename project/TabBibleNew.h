#ifndef TABBIBLENEW_H
#define TABBIBLENEW_H

#include "AbstractTab.h"
#include "AppConfig.h"
#include "DatabaseService.h"
#include "NavPanel.h"
#include "PassageBrowserNew.h"

class TabBibleNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabBibleNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void SetUiTexts() override;

    void SetLocationFromConfig();
    void SaveLocationToConfig();
    void SetTabIndexFromConfig();
    void SaveTabIndexToConfig();

private:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    NavPanel *ui_NavPanel;
    QTabWidget *ui_TabWidget_Bibles;

    QList<PassageBrowserNew *> m_passageBrowsers;

    virtual void ConnectSignals() override;
    virtual void AddControls() override;

    void OnLocationChanged(qbv::Location loc);
    void OnTabChanged(int idx);
    void OnTabMoved(int from, int to);

signals:
    void AddNoteRequested(qbv::Location loc);
};

#endif // TABBIBLENEW_H

#ifndef TABCOMPARENEW_H
#define TABCOMPARENEW_H

#include "AbstractTab.h"
#include "CompareVerseBrowser.h"
#include "Location.h"
#include "NavPanel.h"

class TabCompareNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabCompareNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void SetUiTexts() override;

    void SetFontFromConfig();
    void ReloadBookNames();

private:
    NavPanel *ui_NavPanel;
    CompareVerseBrowser *ui_CompareVerseBrowser;

    virtual void ConnectSignals() override;
    virtual void AddControls() override;

    void OnLocationChanged(qbv::Location loc);

    void OnBibleNameClicked(const QString &name);



signals:
    void BibleNameClicked(QString name, qbv::Location loc);
};

#endif // TABCOMPARENEW_H

#ifndef TABSEARCHNEW_H
#define TABSEARCHNEW_H

#include "AbstractTab.h"
#include "AppConfig.h"
#include "DatabaseService.h"
#include "SearchOptionsPanelNew.h"
#include "SearchResultsBrowser.h"

class TabSearchNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabSearchNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);
    virtual void SetUiTexts() override;

private:
    QLabel *ui_Label_Enter;
    QLineEdit *ui_LineEdit_Search;
    QPushButton *ui_Button_Search;

    QLabel *ui_Label_Results;

    SearchResultsBrowser *ui_searchResultsBrowser;

    QPushButton *ui_Button_First;
    QPushButton *ui_Button_Prev;
    QPushButton *ui_Button_Next;
    QPushButton *ui_Button_Last;

    QSpinBox *ui_SpinBox_PageNum;

    QLabel *ui_Label_GoToPage;
    QPushButton *ui_Button_GoTo;
    QLabel *ui_Label_Of;
    QPushButton *ui_Button_RandomVerse;
    SearchResultsBrowser *ui_randomVerseBrowser;


    SearchOptionsPanelNew *ui_SearchOptionsPanel;


    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    int m_lastIdx = 0;
    QList<qbv::PassageWithLocation> m_results;

    virtual void AddControls() override;
    virtual void ConnectSignals() override;

    void OnSearchButtonClicked();

signals:
    void StatusMsgSet(QString msg);
};

#endif // TABSEARCHNEW_H

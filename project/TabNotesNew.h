#ifndef TABNOTESNEW_H
#define TABNOTESNEW_H

#include "AbstractTab.h"
#include "AppConfig.h"
#include "DatabaseService.h"
#include "Module.h"
#include "SearchResultsBrowser.h"

class TabNotesNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabNotesNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void SetUiTexts() override;
    virtual void Initialize() override;

    void AddToNotes(qbv::Location loc);

    void SetPassage(const QString &bookName, int chapter, int verse, const QString &scripture);
    void SetPassage(const QString &bookName, int chapter, int verse1, int verse2, const QString &scripture);

signals:

private:
    QLabel *ui_Label_References;
    QLabel *ui_Label_Passage;
    QLabel *ui_Label_Notes;

    QListWidget *ui_ListView_References;
    SearchResultsBrowser *ui_PassageBrowser;
    QTextEdit *ui_TextEdit_Notes;

    QPushButton *ui_Button_Save;
    QPushButton *ui_Button_Delete;

    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    QList<qbv::Location> m_locations;

    virtual void ConnectSignals() override;
    virtual void AddControls() override;

    void OnIndexChanged(int idx);
    void OnButtonSaveClicked();
    void OnButtonDeleteClicked();
};

#endif // TABNOTESNEW_H

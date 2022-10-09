#ifndef TABCOMPARE_H
#define TABCOMPARE_H

#include "AbstractTab.h"
#include "NavigationPanel.h"
#include "CompareVerseBrowser.h"
#include "Module.h"

class TabCompare : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabCompare(const QStringList &bookNames, const QStringList &shortBookNames, const QList<qbv::Module> &modules, const QSqlDatabase &verseData,
                        QWidget *parent = nullptr);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;

    void SetFont(const QFont& font);

signals:
   void StatusMsgSet(QString msg);
   void TranslationNameClicked(int idx, int book, int chapter, int verse);


public slots:

private:
    NavigationPanel *ui_NavigationPanel;
    CompareVerseBrowser *ui_CompareTextBrowser;

    const QStringList *m_pBookNames;
    const QList<qbv::Module> *m_pModules;
    const QSqlDatabase *m_pVerseData;


    void OnVerseChanged(int book, int chapter, int verse);
};

#endif // TABCOMPARE_H

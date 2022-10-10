#ifndef TABBIBLE_H
#define TABBIBLE_H

#include "AbstractTab.h"
#include "Module.h"
#include "ModuleTabWidget.h"
#include "NavigationPanel.h"
#include "NavPanel.h"
#include "CrossReferenceBox.h"
#include "FindOnPageBox.h"

class TabBible : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabBible(const QSqlDatabase &verseData, const QSqlDatabase &crossRefs, const QStringList &bookNames,
                      const QStringList &shortBookNames, const QList<qbv::Module>& modules, AppConfig *config,
                      QWidget *parent = nullptr);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;

//  void AddModule(const QString &name, const QString &filePath, bool hasOT = true, bool hasStrong = false);
    void AddModule(const qbv::Module &module);
    int GetSelectedBook() const;
    int GetSelectedChapter() const;
    int GetCurrentIndex() const;
    int GetModuleCount() const;
    int GetSelectedVerseFrom() const;
    int GetSelectedVerseTo() const;
    QByteArray GetSplitterLayout() const;
    void HighlightVerse(int verse);
    void LoadPassageInCurrentTab();
    void ReloadBookNames();
    void SelectAllText();
    void SetActiveModule(int idx, bool blockSignals = false);
    void SetNavigationLocation(int book, int chapter, bool load = false);
//  void SelectPassage(int book, int chapter, int verse1, int verse2, bool sendSignal = true);
    void SelectPassageFromConfig();
    void SetPassageBrowserFont(const QFont& font);
    void SetPassageBrowserHighlightColor(QColor &color);
    void UpdateFromConfig();

   QString getBookName(int idx) const ;
   void FindPhrase();

signals:
   void StatusMsgSet(QString msg);
   void AddToNotesRequested(qbv::Location loc);

public slots:
    void OnPassageChanged(int book, int chapter, int verse1, int verse2);
    void onRandomChapterRequested();
    void OnModuleTabWidgetTabChanged(int index);

private:
    QTabBar *ui_TabBar_Modules;
    ModuleTabWidget *ui_ModuleTabWidget;
    NavigationPanel *ui_NavigationPanel;
    NavPanel *ui_NavPanel;

    QVBoxLayout *ui_VerLayout_Modules;

    QSplitter *ui_Splitter_ModuleCrossRef;
    CrossReferenceBox *ui_CrossRefBox;
    FindOnPageBox *ui_FindOnPageBox;
    QLineEdit *ui_LineEdit_Find;
    QPushButton *ui_Button_Close;

    AppConfig *m_pConfig;
    const QStringList *m_pBookNames;
    const QStringList *m_pBookShortNames;
    const QList<qbv::Module> *m_pModules;
    const QSqlDatabase *m_pVerseData;
    const QSqlDatabase *m_pCrossRefs;
    QList<qbv::TabbedLocation> m_tabbedLocationHistory;

    void OnCrossReferenceRequested(int verse);
};

#endif // TABBIBLE_H

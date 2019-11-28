#ifndef TABBIBLE_H
#define TABBIBLE_H

#include "AbstractTab.h"
#include "ModuleData.h"
#include "ModuleTabWidget.h"
#include "BibleNavigationPanel.h"

class TabBible : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabBible(const QSqlDatabase &verseData, const QStringList &bookNames, const QList<ModuleData>& modules,
                      QWidget *parent = nullptr);

    virtual void connectSignals() override;
    virtual void addControls() override;
    virtual void setUiTexts() override;

    void addModule(const QString &name, const QString &filePath, bool hasOT = true, bool hasStrong = false);
    int getCurrentIndex() const;
    int getModuleCount() const;
    void selectModule(int idx);
    void selectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal = true);
    void setPassageTextBrowserFont(const QFont& font);

signals:

public slots:
    void onPassageChanged(int book, int chapter, int verseFrom, int verseTo);
    void onRandomChapterRequested();

private:
    QTabBar *ui_TabBar_Modules;
    ModuleTabWidget *ui_ModuleTabWidget;
    BibleNavigationPanel *ui_NavigationPanel;

    const QStringList *m_bookNames;
    const QList<ModuleData> *m_modules;
    const QSqlDatabase *m_verseData;
};

#endif // TABBIBLE_H

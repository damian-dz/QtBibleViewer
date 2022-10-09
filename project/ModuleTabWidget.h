#ifndef MODULETABWIDGET_H
#define MODULETABWIDGET_H

#include "precomp.h"

#include "TabbedLocation.h"
#include "Module.h"
#include "PassageBrowser.h"

class ModuleTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit ModuleTabWidget(const QList<qbv::Module> &modules, const QStringList &bookNames, AppConfig *config,
                             QWidget *parent = nullptr);

//  void AddModule(const QString &name, const QString &filePath, bool hasOT = true, bool hasStrong = false);
    void AddModule(const qbv::Module &module);
    qbv::Location GetLocation() const;
    bool HasModules() const;
    void HighlightText(const QString &text);
    void HighlightVerse(int verse);
    void LoadPassage(const QSqlDatabase &module);
    void LoadPassage(int book, int chapter, int verseFrom, int verseTo, const QSqlDatabase &module);
    void SetActiveModule(int idx, bool blockSignals = true);
    void SelectAllText();
    void SetHighlightColor(QColor &color);
    void SetLocation(qbv::Location location);
    void SetBiblePassageBrowserFont(const QFont &font);
    void UpdateFromConfig();
    void LoadPassageInCurrentTab();
private:
    AppConfig *m_pConfig;

    qbv::Location m_location;
    QList<PassageBrowser *> m_passageBrowers;
    const QList<qbv::Module> *m_pModules;
    const QStringList* m_pBookNames;

    void ConnectSignals();

    void OnModuleMoved(int from, int to);

    void OnTabCloseRequested(int index);

signals:
    void CrossReferenceRequested(int verse);
    void AddToFavoritesRequested(qbv::Location loc);
};

#endif // MODULETABWIDGET_H

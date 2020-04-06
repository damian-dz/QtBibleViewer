#ifndef MODULETABWIDGET_H
#define MODULETABWIDGET_H

#include "precomp.h"

#include "Location.h"
#include "BiblePassageBrowser.h"

class BibleModuleTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit BibleModuleTabWidget(QWidget *parent = nullptr);

    void addModule(const QString &name, const QString &filePath, bool hasOT = true, bool hasStrong = false);
    bool hasModules() const;
    void loadPassage(int book, int chapter, int verseFrom, int verseTo, const QSqlDatabase &module);
    void selectModule(int idx);
    void setPassageTextBrowserFont(const QFont &font);

private:
    QList<BiblePassageBrowser *> m_passageTextBrowers;
    qbv::Location m_location;

    void connectSignals();

private slots:
    void onModuleMoved(int from, int to);
    void onTabCloseRequested(int index);
};

#endif // MODULETABWIDGET_H

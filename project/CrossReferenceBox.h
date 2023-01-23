#ifndef CROSSREFERENCEBOX_H
#define CROSSREFERENCEBOX_H

#include "precomp.h"

#include "TabbedLocation.h"
#include "Module.h"
#include "SearchResultBrowser.h"

class CrossReferenceBox : public QWidget
{
    Q_OBJECT
public:
    CrossReferenceBox(const QList<qbv::Module>& modules, const QSqlDatabase &crossRefs, const QStringList &bookNames,
        QWidget *parent = nullptr);

    void LoadCrossReferences(int moduleIdx, int book, int chapter, int verse);
    void SetUiTexts();

private:
    QLabel *ui_Label_CrossRefBox;
    SearchResultBrowser *ui_CrossRefBrowser;

    const QList<qbv::Module> *m_pModules;
    const QSqlDatabase *m_pCrossRefs;
    const QStringList *m_pBookNames;

    void ParseMultiBookCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, int bookTo, int chapterTo, QStringList &results,
                                      QStringList &refs);
    void ParseMultiChapterCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, int chapterTo, QStringList &results,
                                         QStringList &refs);
    void ParseMultiVerseCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, QStringList &results, QStringList &refs);
    void ParseSingleVerseCrossReference(int moduleIdx, const QString &xRef, QStringList &results, QStringList &refs);
};

#endif // CROSSREFERENCEBOX_H

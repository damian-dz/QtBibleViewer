#ifndef COMPAREVERSEBROWSER_H
#define COMPAREVERSEBROWSER_H

#include "precomp.h"
#include "TabbedLocation.h"
#include "Module.h"

class CompareVerseBrowser: public QTextBrowser
{
    Q_OBJECT
public:
    explicit CompareVerseBrowser(QWidget *parent = nullptr);
    ~CompareVerseBrowser();

    void LoadNamesAndVerses(const QList<qbv::Module> &modules);
    void SetVerseLocation(int book, int chapter, int verse);

private:
    QTextTableFormat m_tableFormat;

    int m_book;
    int m_chapter;
    int m_verse;

    void OnAnchorClicked(const QUrl &link);

signals:
    void ReferenceClicked(int book, int chapter, int verse);
    void TranslationNameClicked(int idx, int book, int chapter, int verse);
};

#endif // COMPAREVERSEBROWSER_H

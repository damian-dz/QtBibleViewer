#ifndef SEARCHRESULTSAREA_H
#define SEARCHRESULTSAREA_H

#include "precomp.h"

#include "SearchResultBrowser.h"

class SearchResultArea : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit SearchResultArea(QWidget *parent = nullptr);
    ~SearchResultArea();

    void ConnectSignals();
    void DisplayResults();
    void SetFont(const QFont& font);
    void SetHighlightRegex(const QRegularExpression &regex);
    void setNumResultsPerPage(int numResults);
    void setRandomVerse(const QString &bookName, int chapter, int verse, const QString& scripture);
    void SetResults(const QStringList &results, const QStringList &refs);
    void SetUiTexts();

    void onRandomVerseButtonClicked();

private:
    void setResultsAndButtons();

    void onGoToButtonClicked();
    void onNextButtonClicked();
    void onPrevButtonClicked();

signals:
    void SearchButtonClicked(const QString &text);
    void RandomVerseRequested();
    void ReferenceClicked(int book, int chapter, int verse);

private:
    QLineEdit *ui_LineEdit_Search;
    QPushButton *ui_Button_Search;

    SearchResultBrowser *ui_SearchResultsBrowser;
    QPushButton *ui_Button_First;
    QPushButton *ui_Button_Prev;
    QPushButton *ui_Button_Next;
    QPushButton *ui_Button_Last;

    QSpinBox *ui_SpinBox_PageNum;
    QPushButton *ui_Button_GoTo;
    QPushButton *ui_Button_RandomVerse;
    SearchResultBrowser *ui_RandomVerseBrowser;

    QLabel *ui_Label_Enter;
    QLabel *ui_Label_Results;
    QLabel *ui_Label_GoToPage;
    QLabel *ui_Label_Of;

    QRegularExpression m_highlightRegex;
    int m_currentIdx;
    int m_numResPerPage;
    const QStringList *m_pRefs;
    const QStringList *m_pResults;

};

#endif // SEARCHRESULTSAREA_H

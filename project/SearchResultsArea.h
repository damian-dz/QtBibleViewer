#ifndef SEARCHRESULTSAREA_H
#define SEARCHRESULTSAREA_H

#include "precomp.h"

class SearchResultsArea : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit SearchResultsArea(QWidget *parent = nullptr);

    void connectSignals();
    void displayResults();
    void setFonts(const QFont& font);
    void setUiTexts();
    void setNumResultsPerPage(int numResults);
    void setResults(const QStringList &results, const QStringList &refs);

signals:
    void searchButtonClicked(const QString &text);

private:
    QLineEdit *ui_LineEdit_Search;
    QPushButton *ui_Button_Search;

    QTextBrowser *ui_TextBrowser_Results;
    QPushButton *ui_Button_First;
    QPushButton *ui_Button_Prev;
    QPushButton *ui_Button_Next;
    QPushButton *ui_Button_Last;

    QSpinBox *ui_SpinBox_PageNum;
    QPushButton *ui_Button_GoTo;
    QPushButton *ui_Button_RandomVerse;
    QTextBrowser *ui_TextBrowser_RandomVerse;

    QLabel *ui_Label_Enter;
    QLabel *ui_Label_Results;
    QLabel *ui_Label_GoToPage;
    QLabel *ui_Label_Of;

    int m_currentIdx;
    int m_numResPerPage;
    const QStringList *m_results;
    const QStringList *m_refs;

private slots:
    void onNextButtonClicked();
    void onPrevButtonClicked();
};

#endif // SEARCHRESULTSAREA_H

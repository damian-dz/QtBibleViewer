#ifndef SEARCHRESULTSAREA_H
#define SEARCHRESULTSAREA_H

#include "precomp.h"

#include "SearchResultsBrowser.h"

class SearchResultsArea : public QGridLayout
{
    Q_OBJECT
public:
    explicit SearchResultsArea(QWidget *parent = nullptr);

    void SetUiTexts();

private:
    QLabel *ui_Label_Enter;
    QLineEdit *ui_LineEdit_Search;
    QPushButton *ui_Button_Search;

    QLabel *ui_Label_Results;

    SearchResultsBrowser *ui_searchResultsBrowser;

};

#endif // SEARCHRESULTSAREA_H

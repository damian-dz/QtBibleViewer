#include "SearchResultsArea.h"

SearchResultsArea::SearchResultsArea(QWidget *parent) : QGridLayout(parent)
{
    ui_Label_Enter = new QLabel;

    ui_LineEdit_Search = new QLineEdit;
    ui_LineEdit_Search->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_LineEdit_Search->setFixedHeight(21);
    ui_LineEdit_Search->setFocus();

    ui_Button_Search = new QPushButton;
    ui_Button_Search->setDisabled(true);
    ui_Button_Search->setFixedHeight(23);

    ui_Label_Results = new QLabel;

    // ui_searchResultsBrowser = new SearchResultsBrowser;

    QGridLayout::addWidget(ui_Label_Enter, 0, 0);
    QGridLayout::addWidget(ui_LineEdit_Search, 1, 0);
    QGridLayout::addWidget(ui_Button_Search, 1, 1);
    QGridLayout::addWidget(ui_Label_Results, 2, 0);
    QGridLayout::addWidget(ui_searchResultsBrowser, 3, 0, 1, 2);

    SetUiTexts();
}

void SearchResultsArea::SetUiTexts()
{
    ui_Label_Enter->setText(tr("Enter a Word or a Phrase:"));
    ui_Button_Search->setText(tr("Search"));
    ui_Label_Results->setText(tr("Results:"));
}

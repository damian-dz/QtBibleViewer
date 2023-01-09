#include "TabDictionary.h"

TabDictionary::TabDictionary(QWidget *parent) : AbstractTab(parent)
{

}

void TabDictionary::ConnectSignals()
{

}

void TabDictionary::AddControls()
{
    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->setSpacing(5);
    mainHorLayout->setContentsMargins(5, 5, 5, 5);

    QVBoxLayout *dictionariesVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(dictionariesVerLayout);

    QLabel *dictionariesLabel = new QLabel("Dictionaries:");
    dictionariesVerLayout->addWidget(dictionariesLabel);

    QListWidget *dictionariesListWidget = new QListWidget;
    dictionariesListWidget->setMaximumWidth(200);
    dictionariesVerLayout->addWidget(dictionariesListWidget);

    QVBoxLayout *numberEntriesVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(numberEntriesVerLayout);
    QLabel *numberLabel = new QLabel("Number:");
    numberEntriesVerLayout->addWidget(numberLabel);

    QLineEdit *numberLineEdit = new QLineEdit;
    numberLineEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    numberLineEdit->setMaximumWidth(180);
    numberEntriesVerLayout->addWidget(numberLineEdit);
    numberLineEdit->setFocus();
   // QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
  //  QValidator *validator = new QRegExpValidator(regex, this);
   // numberLineEdit->setValidator(validator);

    QLabel *allEntriesLabel = new QLabel("All Entries:");
    numberEntriesVerLayout->addWidget(allEntriesLabel);

    auto ui_Dic_ListWidget_AllEntries = new QListWidget;
    ui_Dic_ListWidget_AllEntries->setMaximumWidth(180);
    numberEntriesVerLayout->addWidget(ui_Dic_ListWidget_AllEntries);

    QVBoxLayout *definitionVerLayout = new QVBoxLayout;
    mainHorLayout->addLayout(definitionVerLayout);

    QLabel *definitionLabel = new QLabel("Definition:");
    definitionVerLayout->addWidget(definitionLabel);

    auto ui_Dic_TextBrowser_Definition = new QTextBrowser;
    ui_Dic_TextBrowser_Definition->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_Dic_TextBrowser_Definition->setOpenLinks(false);
    definitionVerLayout->addWidget(ui_Dic_TextBrowser_Definition);
    QWidget::setLayout(mainHorLayout);
}

void TabDictionary::SetUiTexts()
{

}

#include "TabNotesNew.h"

#include "PassageBrowser.h"

TabNotesNew::TabNotesNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(config, databaseService, parent)
{

}

void TabNotesNew::AddControls()
{
    ui_Label_References = new QLabel;
    ui_ListWidget_References = new QListWidget;
    ui_ListWidget_References->setMaximumWidth(200);

    ui_Label_Passage = new QLabel;
    ui_PassageBrowser = new SearchResultsBrowser(*m_pConfig, *m_pDatabaseService);
    ui_PassageBrowser->SetIncludeResultNumber(false);
    ui_PassageBrowser->SetShouldHighlightBackground(false);

    ui_Label_Notes = new QLabel;
    ui_TextEdit_Notes = new QTextEdit;

    ui_Button_Save = new QPushButton;
    ui_Button_Save->setMaximumWidth(100);

    ui_Button_Delete = new QPushButton;
    ui_Button_Delete->setMaximumWidth(100);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addWidget(ui_Label_References, 0, 0);
    mainLayout->addWidget(ui_ListWidget_References, 1, 0, 4, 1);
    mainLayout->addWidget(ui_Label_Passage, 0, 1);
    mainLayout->addWidget(ui_PassageBrowser, 1, 1, 1, 5);
    mainLayout->addWidget(ui_Label_Notes, 2, 1);
    mainLayout->addWidget(ui_TextEdit_Notes, 3, 1, 1, 5);
    mainLayout->addWidget(ui_Button_Save, 4, 4);
    mainLayout->addWidget(ui_Button_Delete, 4, 5);

    QWidget::setLayout(mainLayout);
}




void TabNotesNew::ConnectSignals()
{
    QObject::connect(ui_Button_Save, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnButtonSaveClicked(); } );
    QObject::connect(ui_Button_Delete, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnButtonDeleteClicked(); } );
    QObject::connect(ui_ListWidget_References, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int idx) { OnIndexChanged(idx); } );
}

void TabNotesNew::SetUiTexts()
{
    ui_Label_References->setText(tr("References:"));
    ui_Label_Passage->setText(tr("Passage:"));
    ui_Label_Notes->setText(tr("Note:"));

    ui_Button_Save->setText(tr("Save"));
    ui_Button_Delete->setText(tr("Delete"));
}

void TabNotesNew::Initialize()
{
    AbstractTab::Initialize();
    m_pDatabaseService->CreateUserDir();
    m_pDatabaseService->OpenUserNotesDb();

    m_locations = m_pDatabaseService->NotesLocations();

    QStringList noteList;
    for (const qbv::Location &loc : m_locations) {
        QString passageId = m_pDatabaseService->PassageIdForLocation(loc);
        noteList << passageId;
    }

    if (noteList.count() > 0) {
        ui_ListWidget_References->addItems(noteList);
        ui_ListWidget_References->setCurrentRow(0);
    }
}

void TabNotesNew::AddToNotes(qbv::Location loc)
{
    ui_ListWidget_References->blockSignals(true);
    m_pDatabaseService->AddToNotes(loc);
    m_locations.append(loc);
    std::sort(m_locations.begin(), m_locations.end());
    QString passageId = m_pDatabaseService->PassageIdForLocation(loc);
    int idx = m_locations.indexOf(loc);
    ui_ListWidget_References->insertItem(idx, passageId);
    ui_ListWidget_References->blockSignals(false);
    ui_ListWidget_References->setCurrentRow(idx);
    ui_TextEdit_Notes->setFocus();
}


void TabNotesNew::OnIndexChanged(int idx)
{
    qbv::Location loc = m_locations[idx];
    QString note = m_pDatabaseService->Note(loc);
    ui_TextEdit_Notes->setPlainText(note);

    QStringList passage = m_pDatabaseService->GetScriptures(0, loc);
    ui_PassageBrowser->SetResult( qbv::PassageWithLocation { passage.join(" "), loc  } );
}

void TabNotesNew::OnButtonSaveClicked()
{
    int idx = ui_ListWidget_References->currentRow();
    qbv::Location loc = m_locations[idx];
    QString note = ui_TextEdit_Notes->toPlainText();
    m_pDatabaseService->SaveNote(note, loc);
}

void TabNotesNew::OnButtonDeleteClicked()
{

}

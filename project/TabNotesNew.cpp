#include "TabNotesNew.h"

#include "PassageBrowser.h"

TabNotesNew::TabNotesNew(qbv::DatabaseService &databaseService, QWidget *parent) :
    AbstractTab(parent),
    m_pDatabaseService(&databaseService)
{

}

void TabNotesNew::AddControls()
{
    ui_Label_References = new QLabel;
    ui_ListView_References = new QListWidget;
    ui_ListView_References->setMaximumWidth(200);

    ui_Label_Passage = new QLabel;
    ui_PassageBrowser = new SearchResultBrowser;
    ui_PassageBrowser->SetIncludeResultNumber(false);

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
    mainLayout->addWidget(ui_ListView_References, 1, 0, 4, 1);
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
    QObject::connect(ui_ListView_References, QOverload<int>::of(&QListWidget::currentRowChanged),
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
        ui_ListView_References->addItems(noteList);
        ui_ListView_References->setCurrentRow(0);
    }
}


void TabNotesNew::AddToNotes(qbv::Location loc)
{
    ui_ListView_References->blockSignals(true);
    m_pDatabaseService->AddToNotes(loc);
    m_locations.append(loc);
    std::sort(m_locations.begin(), m_locations.end());
    QString passageId = m_pDatabaseService->PassageIdForLocation(loc);
    int idx = m_locations.indexOf(loc);
    ui_ListView_References->insertItem(idx, passageId);
    ui_ListView_References->blockSignals(false);
    ui_ListView_References->setCurrentRow(idx);
    ui_TextEdit_Notes->setFocus();
}

void TabNotesNew::SetPassage(const QString &bookName, int chapter, int verse, const QString &scripture)
{
    ui_PassageBrowser->SetResult(scripture, QString("<b>—%1 %2:%3</b>")
                                 .arg(bookName, QString::number(chapter), QString::number(verse)));
}

void TabNotesNew::SetPassage(const QString &bookName, int chapter, int verse1, int verse2, const QString &scripture)
{
    ui_PassageBrowser->SetResult(scripture, QString("<b>—%1 %2:%3-%4</b>")
                                 .arg(bookName, QString::number(chapter),
                                      QString::number(verse1), QString::number(verse2)));
}

void TabNotesNew::OnIndexChanged(int idx)
{
    auto loc = m_locations[idx];
    QString note = m_pDatabaseService->Note(loc);
    ui_TextEdit_Notes->setPlainText(note);
}

void TabNotesNew::OnButtonSaveClicked()
{
    int idx = ui_ListView_References->currentRow();
    qbv::Location loc = m_locations[idx];
    QString note = ui_TextEdit_Notes->toPlainText();
    m_pDatabaseService->SaveNote(note, loc);
}

void TabNotesNew::OnButtonDeleteClicked()
{

}

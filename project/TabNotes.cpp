#include "TabNotes.h"

#include "PassageBrowser.h"

TabNotes::TabNotes(const QString &filename, const QList<qbv::Module>& modules, const QStringList &bookNames,
                   QWidget *parent) :
    AbstractTab(parent),
    m_filename(filename),
    m_pModules(&modules),
    m_pBookNames(&bookNames)
{

}

void TabNotes::CreateFavDatabase(const QString &filename)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filename);
    if (m_db.open()) {
        QSqlQuery query(m_db);
        query.exec("CREATE TABLE Notes (Book INT, Chapter INT, Verse1 INT, Verse2 INT, Note TEXT)");
        query.exec("CREATE UNIQUE INDEX `fav_key` ON `Notes` "
                   "(`Book` ASC, `Chapter` ASC, `VerseFirst` ASC, `VerseLast` ASC)");
    }
}

void TabNotes::CurrentIndexChanged(int idx)
{
    qbv::Location loc = m_locations[idx];
    QSqlQuery query(m_db);
    query.prepare("SELECT Note FROM Notes "
                  "WHERE Book=? AND Chapter=? AND Verse1=? AND Verse2=?");
    query.addBindValue(loc.book, QSql::Out);
    query.addBindValue(loc.chapter, QSql::Out);
    query.addBindValue(loc.verse, QSql::Out);
    query.addBindValue(loc.endVerse, QSql::Out);
    if (query.exec() && query.next()) {
        QString note = query.record().value(0).toString();
        ui_TextEdit_Notes->setPlainText(note);

        QStringList passage = m_pModules->at(0).GetPassage(loc);
       // passage.join(" ")
        QString bookName = m_pBookNames->at(loc.book - 1);
        if (loc.verse == loc.endVerse) {
            SetPassage(bookName, loc.chapter, loc.verse, passage.join(" "));
        } else {
            SetPassage(bookName, loc.chapter, loc.verse, loc.endVerse, passage.join(" "));
        }
    }
}

void TabNotes::SaveButtonClicked()
{
    int idx = ui_ListView_References->currentRow();
    qbv::Location loc = m_locations[idx];
    QString note = ui_TextEdit_Notes->toPlainText();
    QSqlQuery query(m_db);
    query.prepare("UPDATE Notes SET Note=? "
                  "WHERE Book=? AND Chapter=? AND Verse1=? AND Verse2=?");
    query.addBindValue(note);
    query.addBindValue(loc.book);
    query.addBindValue(loc.chapter);
    query.addBindValue(loc.verse);
    query.addBindValue(loc.endVerse);
    query.exec();
}

void TabNotes::ConnectSignals()
{

}

void TabNotes::AddControls()
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    ui_Label_References = new QLabel;
    ui_ListView_References = new QListWidget;

    mainLayout->addWidget(ui_Label_References, 0, 0);
    mainLayout->addWidget(ui_ListView_References, 1, 0, 4, 1);
    ui_ListView_References->setMaximumWidth(200);

    ui_Label_Passage = new QLabel;
    ui_PassageBrowser = new SearchResultBrowser;
    ui_PassageBrowser->SetIncludeResultNumber(false);

    ui_Label_Notes = new QLabel;
    ui_TextEdit_Notes = new QTextEdit;

    mainLayout->addWidget(ui_Label_Passage, 0, 1);
    mainLayout->addWidget(ui_PassageBrowser, 1, 1, 1, 5);
    mainLayout->addWidget(ui_Label_Notes, 2, 1);
    mainLayout->addWidget(ui_TextEdit_Notes, 3, 1, 1, 5);

    QPushButton *saveButton = new QPushButton(tr("Save"));
    saveButton->setMaximumWidth(100);



    QPushButton *deleteButton = new QPushButton(tr("Delete"));
    deleteButton->setMaximumWidth(100);

    mainLayout->addWidget(saveButton, 4, 4);
    mainLayout->addWidget(deleteButton, 4, 5);

    QWidget::setLayout(mainLayout);

    QObject::connect(saveButton, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { SaveButtonClicked(); } );
    QObject::connect(ui_ListView_References, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int row) { CurrentIndexChanged(row); } );
}

void TabNotes::SetUiTexts()
{
    ui_Label_References->setText(tr("References:"));
    ui_Label_Passage->setText(tr("Passage:"));
    ui_Label_Notes->setText(tr("Note:"));
}

void TabNotes::Initialize()
{
    AbstractTab::Initialize();
    if (!QDir().exists(m_filename)) {
        CreateFavDatabase(m_filename);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(m_filename);
        m_db.open();
    }
    QStringList noteList;

    QSqlQuery query(m_db);
    query.exec("SELECT Book, Chapter, Verse1, Verse2 FROM Notes");
    while (query.next()) {
        QSqlRecord record = query.record();
        int book = record.value(0).toInt();
        int chapter = record.value(1).toInt();
        int verse1 = record.value(2).toInt();
        int verse2 = record.value(3).toInt();
        m_locations.append(qbv::Location { book, chapter, verse1, verse2 } );

        QString passageId = m_pBookNames->at(book - 1) + " " + QString::number(chapter) +
                ":" + QString::number(verse1);
        if (verse1 != verse2) {
            passageId += "-" % QString::number(verse2);
        }
        noteList << passageId;

    }
    if (noteList.count() > 0) {
        ui_ListView_References->addItems(noteList);
        ui_ListView_References->setCurrentRow(0);
    }
}

void TabNotes::AddToNotes(qbv::Location loc)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Notes (Book, Chapter, Verse1, Verse2) VALUES (?, ?, ?, ?)");
    query.addBindValue(loc.book);
    query.addBindValue(loc.chapter);
    query.addBindValue(loc.verse);
    query.addBindValue(loc.endVerse);
    query.exec();

    m_locations.append(loc);
    QString passageId = m_pBookNames->at(loc.book - 1) + " " + QString::number(loc.chapter) +
            ":" + QString::number(loc.verse);
    if (loc.verse != loc.endVerse) {
        passageId += "-" % QString::number(loc.endVerse);
    }
    ui_ListView_References->addItem(passageId);
    ui_ListView_References->setCurrentRow(ui_ListView_References->count() - 1);

    ui_TextEdit_Notes->setFocus();
}

void TabNotes::SetPassage(const QString &bookName, int chapter, int verse, const QString &scripture)
{
    ui_PassageBrowser->SetResult(scripture, QString("<b>—%1 %2:%3</b>")
                                 .arg(bookName, QString::number(chapter), QString::number(verse)));
}

void TabNotes::SetPassage(const QString &bookName, int chapter, int verse1, int verse2, const QString &scripture)
{
    ui_PassageBrowser->SetResult(scripture, QString("<b>—%1 %2:%3-%4</b>")
                                 .arg(bookName, QString::number(chapter),
                                      QString::number(verse1), QString::number(verse2)));
}

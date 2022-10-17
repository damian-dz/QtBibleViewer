#include "NavPanel.h"

NavPanel::NavPanel(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    QGridLayout(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService),
    m_moveByVerse(false)
{
    AddControls();
    ConnectSignals();
    SetUiTexts();


}

void NavPanel::AddControls()
{
    ui_Label_Book = new QLabel;
    ui_ListWidget_Book = new QListWidget;
    ui_ListWidget_Book->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_Book->setMaximumSize(QSize(155, 16777215));
    ui_ListWidget_Book->addItems(m_pDatabaseService->BookNames());

    ui_Label_Chapter = new QLabel;
    ui_ListWidget_Chapter = new QListWidget;
    ui_ListWidget_Chapter->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_Chapter->setMaximumSize(QSize(60, 16777215));

    ui_LineEdit_MatchPassage = new QLineEdit;
    ui_LineEdit_MatchPassage->setPlaceholderText(tr("Book chapter:from-to (eg. Gen 3:1-4)"));
    ui_LineEdit_MatchPassage->setFixedHeight(23);

    ui_Label_Verses = new QLabel;
    ui_ListWidget_VerseFrom = new QListWidget;
    ui_ListWidget_VerseFrom->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_VerseFrom->setMaximumSize(QSize(55, 16777215));
    ui_ListWidget_VerseFrom->setMinimumSize(QSize(55, 20));

    ui_ListWidget_VerseTo = new QListWidget;
    ui_ListWidget_VerseTo->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_VerseTo->setMaximumSize(QSize(55, 16777215));
    ui_ListWidget_VerseTo->setMinimumSize(QSize(55, 20));

    ui_Button_Random = new QPushButton;
    ui_Button_Random->setMaximumSize(QSize(55, 30));

    ui_Button_Prev = new QPushButton("<<");
    ui_Button_Prev->setMaximumWidth(25);

    ui_Button_Next = new QPushButton(">>");
    ui_Button_Next->setMaximumWidth(25);

    QHBoxLayout *prevNextHorLayout = new QHBoxLayout;
    prevNextHorLayout->addWidget(ui_Button_Prev);
    prevNextHorLayout->addWidget(ui_Button_Next);

    QGridLayout::addWidget(ui_Label_Book, 0, 0);
    QGridLayout::addWidget(ui_ListWidget_Book, 1, 0, 3, 1);
    QGridLayout::addWidget(ui_Label_Chapter, 0, 1);
    QGridLayout::addWidget(ui_ListWidget_Chapter, 1, 1, 3, 1);
    QGridLayout::addWidget(ui_LineEdit_MatchPassage, 4, 0, 1, 2);
    QGridLayout::addWidget(ui_Label_Verses, 0, 2);
    QGridLayout::addWidget(ui_ListWidget_VerseFrom, 1, 2);
    QGridLayout::addWidget(ui_ListWidget_VerseTo, 2, 2);
    QGridLayout::addWidget(ui_Button_Random, 3, 2);
    QGridLayout::addLayout(prevNextHorLayout, 4, 2);
}

void NavPanel::ConnectSignals()
{
    QObject::connect(ui_ListWidget_Book, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int idx) { OnCurrentBookChanged(idx + 1); });
    QObject::connect(ui_ListWidget_Chapter, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int idx) { OnCurrentChapterChanged(idx + 1); });
    QObject::connect(ui_ListWidget_VerseFrom, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int idx) { OnCurrentVerseFromChanged(idx + 1); });
    QObject::connect(ui_ListWidget_VerseTo, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int idx) { OnCurrentVerseToChanged(idx + 1); });

    QObject::connect(ui_Button_Random, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnRandomButtonClicked(); });
    QObject::connect(ui_Button_Prev, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnNavButtonClicked(-1); });
    QObject::connect(ui_Button_Next, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnNavButtonClicked(+1); });
}

void NavPanel::SetUiTexts()
{
    ui_Label_Book->setText(tr("Book:"));
    ui_Label_Chapter->setText(tr("Chapter:"));
    ui_Label_Verses->setText(tr("Verses:"));

    ui_Button_Random->setText(tr("Random"));
}

QStringList NavPanel::GenerateRange(int min, int max)
{
    QStringList range;
    for (int i = min; i <= max; ++i) {
        range << QString::number(i);
    }
    return range;
}


void NavPanel::OnCurrentBookChanged(int book)
{
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_Chapter->clear();
    int numChapters = m_pDatabaseService->NumChapters(book);
    ui_ListWidget_Chapter->addItems(GenerateRange(1, numChapters));
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_Chapter->setCurrentRow(0);
}

void NavPanel::OnCurrentChapterChanged(int chapter)
{
    int numVerses = m_pDatabaseService->NumVerses(ui_ListWidget_Book->currentRow() + 1, chapter);
    QStringList verseNumbers = GenerateRange(1, numVerses);

    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);
    ui_ListWidget_VerseFrom->clear();
    ui_ListWidget_VerseFrom->addItems(verseNumbers);
    ui_ListWidget_VerseFrom->setCurrentRow(0);

    ui_ListWidget_VerseTo->clear();
    ui_ListWidget_VerseTo->addItems(verseNumbers);
    ui_ListWidget_VerseTo->blockSignals(false);
    ui_ListWidget_VerseTo->setCurrentRow(numVerses - 1);
    ui_ListWidget_VerseFrom->blockSignals(false);
}

void NavPanel::OnCurrentVerseFromChanged(int verse, bool emitSignals)
{
    if (verse - 1 > ui_ListWidget_VerseTo->currentRow()) {
        ui_ListWidget_VerseTo->blockSignals(true);
        ui_ListWidget_VerseTo->setCurrentRow(verse - 1);
        ui_ListWidget_VerseTo->blockSignals(false);
    }
    OnLocationChanged(emitSignals);
}

void NavPanel::OnCurrentVerseToChanged(int verse, bool emitSignals)
{
    if (verse - 1 < ui_ListWidget_VerseFrom->currentRow()) {
        ui_ListWidget_VerseFrom->blockSignals(true);
        ui_ListWidget_VerseFrom->setCurrentRow(verse - 1);
        ui_ListWidget_VerseFrom->blockSignals(false);
    }
    OnLocationChanged(emitSignals);
}

void NavPanel::OnRandomButtonClicked()
{
    QRandomGenerator randGen(quint32(QDateTime::currentDateTime().time().msec()));
    if (m_moveByVerse) {
        //SelectPassageByVerseId(randGen.bounded(1, 31102));
    } else {
        // SelectPassageByChapterId(randGen.bounded(1, 1189));
        int id = randGen.bounded(1, 1189);
        auto loc = m_pDatabaseService->LocationForChapterId(id);
        SetLocation(loc, true);
    }
}

void NavPanel::OnNavButtonClicked(int direction)
{
    int chapterId = m_pDatabaseService->ChapterIdForLocation(GetLocation()) + direction;
    auto loc = m_pDatabaseService->LocationForChapterId(chapterId);
    SetLocation(loc, true);

}

void NavPanel::OnNextButtonClicked()
{

}

void NavPanel::OnLocationChanged(bool emitSignals)
{
    int book = ui_ListWidget_Book->currentRow() + 1;
    int chapter = ui_ListWidget_Chapter->currentRow() + 1;
    int verse1 = ui_ListWidget_VerseFrom->currentRow() + 1;
    int verse2 = ui_ListWidget_VerseTo->currentRow() + 1;
    if (m_moveByVerse) {
        if (emitSignals) {
            emit LocationChanged(qbv::Location { book, chapter, verse1, verse1 } );
        }
        m_verseId =  m_pDatabaseService->VerseId(book, chapter, verse1);
        ui_Button_Next->setEnabled(m_verseId < 31102);
        ui_Button_Prev->setEnabled(m_verseId > 1);
    } else {
        if (emitSignals) {
            emit LocationChanged(qbv::Location { book, chapter, verse1, verse2 } );
        }
        m_chapterId = m_pDatabaseService->ChapterId(book, chapter);
        ui_Button_Next->setEnabled(m_chapterId < 1189);
        ui_Button_Prev->setEnabled(m_chapterId > 1);
    }

}

void NavPanel::UpdateLocationInConfig(qbv::Location loc)
{

}

qbv::Location NavPanel::GetLocation() const
{
    return qbv::Location {
        ui_ListWidget_Book->currentRow() + 1,
        ui_ListWidget_Chapter->currentRow() + 1,
        ui_ListWidget_VerseFrom->currentRow() + 1,
        ui_ListWidget_VerseTo->currentRow() + 1
    };
}

void NavPanel::SetLocation(qbv::Location loc, bool emitSignal)
{
    qDebug() << "NavPanel::SetLocation";

    ui_ListWidget_Book->blockSignals(true);
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);

    ui_ListWidget_Book->setCurrentRow(loc.book - 1);
    ui_ListWidget_Chapter->clear();
    int numChapters = m_pDatabaseService->NumChapters(loc.book);
    ui_ListWidget_Chapter->addItems(GenerateRange(1, numChapters));
    ui_ListWidget_Chapter->setCurrentRow(loc.chapter - 1);

    int numVerses = m_pDatabaseService->NumVerses(loc.book, loc.chapter);
    QStringList verseNumbers = GenerateRange(1, numVerses);

    ui_ListWidget_VerseFrom->clear();
    ui_ListWidget_VerseFrom->addItems(verseNumbers);
    ui_ListWidget_VerseFrom->setCurrentRow(loc.verse1 - 1);

    ui_ListWidget_VerseTo->clear();
    ui_ListWidget_VerseTo->addItems(verseNumbers);
    ui_ListWidget_VerseTo->setCurrentRow(loc.verse2 - 1);

    ui_ListWidget_Book->blockSignals(false);
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_VerseFrom->blockSignals(false);
    ui_ListWidget_VerseTo->blockSignals(false);

    int chapterId = m_pDatabaseService->ChapterIdForLocation(loc);
    ui_Button_Next->setEnabled(chapterId < 1189);
    ui_Button_Prev->setEnabled(chapterId > 1);

    UpdateLocationInConfig(loc);
    if (emitSignal) {
        emit LocationChanged(loc);
    }
}

int NavPanel::GetChapterId() const
{
    return -1;
}

void NavPanel::HideVerseTo()
{
    ui_ListWidget_VerseTo->hide();
}

#include "NavigationPanel.h"

#include "SearchEngine.h"

NavigationPanel::NavigationPanel(const QSqlDatabase &verseData, const QStringList &bookNames,
                                 const QStringList &bookAbbreviations,
                                 QWidget *parent) :
    QHBoxLayout(parent),
    m_bookNames(&bookNames),
    m_bookAbbreviations(&bookAbbreviations),
    m_chapterId(0),
    m_verseId(0),
    m_pVerseData(&verseData)
{
    ui_Label_Book = new QLabel;

    ui_ListWidget_Book = new QListWidget;
    ui_ListWidget_Book->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_Book->setMaximumSize(QSize(155, 16777215));
    ui_ListWidget_Book->addItems(bookNames);

    ui_Label_Chapter = new QLabel;

    ui_ListWidget_Chapter = new QListWidget;
    ui_ListWidget_Chapter->setFont(QFont(QApplication::font().family(), 10));
    ui_ListWidget_Chapter->setMaximumSize(QSize(60, 16777215));

    ui_LineEdit_MatchPassage = new QLineEdit;
    ui_LineEdit_MatchPassage->setPlaceholderText(tr("Book chapter:from-to (eg. Gen 3:1-4)"));
    ui_LineEdit_MatchPassage->setFixedHeight(23);

    QVBoxLayout *bookVerLayout = new QVBoxLayout;
    bookVerLayout->addWidget(ui_Label_Book);
    bookVerLayout->addWidget(ui_ListWidget_Book);

    QVBoxLayout *chapterVerLayout = new QVBoxLayout;
    chapterVerLayout->addWidget(ui_Label_Chapter);
    chapterVerLayout->addWidget(ui_ListWidget_Chapter);

    QGridLayout *bookChapterGridLayout = new QGridLayout;
    bookChapterGridLayout->addLayout(bookVerLayout, 0, 0);
    bookChapterGridLayout->addLayout(chapterVerLayout, 0, 1);
    bookChapterGridLayout->addWidget(ui_LineEdit_MatchPassage, 1, 0, 1, 2);

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

    QVBoxLayout *versesVerLayout = new QVBoxLayout;
    versesVerLayout->addWidget(ui_Label_Verses);
    versesVerLayout->addWidget(ui_ListWidget_VerseFrom);
    versesVerLayout->addWidget(ui_ListWidget_VerseTo);

    versesVerLayout->addWidget(ui_Button_Random);
    versesVerLayout->addLayout(prevNextHorLayout);

    QHBoxLayout::setSpacing(5);
    QHBoxLayout::setContentsMargins(5, 5, 5, 5);
    QHBoxLayout::addLayout(bookChapterGridLayout);
    QHBoxLayout::addLayout(versesVerLayout);

    ConnectSignals();
    // setUiTexts();
}

void NavigationPanel::ConnectSignals()
{
    QObject::connect(ui_ListWidget_Book, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { OnCurrentBookChanged(currentRow + 1); });
    QObject::connect(ui_ListWidget_Chapter, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { OnCurrentChapterChanged(currentRow + 1); });
    QObject::connect(ui_ListWidget_VerseFrom, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { OnCurrentVerseFromChanged(currentRow + 1); });
    QObject::connect(ui_ListWidget_VerseTo, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { OnCurrentVerseToChanged(currentRow + 1); });
    QObject::connect(ui_Button_Random, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnRandomButtonClicked(); });
    QObject::connect(ui_Button_Prev, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnPrevButtonClicked(); });
    QObject::connect(ui_Button_Next, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { OnNextButtonClicked(); });

    QObject::connect(ui_LineEdit_MatchPassage, QOverload<>::of(&QLineEdit::returnPressed),
                     [=] { OnMatchPassage(); });
}

QStringList NavigationPanel::GenerateRange(int min, int max)
{
    QStringList range;
    for (int i = min; i <= max; ++i) {
        range << QString::number(i);
    }
    return range;
}

int NavigationPanel::GetChapterCount(int book)
{
    qDebug() << "NavigationPanel::GetChapterCount(int)";
    QString command = "SELECT MAX(Chapter) FROM ChapterMap WHERE Book = " % QString::number(book);
    QSqlQuery query(*m_pVerseData);
    int chapterCount = -1;
    if (query.exec(command)) {
        if (query.next()) {
            chapterCount = query.record().value(0).toInt();
        }
    }
    return chapterCount;
}

qbv::Location NavigationPanel::GetLocation() const
{
    int book = GetSelectedBook();
    int chapter = GetSelectedChapter();
    int verseFrom = GetSelectedVerseFrom();
    int verseTo = GetSelectedVerseTo();
    return qbv::Location{ book, chapter, verseFrom, verseTo };
}

int NavigationPanel::GetNumVerses(int book, int chapter)
{
    QString command = "SELECT NumVerses FROM ChapterMap WHERE Book = " %
            QString::number(book) % " AND Chapter = " % QString::number(chapter);
    QSqlQuery query(*m_pVerseData);
    int verseCount = -1;
    if (query.exec(command)) {
        if (query.next()) {
            verseCount = query.record().value(0).toInt();
        }
    }
    return verseCount;
}

int NavigationPanel::GetChapterId(int book, int chapter)
{
    QString command = "SELECT Id FROM ChapterMap WHERE Book = " %
            QString::number(book) % " AND Chapter = " % QString::number(chapter);
    QSqlQuery query(*m_pVerseData);
    int chapterId = 0;
    if (query.exec(command)) {
        if (query.next()) {
            chapterId = query.record().value(0).toInt();
        }
    }
    return chapterId;
}

int NavigationPanel::GetVerseId(int book, int chapter, int verseFrom)
{
    QString command = "SELECT Id FROM VerseMap WHERE Book = " %
            QString::number(book) % " AND Chapter = " % QString::number(chapter)
            % " AND Verse = " % QString::number(verseFrom);
    QSqlQuery query(*m_pVerseData);
    int verseId = 0;
    if (query.exec(command)) {
        if (query.next()) {
            verseId = query.record().value(0).toInt();
        }
    }
    return verseId;
}

int NavigationPanel::GetSelectedBook() const
{
    return ui_ListWidget_Book->currentRow() + 1;
}

int NavigationPanel::GetSelectedChapter() const
{
    return ui_ListWidget_Chapter->currentRow() + 1;
}

int NavigationPanel::GetSelectedVerseFrom() const
{
    return ui_ListWidget_VerseFrom->currentRow() + 1;
}

int NavigationPanel::GetSelectedVerseTo() const
{
    return ui_ListWidget_VerseTo->currentRow() + 1;
}

void NavigationPanel::HideVerseTo()
{
    ui_ListWidget_VerseTo->hide();
}

void NavigationPanel::reloadBookNames()
{
    int idx = ui_ListWidget_Book->currentRow();
    ui_ListWidget_Book->blockSignals(true);
    ui_ListWidget_Book->clear();
    ui_ListWidget_Book->addItems(*m_bookNames);
    ui_ListWidget_Book->setCurrentRow(idx);
    ui_ListWidget_Book->blockSignals(false);
}

void NavigationPanel::SetLocation(int book, int chapter, bool load)
{
    qDebug() << "NavigationPanel::SetLocation(int book, int chapter)";
    SetLocation({ book, chapter, 1, GetNumVerses(book, chapter) }, load);
}

void NavigationPanel::SetLocation_Old(int book, int chapter, int verseFrom, int verseTo, bool sendSignal)
{
    qDebug() << "NavigationPanel::selectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal)";
    int oldChapterCount = ui_ListWidget_Chapter->count();
    int oldVerseCount = ui_ListWidget_VerseTo->count();

    ui_ListWidget_Book->blockSignals(true);
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);

    int chapterCount = GetChapterCount(book);
    if (chapterCount != oldChapterCount) {
        ui_ListWidget_Chapter->clear();
        ui_ListWidget_Chapter->addItems(GenerateRange(1, chapterCount));
    }

    int verseCount = GetNumVerses(book, chapter);
    if (verseCount != oldVerseCount) {
        ui_ListWidget_VerseFrom->clear();
        ui_ListWidget_VerseTo->clear();
        QStringList chapterNumbers = GenerateRange(1, verseCount);
        ui_ListWidget_VerseFrom->addItems(chapterNumbers);
        ui_ListWidget_VerseTo->addItems(chapterNumbers);
    }

    ui_ListWidget_Book->setCurrentRow(book - 1);
    ui_ListWidget_Chapter->setCurrentRow(chapter - 1);
    ui_ListWidget_VerseFrom->setCurrentRow(verseFrom - 1);
    ui_ListWidget_VerseTo->setCurrentRow(verseTo - 1);

    if (sendSignal) {
        OnLocationChanged();
    }

    ui_ListWidget_Book->blockSignals(false);
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_VerseFrom->blockSignals(false);
    ui_ListWidget_VerseTo->blockSignals(false);
}

void NavigationPanel::SelectPassageByChapterId(int chapterId)
{
    qDebug() << "BibleNavigationPanel::SelectPassageByChapterId(int chapterId)";
    QString command = "SELECT Book, Chapter, NumVerses FROM ChapterMap WHERE Id = " %
            QString::number(chapterId);
    qDebug() << command;
    qDebug();
    QSqlQuery query(*m_pVerseData);
    if (query.exec(command)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verseTo = record.value(2).toInt();
            SetLocation_Old(book, chapter, 1, verseTo);
        }
    }
}

void NavigationPanel::SelectPassageByVerseId(int verseId)
{
    QString command = "SELECT Book, Chapter, Verse FROM VerseMap WHERE Id = " %
            QString::number(verseId);
    QSqlQuery query(*m_pVerseData);
    if (query.exec(command)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt();
            int chapter = record.value(1).toInt();
            int verse = record.value(2).toInt();
            SetLocation_Old(book, chapter, verse, verse);
        }
    }
}

/*!
 * \brief Determines whether pressing the previous/next button navigates to the previous/next verse or chapter.
 * \param moveByVerse
 */
void NavigationPanel::SetMoveByVerse(bool moveByVerse)
{
    m_moveByVerse = moveByVerse;
}

void NavigationPanel::SetLocation(qbv::Location location, bool load)
{
    int oldChapterCount = ui_ListWidget_Chapter->count();
    int oldVerseCount = ui_ListWidget_VerseTo->count();

    ui_ListWidget_Book->blockSignals(true);
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);

    int chapterCount = GetChapterCount(location.book);
    if (chapterCount != oldChapterCount) {
        ui_ListWidget_Chapter->clear();
        ui_ListWidget_Chapter->addItems(GenerateRange(1, chapterCount));
    }

    int verseCount = GetNumVerses(location.book, location.chapter);
    if (verseCount != oldVerseCount) {
        ui_ListWidget_VerseFrom->clear();
        ui_ListWidget_VerseTo->clear();
        QStringList chapterNumbers = GenerateRange(1, verseCount);
        ui_ListWidget_VerseFrom->addItems(chapterNumbers);
        ui_ListWidget_VerseTo->addItems(chapterNumbers);
    }

    ui_ListWidget_Book->setCurrentRow(location.book - 1);
    ui_ListWidget_Chapter->setCurrentRow(location.chapter - 1);
    if (location.verse1 > -1) {
        ui_ListWidget_VerseFrom->setCurrentRow(location.verse1 - 1);
    } else {
        ui_ListWidget_VerseFrom->setCurrentRow(0);
    }
    if (location.verse2 > -1) {
        ui_ListWidget_VerseTo->setCurrentRow(location.verse2 - 1);
    } else {
        if (location.verse1 == -1) {
            ui_ListWidget_VerseTo->setCurrentRow(verseCount - 1);
        } else {
            ui_ListWidget_VerseTo->setCurrentRow(location.verse1 - 1);
        }

    }
    OnLocationChanged(load);

    ui_ListWidget_Book->blockSignals(false);
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_VerseFrom->blockSignals(false);
    ui_ListWidget_VerseTo->blockSignals(false);
}

void NavigationPanel::OnLocationChanged(bool emitSignals)
{
    qDebug() << "NavigationPanel::OnLocationChanged()";
    int book = ui_ListWidget_Book->currentRow() + 1;
    int chapter = ui_ListWidget_Chapter->currentRow() + 1;
    int verseFrom = ui_ListWidget_VerseFrom->currentRow() + 1;
    int verseTo = ui_ListWidget_VerseTo->currentRow() + 1;
    if (m_moveByVerse) {
        if (emitSignals) {
            emit LocationChanged(book, chapter, verseFrom, verseFrom);
        }
        m_verseId = GetVerseId(book, chapter, verseFrom);
        ui_Button_Next->setEnabled(m_verseId < 31102);
        ui_Button_Prev->setEnabled(m_verseId > 1);
    } else {
        if (emitSignals) {
            emit LocationChanged(book, chapter, verseFrom, verseTo);
        }
        m_chapterId = GetChapterId(book, chapter);
        ui_Button_Next->setEnabled(m_chapterId < 1189);
        ui_Button_Prev->setEnabled(m_chapterId > 1);
    }
}

void NavigationPanel::SetUiTexts()
{
    ui_Label_Book->setText(tr("Book:"));
    ui_Label_Chapter->setText(tr("Chapter:"));
    ui_Label_Verses->setText(tr("Verses:"));

    ui_Button_Random->setText(tr("Random"));
    ui_Button_Random->setToolTip(tr("Go to a random chapter."));
    ui_Button_Prev->setToolTip(tr("Go to the previous chapter."));
    ui_Button_Next->setToolTip(tr("Go to the next chapter."));
}

void NavigationPanel::OnCurrentBookChanged(int currentBook)
{
    qDebug() << "onCurrentBookChanged()";
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_Chapter->clear();
    ui_ListWidget_Chapter->addItems(GenerateRange(1, GetChapterCount(currentBook)));
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_Chapter->setCurrentRow(0);
}

void NavigationPanel::OnCurrentChapterChanged(int currentChapter)
{
    qDebug() << "NavigationPanel::OnCurrentChapterChanged(int currentRow)";
    int verseCount = GetNumVerses(ui_ListWidget_Book->currentRow() + 1, currentChapter);
    QStringList verseNumbers = GenerateRange(1, verseCount);

    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);
    ui_ListWidget_VerseFrom->clear();
    ui_ListWidget_VerseFrom->addItems(verseNumbers);
    ui_ListWidget_VerseFrom->setCurrentRow(0);

    ui_ListWidget_VerseTo->clear();
    ui_ListWidget_VerseTo->addItems(verseNumbers);
    ui_ListWidget_VerseTo->blockSignals(false);
    ui_ListWidget_VerseTo->setCurrentRow(verseCount - 1);
    ui_ListWidget_VerseFrom->blockSignals(false);
}

void NavigationPanel::OnCurrentVerseFromChanged(int currentVerse)
{
    qDebug() << "NavigationPanel::OnCurrentVerseFromChanged(int currentRow)";
    if (currentVerse - 1 > ui_ListWidget_VerseTo->currentRow()) {
        ui_ListWidget_VerseTo->blockSignals(true);
        ui_ListWidget_VerseTo->setCurrentRow(currentVerse - 1);
        ui_ListWidget_VerseTo->blockSignals(false);
    }
    OnLocationChanged();
}

void NavigationPanel::OnCurrentVerseToChanged(int currentVerse)
{
    qDebug() << "NavigationPanel::onCurrentVerseToChanged(int currentRow)";
    if (currentVerse - 1 < ui_ListWidget_VerseFrom->currentRow()) {
        ui_ListWidget_VerseFrom->blockSignals(true);
        ui_ListWidget_VerseFrom->setCurrentRow(currentVerse - 1);
        ui_ListWidget_VerseFrom->blockSignals(false);
    }
    OnLocationChanged();
}


void NavigationPanel::OnNextButtonClicked()
{
    m_moveByVerse ? SelectPassageByVerseId(++m_verseId) : SelectPassageByChapterId(++m_chapterId);
}

void NavigationPanel::OnPrevButtonClicked()
{
    m_moveByVerse ? SelectPassageByVerseId(--m_verseId) : SelectPassageByChapterId(--m_chapterId);
}

void NavigationPanel::OnMatchPassage()
{
    auto loc = SearchEngine::parseLocationStr(ui_LineEdit_MatchPassage->text(), *m_bookNames, *m_bookAbbreviations);
    qDebug() << loc.ToQString();
    SetLocation(loc, true);


}

void NavigationPanel::OnRandomButtonClicked()
{
    QRandomGenerator randGen(quint32(QDateTime::currentDateTime().time().msec()));
    if (m_moveByVerse) {
        SelectPassageByVerseId(randGen.bounded(1, 31102));
    } else {
        SelectPassageByChapterId(randGen.bounded(1, 1189));
    }
}

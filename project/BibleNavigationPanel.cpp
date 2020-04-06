#include "BibleNavigationPanel.h"

BibleNavigationPanel::BibleNavigationPanel(const QSqlDatabase &verseData, const QStringList &bookNames,
                                           QWidget *parent) :
    QHBoxLayout(parent),
    m_bookNames(&bookNames),
    m_chapterNumber(0),
    m_verseData(&verseData)
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

    connectSignals();
   // setUiTexts();
}

void BibleNavigationPanel::connectSignals()
{
    QObject::connect(ui_ListWidget_Book, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { onCurrentBookChanged(currentRow); });
    QObject::connect(ui_ListWidget_Chapter, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { onCurrentChapterChanged(currentRow); });
    QObject::connect(ui_ListWidget_VerseFrom, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { onCurrentVerseFromChanged(currentRow); });
    QObject::connect(ui_ListWidget_VerseTo, QOverload<int>::of(&QListWidget::currentRowChanged),
                     [=] (int currentRow) { onCurrentVerseToChanged(currentRow); });
    QObject::connect(ui_Button_Random, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onRandomButtonClicked(); });
    QObject::connect(ui_Button_Prev, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onPrevButtonClicked(); });
    QObject::connect(ui_Button_Next, QOverload<bool>::of(&QPushButton::clicked),
                     [=] { onNextButtonClicked(); });
}

QStringList BibleNavigationPanel::generateRange(int min, int max)
{
    QStringList range;
    for (int i = min; i <= max; ++i) {
        range << QString::number(i);
    }
    return range;
}

int BibleNavigationPanel::getChapterCount(int book)
{
    QString command = "SELECT MAX(Chapter) FROM Counters WHERE Book = " % QString::number(book + 1);
    QSqlQuery query(*m_verseData);
    int chapterCount = -1;
    if (query.exec(command)) {
        if (query.next()) {
            chapterCount = query.record().value(0).toInt();

        }
    }
    return chapterCount;
}

int BibleNavigationPanel::getVerseCount(int book, int chapter)
{
    QString command = "SELECT VerseCount FROM Counters WHERE Book = " %
            QString::number(book + 1) % " AND Chapter = " % QString::number(chapter + 1);
    QSqlQuery query(*m_verseData);
    int verseCount = -1;
    if (query.exec(command)) {
        if (query.next()) {
            verseCount = query.record().value(0).toInt();
        }
    }
    return verseCount;
}

int BibleNavigationPanel::getChapterNumber(int book, int chapter)
{
    QString command = "SELECT ChapterNumber FROM Counters WHERE Book = " %
            QString::number(book + 1) % " AND Chapter = " % QString::number(chapter + 1);
    QSqlQuery query(*m_verseData);
    int chapterNumber = 0;
    if (query.exec(command)) {
        if (query.next()) {
            chapterNumber = query.record().value(0).toInt();
        }
    }
    return chapterNumber;
}

void BibleNavigationPanel::reloadBookNames()
{
    ui_ListWidget_Book->blockSignals(true);
    int idx = ui_ListWidget_Book->currentRow();
    ui_ListWidget_Book->clear();
    ui_ListWidget_Book->addItems(*m_bookNames);
    ui_ListWidget_Book->setCurrentRow(idx);
    ui_ListWidget_Book->blockSignals(false);
}

void BibleNavigationPanel::selectPassage(int book, int chapter)
{
    selectPassage(book, chapter, 0, getVerseCount(book, chapter) - 1);
}

void BibleNavigationPanel::selectPassage(int book, int chapter, int verseFrom, int verseTo, bool sendSignal)
{
    int oldChapterCount = ui_ListWidget_Chapter->count();
    int oldVerseCount = ui_ListWidget_VerseTo->count();

    ui_ListWidget_Book->blockSignals(true);
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_VerseFrom->blockSignals(true);
    ui_ListWidget_VerseTo->blockSignals(true);

    int chapterCount = getChapterCount(book);
    if (chapterCount != oldChapterCount) {
        ui_ListWidget_Chapter->clear();
        ui_ListWidget_Chapter->addItems(generateRange(1, chapterCount));
    }

    int verseCount = getVerseCount(book, chapter);
    if (verseCount != oldVerseCount) {
        ui_ListWidget_VerseFrom->clear();
        ui_ListWidget_VerseTo->clear();
        QStringList chapterNumbers = generateRange(1, verseCount);
        ui_ListWidget_VerseFrom->addItems(chapterNumbers);
        ui_ListWidget_VerseTo->addItems(chapterNumbers);
    }

    ui_ListWidget_Book->setCurrentRow(book);
    ui_ListWidget_Chapter->setCurrentRow(chapter);
    ui_ListWidget_VerseFrom->setCurrentRow(verseFrom);
    ui_ListWidget_VerseTo->setCurrentRow(verseTo);

    if (sendSignal) {
        sendPassageLocation();
    }

    ui_ListWidget_Book->blockSignals(false);
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_VerseFrom->blockSignals(false);
    ui_ListWidget_VerseTo->blockSignals(false);
}

void BibleNavigationPanel::selectPassage(int chapterNumber)
{
    QString command = "SELECT Book, Chapter, VerseCount FROM Counters WHERE ChapterNumber = " %
            QString::number(chapterNumber);
    QSqlQuery query(*m_verseData);
    if (query.exec(command)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            int book = record.value(0).toInt() - 1;
            int chapter = record.value(1).toInt() - 1;
            int verseTo = record.value(2).toInt() - 1;
            selectPassage(book, chapter, 0, verseTo);
        }
    }
}

void BibleNavigationPanel::sendPassageLocation()
{
    qDebug() << "sendPassageLocation()";
    emit passageChanged(ui_ListWidget_Book->currentRow(),
                        ui_ListWidget_Chapter->currentRow(),
                        ui_ListWidget_VerseFrom->currentRow(),
                        ui_ListWidget_VerseTo->currentRow());
    m_chapterNumber = getChapterNumber(ui_ListWidget_Book->currentRow(), ui_ListWidget_Chapter->currentRow());
    ui_Button_Next->setEnabled(m_chapterNumber < 1189);
    ui_Button_Prev->setEnabled(m_chapterNumber > 1);
}

void BibleNavigationPanel::setUiTexts()
{
    ui_Label_Book->setText(tr("Book:"));
    ui_Label_Chapter->setText(tr("Chapter:"));
    ui_Label_Verses->setText(tr("Verses:"));

    ui_Button_Random->setText(tr("Random"));
    ui_Button_Random->setToolTip(tr("Go to a random chapter."));
    ui_Button_Prev->setToolTip(tr("Go to the previous chapter."));
    ui_Button_Next->setToolTip(tr("Go to the next chapter."));
}

void BibleNavigationPanel::onCurrentBookChanged(int currentRow)
{
    qDebug() << "onCurrentBookChanged()";
    ui_ListWidget_Chapter->blockSignals(true);
    ui_ListWidget_Chapter->clear();
    ui_ListWidget_Chapter->addItems(generateRange(1, getChapterCount(currentRow)));
    ui_ListWidget_Chapter->blockSignals(false);
    ui_ListWidget_Chapter->setCurrentRow(0);
}

void BibleNavigationPanel::onCurrentChapterChanged(int currentRow)
{
    qDebug() << "onCurrentChapterChanged()";
    int verseCount = getVerseCount(ui_ListWidget_Book->currentRow(), currentRow);
    QStringList verseNumbers = generateRange(1, verseCount);

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

void BibleNavigationPanel::onCurrentVerseFromChanged(int currentRow)
{
    qDebug() << "onCurrentVerseFromChanged()";
    if (currentRow > ui_ListWidget_VerseTo->currentRow()) {
        ui_ListWidget_VerseTo->blockSignals(true);
        ui_ListWidget_VerseTo->setCurrentRow(currentRow);
        ui_ListWidget_VerseTo->blockSignals(false);
    }
    sendPassageLocation();
}

void BibleNavigationPanel::onCurrentVerseToChanged(int currentRow)
{
    qDebug() << "onCurrentVerseToChanged()";
    if (currentRow < ui_ListWidget_VerseFrom->currentRow()) {
        ui_ListWidget_VerseFrom->blockSignals(true);
        ui_ListWidget_VerseFrom->setCurrentRow(currentRow);
        ui_ListWidget_VerseFrom->blockSignals(false);
    }
    sendPassageLocation();
}

void BibleNavigationPanel::onNextButtonClicked()
{
    selectPassage(++m_chapterNumber);
}

void BibleNavigationPanel::onPrevButtonClicked()
{
    selectPassage(--m_chapterNumber);
}

void BibleNavigationPanel::onRandomButtonClicked()
{
    QRandomGenerator randGen(quint32(QDateTime::currentDateTime().time().msec()));
    selectPassage(randGen.bounded(1, 1189));
}

#include "BiblePassageBrowser.h"
#include "Formatting.h"

BiblePassageBrowser::BiblePassageBrowser(QWidget *parent) :
    QTextBrowser(parent),
    m_isZeroIndexed(true),
    m_hasOldTestament(true),
    m_hasStrong(false)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);

    QObject::connect(this, QOverload<const QString &>::of(&QTextBrowser::highlighted),
                     [=] (const QString &link) { onTextBrowserHighlighted(link); } );
}

BiblePassageBrowser::~BiblePassageBrowser()
{
    QTextEdit::clear();
    m_notes.clear();
}

int BiblePassageBrowser::getBook() const
{
    return m_book;
}

int BiblePassageBrowser::getChapter() const
{
    return m_chapter;
}

QString BiblePassageBrowser::getPassageAsPlainText(int verseFrom, int verseTo, bool includeVerseNumber)
{
    if (!m_isZeroIndexed) {
        --verseFrom;
        --verseTo;
    }
    QString text;
    for (int i = verseFrom; i <= verseTo; ++i) {
        QString textToAppend = includeVerseNumber ?
                    QTextBrowser::document()->findBlockByNumber(i).text().simplified() :
                    removeVerseNumber(QTextBrowser::document()->findBlockByNumber(i).text().simplified());
        text += i == verseFrom ? textToAppend : " " % textToAppend;
    }
    return text.remove("*");
}

int BiblePassageBrowser::getVerseCount() const
{
    return m_verseCount;
}

void BiblePassageBrowser::insertNotFoundMessage(QTextCursor &cursor, int missingVerseCount, int &verseNumber)
{
    for (int i = 0; i < missingVerseCount; ++i) {
        cursor.insertHtml("[" % QString::number(verseNumber) %
                          "] <font color='gray'><i>" % tr("Not found.") % "</i></font>");
        cursor.insertBlock();
        ++verseNumber;
    }
}

void BiblePassageBrowser::loadPassageV1(const QSqlDatabase &module)
{
    QTextEdit::clear();
    m_notes.clear();
    QString book = m_isZeroIndexed ? QString::number(m_book + 1) : QString::number(m_book);
    QString chapter = m_isZeroIndexed ? QString::number(m_chapter + 1) : QString::number(m_chapter);
    QString verseFrom = m_isZeroIndexed ? QString::number(m_verseFrom + 1) : QString::number(m_verseFrom);
    QString verseTo = m_isZeroIndexed ? QString::number(m_verseTo + 1) : QString::number(m_verseTo);
    QString queryString = QStringLiteral("SELECT Verse, Scripture FROM Bible "
                                         "WHERE Book = %1 "
                                         "AND Chapter = %2 "
                                         "AND Verse >= %3 "
                                         "AND Verse <= %4").arg(book, chapter, verseFrom, verseTo);
    int verseNumber = m_isZeroIndexed ? m_verseFrom + 1 : m_verseFrom;
    int verseCount = m_isZeroIndexed ? m_verseTo + 1 : m_verseTo;
    QSqlQuery query(module);
    bool hasNoRecords = true;
    int verse = 1;
    if (query.exec(queryString)) {
        QTextCursor cursor(textCursor());
        while (query.next() && verseNumber <= verseCount) {
            hasNoRecords = false;
            QSqlRecord record = query.record();
            verse = record.value(0).toInt();
            if (verseNumber != verse) {
                insertNotFoundMessage(cursor, verse - verseNumber, verseNumber);
            }
            QString scripture = record.value(1).toString();
            Formatting::formatScripture(scripture, m_notes, m_hasStrong);
            cursor.insertHtml("[" % QString::number(verse) + "] " % scripture);
            cursor.insertBlock();
            ++verseNumber;
        }
        if (hasNoRecords) {
            QTextEdit::setHtml("<h2>Not found in this module.</h2>");
        } else if (verse < verseCount) {
            insertNotFoundMessage(cursor, verseCount - verse, verseNumber);
        }
    }
}

void BiblePassageBrowser::loadPassageV2(const QSqlDatabase &module)
{
    QTextEdit::clear();
    m_notes.clear();
    QString book = m_isZeroIndexed ? QString::number(m_book + 1) : QString::number(m_book);
    QString chapter = m_isZeroIndexed ? QString::number(m_chapter + 1) : QString::number(m_chapter);
    int verseFrom = m_isZeroIndexed ? m_verseFrom + 1 : m_verseFrom;
    int verseTo = m_isZeroIndexed ? m_verseTo + 1 : m_verseTo;
    QSqlQuery query(module);
    for (int verse = verseFrom; verse <= verseTo; ++verse) {
        QString command = QStringLiteral("SELECT Scripture FROM Bible "
                                         "WHERE Book = %1 "
                                         "AND Chapter = %2 "
                                         "AND Verse = %3 ").arg(book, chapter, QString::number(verse));
        if (query.exec(command)) {
            QTextCursor cursor = QTextEdit::textCursor();
            if (query.next()) {
                QString scripture = query.record().value(0).toString();
                Formatting::formatScripture(scripture, m_notes, m_hasStrong);
                cursor.movePosition(QTextCursor::End);
                cursor.insertHtml("[" % QString::number(verse) % "] " % scripture);
            } else {
                cursor.insertHtml("[" % QString::number(verse) % "] - - -");
            }
            cursor.insertBlock();
            QTextEdit::moveCursor(QTextCursor::Start);
        }
    }
}

QString BiblePassageBrowser::removeVerseNumber(const QString &verse)
{
    QStringList splitVerse = verse.split(" ");
    splitVerse.removeFirst();
    return splitVerse.join(" ");
}

void BiblePassageBrowser::setBook(int book)
{
    m_book = book;
}

void BiblePassageBrowser::setChapter(int chapter)
{
    m_chapter = chapter;
}

void BiblePassageBrowser::setHasOldTestament(int hasOldTestament)
{
    m_hasOldTestament = hasOldTestament;
}

void BiblePassageBrowser::setIsZeroIndexed(bool isZeroIndexed)
{
    m_isZeroIndexed = isZeroIndexed;
}

void BiblePassageBrowser::setHasStrong(bool hasStrong)
{
    m_hasStrong = hasStrong;
}

void BiblePassageBrowser::setPassage(int book, int chapter, int verseFrom, int verseTo)
{
    m_book = book;
    m_chapter = chapter;
    m_verseFrom = verseFrom;
    m_verseTo = verseTo;
}

void BiblePassageBrowser::setVerseCount(int count)
{
    m_verseCount = count;
}

void BiblePassageBrowser::setVerseFrom(int verseFrom)
{
    m_verseFrom = verseFrom;
}

void BiblePassageBrowser::setVerseTo(int verseTo)
{
    m_verseTo = verseTo;
}

void BiblePassageBrowser::selectPassage(int verseFrom, int verseTo)
{
    if (!m_isZeroIndexed) {
        --verseFrom;
        --verseTo;
    }
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    QTextCursor cursor = QTextEdit::textCursor();
    format.setBackground(QBrush(QColor(10, 100, 200)));
    cursor.setBlockFormat(format);
    QTextDocument *doc = QTextEdit::document();
    QTextBlock startBlock = doc->findBlockByNumber(verseFrom);
    QTextBlock endBlock = doc->findBlockByNumber(verseTo);
    int start = startBlock.position();
    int end = endBlock.position() + endBlock.text().length();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    QTextEdit::setTextCursor(cursor);
    QWidget::setFocus();
}

void BiblePassageBrowser::onTextBrowserHighlighted(const QString &link)
{
    if (!link.isEmpty()) {
        if (link.startsWith("c:")) {
            QString markupText = m_notes[link.split(":")[1].toInt() - 1];
            QString plainText = markupText.mid(4, markupText.size() - 8);
            plainText.replace("[i]", "<i>").replace("[/i]", "</i>");
            QString fontFamily = "font-family:" % QApplication::font().family();
            QString fontSize = "font-size:" % QString::number(QFontInfo(QWidget::font()).pixelSize()) % "px";
            QString note = QString("<p style='white-space:pre;%1;%2'>%3</p>")
                    .arg(fontFamily, fontSize, plainText);
            QToolTip::showText(QCursor::pos(), note, nullptr, QRect(), 2147483647);
        }
    } else {
        QToolTip::hideText();
    }
}


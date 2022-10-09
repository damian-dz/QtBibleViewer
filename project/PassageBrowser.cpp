#include "PassageBrowser.h"
#include "Formatting.h"

PassageBrowser::PassageBrowser(const QStringList &bookNames, AppConfig *config, QWidget *parent) :
    QTextBrowser(parent),
    m_pBookNames(&bookNames),
    m_pConfig(config),
    m_hasOldTestament(true),
    m_hasStrong(false)
{
    QWidget::setContextMenuPolicy(Qt::CustomContextMenu);
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);

    QObject::connect(this, QOverload<const QString &>::of(&QTextBrowser::highlighted),
                     [=] (const QString &link) { OnHighlighted(link); });
    QObject::connect(this, QOverload<>::of(&QTextEdit::cursorPositionChanged),
                     [=] () { OnCursorPositionChanged(); });
    QObject::connect(this, QOverload<const QPoint &>::of(&QWidget::customContextMenuRequested),
                     [=] (const QPoint &pos) { OnContextMenuRequested(pos); });

}

PassageBrowser::~PassageBrowser()
{
    QTextEdit::clear();
    m_notes.clear();
    m_verseNumbers.clear();
    m_versesUnformatted.clear();
}

int PassageBrowser::GetBook() const
{
    return m_location.book;
}

int PassageBrowser::GetChapter() const
{
    return m_location.chapter;
}

qbv::Location PassageBrowser::GetLocation() const
{
    return m_location;
}

int PassageBrowser::GetVerse1() const
{
    return m_location.verse1;
}

int PassageBrowser::GetVerse2() const
{
    return m_location.verse2;
}

void PassageBrowser::HighlightText(const QString &text)
{
    QTextEdit::extraSelections().clear();
    QTextEdit::moveCursor(QTextCursor::Start);
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    QTextEdit::ExtraSelection extra;
    while (QTextEdit::find(text)) {
        extra.format.setBackground(color);
        extra.cursor = QTextEdit::textCursor();
        extraSelections << extra;
    }
    QTextEdit::setExtraSelections(extraSelections);
    if (!text.isNull() && !text.isEmpty()) {
        extra.cursor.clearSelection();
        QTextEdit::setTextCursor(extra.cursor);
    }
}

void PassageBrowser::HighlightVerse(int verse)
{
    QTextDocument *doc = QTextEdit::document();
    QTextBlock startBlock = doc->findBlockByNumber(verse - 1);
    int start = startBlock.position();
    int end = startBlock.position() + startBlock.text().length();
    QTextCursor cursor = QTextEdit::textCursor();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    QTextEdit::setTextCursor(cursor);
    QWidget::setFocus();
}

void PassageBrowser::InsertNotFoundMessage(QTextCursor &cursor, int missingVerseCount, int &verseNumber)
{
    for (int i = 0; i < missingVerseCount; ++i) {
        cursor.insertHtml(QString("<b>%1%2%3</b> <font color='gray'><i>%4</i>")
                          .arg(m_pConfig->formatting.symbol_before, QString::number(verseNumber),
                               m_pConfig->formatting.symbol_after, tr("Not found.")));
        cursor.insertBlock();
        ++verseNumber;
    }
}

bool PassageBrowser::IsEmpty() const
{
    return QTextEdit::toPlainText().isEmpty();
}

void PassageBrowser::LoadPassage(const QSqlDatabase &module)
{
    QObject::blockSignals(true);
    QTextEdit::clear();
    m_notes.clear();
    m_verseNumbers.clear();
    m_versesUnformatted.clear();

    QString book = QString::number(m_location.book);
    QString chapter = QString::number(m_location.chapter);
    QString verseFrom = QString::number(m_location.verse1);
    QString verseTo = QString::number(m_location.verse2);

    QString command = QStringLiteral("SELECT Verse, Scripture FROM Bible "
                                     "WHERE Book = %1 "
                                     "AND Chapter = %2 "
                                     "AND Verse >= %3 "
                                     "AND Verse <= %4").arg(book, chapter, verseFrom, verseTo);

    int verseNumber = m_location.verse1;
    int finalVerse = m_location.verse2;

    bool hasNoRecords = true;
    int verse = 1;
    QString symbolBefore = m_pConfig->formatting.symbol_before.toHtmlEscaped();
    QString symbolAfter = m_pConfig->formatting.symbol_after.toHtmlEscaped();

    QSqlQuery query(module);
    if (query.exec(command)) {
        QTextCursor cursor(QTextEdit::textCursor());
        while (query.next() && verseNumber <= finalVerse) {

            hasNoRecords = false;
            QSqlRecord record = query.record();
            verse = record.value(0).toInt();
            m_verseNumbers << verse;

            QString verseId = QString("<b>%1%2%3</b>").arg(symbolBefore, QString::number(verse), symbolAfter);

            if (verseNumber != verse) {
                InsertNotFoundMessage(cursor, verse - verseNumber, verseNumber);
            }
            QString scripture = record.value(1).toString();

            m_versesUnformatted << scripture;
            Formatting::FormatTextAndNotes(scripture, m_notes);

            cursor.insertHtml(verseId % " " % scripture);
            if (verseNumber < finalVerse){
                cursor.insertBlock();
            }
            ++verseNumber;
        }
        if (hasNoRecords) {
            QTextEdit::setHtml(QString("<h2>%1</h2>").arg(tr("Not found in this module.")));
        } else if (verse < finalVerse) {
            InsertNotFoundMessage(cursor, finalVerse - verse, verseNumber);
        }
    }
    QTextEdit::moveCursor(QTextCursor::Start);
    QObject::blockSignals(false);
}

void PassageBrowser::LoadPassage_New(const QSqlDatabase &module)
{
    QObject::blockSignals(true);
    QTextEdit::clear();
    m_notes.clear();
    m_verseNumbers.clear();
    m_versesUnformatted.clear();

    QString book = QString::number(m_location.book);
    QString chapter = QString::number(m_location.chapter);
    QString verse1 = QString::number(m_location.verse1);
    QString verse2 = QString::number(m_location.verse2);

    QString command = QStringLiteral("SELECT Verse, Scripture FROM Bible "
                                     "WHERE Book = %1 "
                                     "AND Chapter = %2 "
                                     "AND Verse >= %3 "
                                     "AND Verse <= %4").arg(book, chapter, verse1, verse2);

    int verseNumber = m_location.verse1;
    int finalVerse = m_location.verse2;

    bool hasNoRecords = true;
    int verse = 1;
    QString symbolBefore = m_pConfig->formatting.symbol_before.toHtmlEscaped();
    QString symbolAfter = m_pConfig->formatting.symbol_after.toHtmlEscaped();

    QSqlQuery query(module);
    if (query.exec(command)) {
        QTextCursor cursor(QTextEdit::textCursor());
        while (query.next() && verseNumber <= finalVerse) {

            hasNoRecords = false;
            QSqlRecord record = query.record();

            QString scripture = record.value(1).toString();

            if (scripture.contains("{H}")) {
                qDebug() << "{H}";
                m_verseNumbers << 0;

                QString header = Formatting::ExtractHeading(scripture);
                m_versesUnformatted << header;

                cursor.insertHtml("<b style='font-size:large'>" % header % "</b1>");
                cursor.insertBlock();
            }

            verse = record.value(0).toInt();
            m_verseNumbers << verse;

            QString verseId = QString("<b>%1%2%3</b>").arg(symbolBefore, QString::number(verse), symbolAfter);

            if (verseNumber != verse) {
                InsertNotFoundMessage(cursor, verse - verseNumber, verseNumber);
            }

            m_versesUnformatted << scripture;
            Formatting::FormatTextAndNotes(scripture, m_notes);

            cursor.insertHtml(verseId % " " % scripture);
            if (verseNumber < finalVerse){
                cursor.insertBlock();
            }
            ++verseNumber;
        }
        if (hasNoRecords) {
            QTextEdit::setHtml(QString("<h2>%1</h2>").arg(tr("Not found in this module.")));
        } else if (verse < finalVerse) {
            InsertNotFoundMessage(cursor, finalVerse - verse, verseNumber);
        }
    }
    QTextEdit::moveCursor(QTextCursor::Start);
    QObject::blockSignals(false);
}

void PassageBrowser::SetBook(int book)
{
    m_location.book = book;
}

void PassageBrowser::SetChapter(int chapter)
{
    m_location.chapter = chapter;
}

void PassageBrowser::SetHasOldTestament(bool hasOldTestament)
{
    m_hasOldTestament = hasOldTestament;
}

void PassageBrowser::SetHasStrong(bool hasStrong)
{
    m_hasStrong = hasStrong;
}

void PassageBrowser::SetHighlightColor(QColor &color)
{
    m_highlightColor = &color;
}

void PassageBrowser::SetLocation(const qbv::Location &location)
{
    m_location = location;
}

void PassageBrowser::SetLocation(int book, int chapter, int verse1, int verse2)
{
    m_location.book = book;
    m_location.chapter = chapter;
    m_location.verse1 = verse1;
    m_location.verse2 = verse2;
}

void PassageBrowser::SetVerse1(int verse1)
{
    m_location.verse1 = verse1;
}

void PassageBrowser::SetVerse2(int verse2)
{
    m_location.verse2 = verse2;
}

void PassageBrowser::ComputeSelectedBlockAndVerseRange()
{
    QTextCursor cursor = QTextEdit::textCursor();
    QTextDocument *doc = QTextEdit::document();
    int firstBlock = doc->findBlock(cursor.selectionStart()).blockNumber();
    int lastBlock = doc->findBlock(cursor.selectionEnd()).blockNumber();
    qDebug() << firstBlock << lastBlock;
    m_selectedBlockRange = qMakePair(firstBlock, lastBlock);
    while (m_verseNumbers[firstBlock] == 0) {
        ++firstBlock;
    }
    while (m_verseNumbers[lastBlock] == 0) {
        --lastBlock;
    }
    m_selectedVerseRange = qMakePair(m_verseNumbers[firstBlock], m_verseNumbers[lastBlock]);
}

void PassageBrowser::CopyToClipboard(QString &textToCopy, int verse1, int verse2)
{
    QClipboard *clipboard = qApp->clipboard();

    QString chapter = QString::number(m_location.chapter);
    QString firstVerse = QString::number(verse1);
    QString lastVerse = QString::number(verse2);

    QString book = m_pBookNames->at(m_location.book - 1);

    QString reference =  book % " " % chapter % ":" % firstVerse;
    if (verse1 != verse2) {
        reference += "–" % lastVerse;
    }
    if (m_pConfig->formatting.reference_before) {
        clipboard->setText(reference % ": " % textToCopy.trimmed());
    } else {
        clipboard->setText(textToCopy.trimmed() % "—" % reference);
    }
}

void PassageBrowser::OnAddToFavorites()
{
    qbv::Location loc(m_location.book, m_location.chapter, m_selectedVerseRange.first, m_selectedVerseRange.second);
    emit AddToFavoritesRequested(loc);
}

void PassageBrowser::OnContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = QWidget::mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy Highlighted Text"), this, [=] { QTextEdit::copy(); }, QKeySequence::Copy);
    contextMenu.addAction(tr("Copy with Reference"), this, [=] { OnCopyWithReference(); });
    contextMenu.addAction(tr("Copy with Reference as TeX"), this, [=] { OnCopyAsTeXWithReference(); });
    contextMenu.addAction(tr("Copy with Reference as HTML"), this, [=] { OnCopyAsHtmlWithReference(); });
    contextMenu.addAction(tr("Select All"), this, [=] { QTextEdit::selectAll(); }, QKeySequence::SelectAll);
    contextMenu.addSeparator();
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = QTextEdit::textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    ComputeSelectedBlockAndVerseRange();
    QString crossRefMsg = tr("Show Cross-References for Verse ") + QString::number(m_selectedVerseRange.second);
    contextMenu.addAction(crossRefMsg, this, [=] { OnShowCrossReferences(); });
    QString addVerseMsg = m_selectedVerseRange.first == m_selectedVerseRange.second ?
                   tr("Add Verse ") + QString::number(m_selectedVerseRange.first) + tr(" to Favorites") :
                   tr("Add Verses ") + QString::number(m_selectedVerseRange.first) +
                   "–" + QString::number(m_selectedVerseRange.second) + tr(" to Favorites");
    contextMenu.addAction(addVerseMsg, this, [=] { OnAddToFavorites(); });
    contextMenu.exec(globalPos);
}

void PassageBrowser::OnCursorPositionChanged()
{
    QTextCursor cursor = QTextEdit::textCursor();
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    if (!m_lastCursor.isNull()) {
        m_lastCursor.setBlockFormat(format);
    }
    format.setBackground(QBrush(*m_highlightColor));
    cursor.setBlockFormat(format);
    m_lastCursor = cursor;
}

void PassageBrowser::SelectPassage(int verse1, int verse2)
{
//    if (!m_isZeroIndexed) {
//        --verseFrom;
//        --verseTo;
//    }
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    QTextCursor cursor = QTextEdit::textCursor();
    format.setBackground(QBrush(QColor(10, 100, 200)));
    cursor.setBlockFormat(format);
    QTextDocument *doc = QTextEdit::document();
    QTextBlock startBlock = doc->findBlockByNumber(verse1);
    QTextBlock endBlock = doc->findBlockByNumber(verse2);
    int start = startBlock.position();
    int end = endBlock.position() + endBlock.text().length();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    QTextEdit::setTextCursor(cursor);
    QWidget::setFocus();
}

void PassageBrowser::OnHighlighted(const QString &link)
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

void PassageBrowser::CopyWithReferenceTemplate(PassageBrowser::FormattingFunc format)
{
    int firstBlock = m_selectedBlockRange.first;
    int lastBlock = m_selectedBlockRange.second;
    QString textToCopy;
    for (int i = firstBlock; i <= lastBlock; ++i) {
        if (m_pConfig->formatting.include_numbers) {
            if (i > firstBlock) {
                textToCopy += " ";
            }
            if (m_verseNumbers[i] > 0) {
                textToCopy += m_pConfig->formatting.symbol_before + QString::number(m_verseNumbers[i]) +
                        m_pConfig->formatting.symbol_after + " ";
            }
        }
        QString scripture = m_versesUnformatted[i];
        if (m_verseNumbers[i] > 0) {
            format(scripture);
            textToCopy += scripture;
        }
    }
    while (m_verseNumbers[firstBlock] == 0) {
        ++firstBlock;
    }
    while (m_verseNumbers[lastBlock] == 0) {
        --lastBlock;
    }
    CopyToClipboard(textToCopy, m_verseNumbers[firstBlock], m_verseNumbers[lastBlock]);
}

void PassageBrowser::OnCopyWithReference()
{
    CopyWithReferenceTemplate(Formatting::RemoveTagsAndNotes);
}

void PassageBrowser::OnCopyAsTeXWithReference()
{
    CopyWithReferenceTemplate(Formatting::FormatAsTeX);
}

void PassageBrowser::OnCopyAsHtmlWithReference()
{
    CopyWithReferenceTemplate(Formatting::RemoveNotes);
}

void PassageBrowser::OnShowCrossReferences()
{
    emit CrossReferenceRequested(m_selectedVerseRange.second);
}

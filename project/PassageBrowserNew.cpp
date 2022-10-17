#include "PassageBrowserNew.h"

PassageBrowserNew::PassageBrowserNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) : QTextBrowser(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService)
{
    QWidget::setContextMenuPolicy(Qt::CustomContextMenu);
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);

    QObject::connect(this, QOverload<const QPoint &>::of(&QWidget::customContextMenuRequested),
                     [=] (const QPoint &pos) { OnContextMenuRequested(pos); });
    QObject::connect(this, QOverload<>::of(&QTextEdit::cursorPositionChanged),
                     [=] () { OnCursorPositionChanged(); });
    QObject::connect(this, QOverload<const QString &>::of(&QTextBrowser::highlighted),
                     [=] (const QString &link) { OnHighlighted(link); });
}

PassageBrowserNew::~PassageBrowserNew()
{

}

void PassageBrowserNew::SetAndLoadPassageWithNotes(qbv::PassageWithNotes passageWithNotes)
{
    m_passage = passageWithNotes;
    LoadPassageWithNotes();
}

void PassageBrowserNew::SetPassageWithNotes(qbv::PassageWithNotes passageWithNotes)
{
    m_passage = passageWithNotes;
}

void PassageBrowserNew::LoadPassageWithNotes()
{
    qDebug() << "PassageBrowserNew:LoadPassageWithNotes";
    QObject::blockSignals(true);
    QTextEdit::clear();
    QString symbolBefore = m_pConfig->formatting.symbol_before.toHtmlEscaped();
    QString symbolAfter = m_pConfig->formatting.symbol_after.toHtmlEscaped();
    QTextCursor cursor(QTextEdit::textCursor());
    int numVerses = m_passage.verses.count();
    for (int i = 0; i < numVerses; ++i) {
        QString verseId = QString("<b>%1%2%3</b>")
                                  .arg(symbolBefore, QString::number(m_passage.verses[i]), symbolAfter);
        if (m_passage.IsNullOrEmpty(i)) {
            cursor.insertHtml(QString("%1 <font color='gray'><i>%2</i>").arg(verseId, tr("Not found.")));
            cursor.insertBlock();
        }
        cursor.insertHtml(verseId % " " % m_passage.formatted[i]);
        if (i < numVerses - 1){
            cursor.insertBlock();
        }
    }
    QTextEdit::moveCursor(QTextCursor::Start);
    QObject::blockSignals(false);
}

void PassageBrowserNew::SetScriptures(const QStringList &scriptures, bool hasStrong)
{
    m_scriptures = scriptures;
    m_notes.clear();
    m_verses.clear();

    QObject::blockSignals(true);
    QTextEdit::clear();
    QString symbolBefore = m_pConfig->formatting.symbol_before.toHtmlEscaped();
    QString symbolAfter = m_pConfig->formatting.symbol_after.toHtmlEscaped();
    QTextCursor cursor(QTextEdit::textCursor());
    int verseCounter = m_location.verse1;
    int scriptureCounter = 0;
    for (const QString &scripture : scriptures) {
        ++scriptureCounter;
        m_verses.append(verseCounter);
        QString verseId = QString("<b>%1%2%3</b>")
            .arg(symbolBefore, QString::number(verseCounter), symbolAfter);
        if (scripture.isNull() || scripture.isEmpty()) {
            cursor.insertHtml(QString("%1 <font color='gray'><i>%2</i>").arg(verseId, tr("Not found.")));
            cursor.insertBlock();
        }
        QString formatted(scripture);
        Formatting::FormatScriptureAndAddNotes(formatted, m_notes, hasStrong);
        cursor.insertHtml(verseId % " " % formatted);
        if (scriptureCounter < scriptures.count()){
            cursor.insertBlock();
        }
        ++verseCounter;
    }
    QTextEdit::moveCursor(QTextCursor::Start);
    QObject::blockSignals(false);
}

bool PassageBrowserNew::HasText() const
{
    QString plain = toPlainText();
    return !plain.isNull() && !plain.isEmpty();
}

qbv::Location PassageBrowserNew::Location() const
{
    return m_location;
}

void PassageBrowserNew::SetLocation(qbv::Location loc)
{
    m_location = loc;
}

void PassageBrowserNew::OnContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = QWidget::mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy Selected Text"), this, [=] { QTextEdit::copy(); }, QKeySequence::Copy);
    contextMenu.addAction(tr("Copy with Reference"), this, [=] { OnCopyWithReference(); });
    contextMenu.addAction(tr("Copy as TeX with Reference"), this, [=] { OnCopyAsTeXWithReference(); });
    contextMenu.addAction(tr("Copy as HTML with Reference"), this, [=] { OnCopyAsHtmlWithReference(); });
    contextMenu.addAction(tr("Copy Unformatted with Reference"), this, [=] { OnCopyUnformattedWithReference(); });
    contextMenu.addAction(tr("Remove Highlight"), this, [=] { OnRemoveHighlight(); });
    contextMenu.addAction(tr("Select All"), this, [=] { QTextEdit::selectAll(); }, QKeySequence::SelectAll);
    contextMenu.addSeparator();
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = QTextEdit::textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    ComputeSelectedBlockAndVerseRanges();
   // QString crossRefMsg = tr("Show Cross-References for Verse ") + QString::number(m_selectedVerseRange.second);
   // contextMenu.addAction(crossRefMsg, this, [=] { OnShowCrossReferences(); });
    QString addVerseMsg = m_selectedVerseRange.first == m_selectedVerseRange.second ?
                   tr("Add Note to Verse ") + QString::number(m_selectedVerseRange.first) :
                   tr("Add Note to Verses ") + QString::number(m_selectedVerseRange.first) +
                   "–" + QString::number(m_selectedVerseRange.second);
    contextMenu.addAction(addVerseMsg, this, [=] { OnAddNote(); });
    contextMenu.exec(globalPos);
}

void PassageBrowserNew::OnCursorPositionChanged()
{
    QTextCursor cursor = QTextEdit::textCursor();
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    if (!m_lastCursor.isNull()) {
        m_lastCursor.setBlockFormat(format);
    }
    format.setBackground(QBrush(m_pConfig->appearance.verse_highlight_color));
    cursor.setBlockFormat(format);
    m_lastCursor = cursor;
}

void PassageBrowserNew::OnHighlighted(const QString &link)
{
    if (!link.isEmpty() && link.startsWith("c:")) {
        QString markupText = m_notes[link.split(":")[1].toInt() - 1];
        QString plainText = markupText.mid(4, markupText.size() - 8);
        plainText.replace("[i]", "<i>").replace("[/i]", "</i>");
        QString fontFamily = "font-family:" % QApplication::font().family();
        QString fontSize = "font-size:" % QString::number(QFontInfo(QWidget::font()).pixelSize()) % "px";
        QString note = QString("<p style='white-space:pre;%1;%2'>%3</p>")
                               .arg(fontFamily, fontSize, plainText);
        QToolTip::showText(QCursor::pos(), note, nullptr, QRect(), 2147483647);
    }
    else {
        QToolTip::hideText();
    }
}

void PassageBrowserNew::CopyWithReferenceTemplate(PassageBrowserNew::FormattingFunc format, bool formatStrong)
{
    int firstBlock = m_selectedBlockRange.first;
    int lastBlock = m_selectedBlockRange.second;
    QString textToCopy;
    for (int i = firstBlock; i <= lastBlock; ++i) {
        if (m_pConfig->formatting.include_numbers) {
            if (i > firstBlock) {
                textToCopy += " ";
            }
            if (m_verses[i] > 0) {
                textToCopy += m_pConfig->formatting.symbol_before + QString::number(m_verses[i]) +
                    m_pConfig->formatting.symbol_after + " ";
            }
        }
        QString scripture = m_scriptures[i];
        if (m_verses[i] > 0) {
            format(scripture);
            if (formatStrong) {
                Formatting::FormatStrongAsPlainText(scripture);
            }
            textToCopy += scripture;
        }
    }
    while (m_verses[firstBlock] == 0) {
        ++firstBlock;
    }
    while (m_verses[lastBlock] == 0) {
        --lastBlock;
    }
    CopyToClipboard(textToCopy, m_verses[firstBlock], m_verses[lastBlock]);
}

void PassageBrowserNew::OnCopyWithReference()
{
    CopyWithReferenceTemplate(Formatting::RemoveTagsAndNotes);
}

void PassageBrowserNew::OnCopyAsTeXWithReference()
{
    CopyWithReferenceTemplate(Formatting::FormatAsTeX);
}

void PassageBrowserNew::OnCopyAsHtmlWithReference()
{
    CopyWithReferenceTemplate(Formatting::RemoveNotes);
}

void PassageBrowserNew::OnCopyUnformattedWithReference()
{
    CopyWithReferenceTemplate(Formatting::LeaveIntact, false);
}

void PassageBrowserNew::OnRemoveHighlight()
{
    QTextBlockFormat format;
    format.setBackground(Qt::transparent);
    if (!m_lastCursor.isNull()) {
        m_lastCursor.setBlockFormat(format);
    }
}

void PassageBrowserNew::OnAddNote()
{
    qbv::Location loc(m_location.book, m_location.chapter, m_selectedVerseRange.first, m_selectedVerseRange.second);
    emit AddNoteRequested(loc);
}

void PassageBrowserNew::ComputeSelectedBlockAndVerseRanges()
{
    QTextCursor cursor = QTextEdit::textCursor();
    QTextDocument *doc = QTextEdit::document();
    int firstBlock = doc->findBlock(cursor.selectionStart()).blockNumber();
    int lastBlock = doc->findBlock(cursor.selectionEnd()).blockNumber();
    m_selectedBlockRange = qMakePair(firstBlock, lastBlock);
    while (m_verses[firstBlock] == 0) {
        ++firstBlock;
    }
    while (m_verses[lastBlock] == 0) {
        --lastBlock;
    }
    m_selectedVerseRange = qMakePair(m_verses[firstBlock], m_verses[lastBlock]);
}

void PassageBrowserNew::CopyToClipboard(QString &textToCopy, int verse1, int verse2)
{
    QClipboard *clipboard = qApp->clipboard();

    QString chapter = QString::number(m_location.chapter);
    QString firstVerse = QString::number(verse1);
    QString lastVerse = QString::number(verse2);

    QString book = m_pDatabaseService->BookNameForNumber(m_location.book);

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

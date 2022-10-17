#include "CompareVerseBrowser.h"
#include "Formatting.h"

CompareVerseBrowser::CompareVerseBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(false);

    QVector<QTextLength> colWidths;
    colWidths.append(QTextLength(QTextLength::VariableLength, 5));
    colWidths.append(QTextLength(QTextLength::PercentageLength, 100));

    m_tableFormat.setColumnWidthConstraints(colWidths);
    m_tableFormat.setCellPadding(5);

    QObject::connect(this, QOverload<const QUrl &>::of(&QTextBrowser::anchorClicked),
                     [=] (const QUrl &link) { OnAnchorClicked(link); } );
}

CompareVerseBrowser::~CompareVerseBrowser()
{

}

void CompareVerseBrowser::LoadNamesAndVerses(const QList<qbv::Module> &modules)
{
    QString command = "SELECT Scripture FROM Bible WHERE Book = " %
            QString::number(m_book) % " AND Chapter = " % QString::number(m_chapter)
            % " AND Verse = " % QString::number(m_verse);

    QTextEdit::clear();
    QTextCursor cursor(QTextEdit::textCursor());
    cursor.movePosition(QTextCursor::Start);
    QTextTable *table = cursor.insertTable(modules.count(), 2);

    table->setFormat(m_tableFormat);
    for (int i = 0; i < modules.count(); ++i) {
        QSqlQuery query(modules[i].translation);
        if (query.exec(command)) {
            if (query.next()) {
                QString scripture  = query.record().value(0).toString();
                QString shortName = QString("<b><a href='%1' style='text-decoration:none'>%2</a></b>")
                       .arg(QString::number(i), modules[i].shortName());
                table->cellAt(i, 0).firstCursorPosition().insertHtml(shortName);
                Formatting::FormatScriptureAndRemoveNotes(scripture);
                table->cellAt(i, 1).firstCursorPosition().insertHtml(scripture);
            }
        }
    }
}

void CompareVerseBrowser::SetVerseLocation(int book, int chapter, int verse)
{
    m_book = book;
    m_chapter = chapter;
    m_verse = verse;
}

void CompareVerseBrowser::SetScriptures(const QStringList &scriptures, const QStringList &shortNames)
{
    QTextEdit::clear();
    QTextCursor cursor(QTextEdit::textCursor());
    cursor.movePosition(QTextCursor::Start);

    QTextTable *table = cursor.insertTable(scriptures.count(), 2);
    table->setFormat(m_tableFormat);

    for (int i = 0; i < scriptures.count(); ++i) {
        QString scripture  = scriptures[i];
        QString shortName = QString("<b><a href='%1' style='text-decoration:none'>%2</a></b>")
            .arg(shortNames[i], shortNames[i]);
        table->cellAt(i, 0).firstCursorPosition().insertHtml(shortName);
        Formatting::FormatScriptureAndRemoveNotes(scripture, true);
        table->cellAt(i, 1).firstCursorPosition().insertHtml(scripture);
    }
}

void CompareVerseBrowser::OnAnchorClicked(const QUrl &link)
{
    //qDebug() << link.toString();
    // emit TranslationNameClicked(link.toString().toInt(), m_book, m_chapter, m_verse);
    emit BibleNameClicked(link.toString());
    // emit ReferenceClicked(book, chapter, verse);
}

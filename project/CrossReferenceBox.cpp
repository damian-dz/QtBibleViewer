#include "CrossReferenceBox.h"

CrossReferenceBox::CrossReferenceBox(const QList<qbv::Module> &modules, const QSqlDatabase &crossRefs,
                                     const QStringList &bookNames, QWidget *parent) :
    QWidget(parent),
    ui_Label_CrossRefBox(new QLabel),
    ui_CrossRefBrowser(new SearchResultBrowser),
    m_pModules(&modules),
    m_pCrossRefs(&crossRefs),
    m_pBookNames(&bookNames)
{
    QVBoxLayout *mainVerLayout = new QVBoxLayout;
    mainVerLayout->addWidget(ui_Label_CrossRefBox);
    mainVerLayout->addWidget(ui_CrossRefBrowser);

    QWidget::setLayout(mainVerLayout);
    SetUiTexts();
}

void CrossReferenceBox::LoadCrossReferences(int moduleIdx, int book, int chapter, int verse)
{
    QString command = QString("SELECT XRefs FROM CrossReferences WHERE "
                              "Book = %1 AND "
                              "Chapter = %2 AND "
                              "Verse = %3").arg(QString::number(book),
                                                QString::number(chapter),
                                                QString::number(verse));
    QSqlQuery xRefQuery(*m_pCrossRefs);
    if (xRefQuery.exec(command) && xRefQuery.next()) {
        QString xRefs = xRefQuery.record().value("XRefs").toString();
        QStringList xRefsSplit = xRefs.split(',');
        QStringList results;
        QStringList refs;
        for (QString xRef : xRefsSplit) {
            if (xRef.contains('-')) {
                QStringList startEnd = xRef.split('-');
                QStringList startBcv = startEnd[0].split('.');
                QStringList endBcv = startEnd[1].split('.');
                int verseFrom = startBcv[2].toInt();
                int verseTo = endBcv[2].toInt();
                if (startBcv[0] == endBcv[0]) {
                    int book = startBcv[0].toInt();
                    if (startBcv[1] == endBcv[1]) {
                        int chapter = startBcv[1].toInt();
                        qbv::TabbedLocation loc = { -1, book, chapter, verseFrom, verseTo };
                        ParseMultiVerseCrossReference(moduleIdx, loc, results, refs);

                    } else {
                        int chapterFrom = startBcv[1].toInt();
                        int chapterTo = endBcv[1].toInt();
                        qbv::TabbedLocation loc = { -1, book, chapterFrom, verseFrom, verseTo };
                        ParseMultiChapterCrossReference(moduleIdx, loc, chapterTo, results, refs);
                    }
                } else {
                    int bookFrom = startBcv[0].toInt();
                    int bookTo = endBcv[1].toInt();
                    int chapterFrom = startBcv[1].toInt();
                    int chapterTo = endBcv[1].toInt();
                    qbv::TabbedLocation loc = { -1, bookFrom, chapterFrom, verseFrom, verseTo };
                    ParseMultiBookCrossReference(moduleIdx, loc, bookTo, chapterTo, results, refs);
                }
            } else {
               ParseSingleVerseCrossReference(moduleIdx, xRef, results, refs);
            }
        }
        ui_CrossRefBrowser->SetResults(results, refs, 0, results.count());
    }
}

void CrossReferenceBox::SetUiTexts()
{
    ui_Label_CrossRefBox->setText(tr("Cross-References:"));
}

void CrossReferenceBox::ParseMultiBookCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, int bookTo,
                                                     int chapterTo, QStringList &results, QStringList &refs)
{
    QSqlQuery query(m_pModules->at(moduleIdx).translation);
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3")
               .arg(loc.BookAsStr(), loc.ChapterAsStr(), loc.VerseAsStr()));
    QString result;
    bool isFirst = true;
    while (query.next()) {
        QString space = isFirst ? "" : " ";
        result += space % query.record().value("Scripture").toString();
        isFirst = false;
    }
    query.exec(QString("SELECT MAX(Chapter) from Bible WHERE Book = " % loc.BookAsStr()));
    if (query.next()) {
        int maxChapterFrom = query.record().value(0).toInt();
        if (maxChapterFrom > loc.chapter) {
            query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter > %2")
                       .arg(loc.BookAsStr(), loc.ChapterAsStr()));
            while (query.next()) {
                result += " " % query.record().value("Scripture").toString();
            }
        }
    }
    if (bookTo - loc.book > 1) {
        query.exec(QString("SELECT Scripture FROM Bible WHERE Book > %1 AND Book < %2")
                   .arg(loc.BookAsStr(), QString::number(bookTo)));
        while (query.next()) {
            result += " " % query.record().value("Scripture").toString();
        }
    }
    if (chapterTo > 1) {
        query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter < %2")
                   .arg(QString::number(bookTo), QString::number(chapterTo)));
        while (query.next()) {
            result += " " % query.record().value("Scripture").toString();
        }
    }
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse <= %3")
               .arg(QString::number(bookTo), QString::number(chapterTo), loc.EndVerseAsStr()));
    while (query.next()) {
        result += " " % query.record().value("Scripture").toString();
    }
    results << result;
    refs << QString("<b><a href='#' style='text-decoration:none'>—%1 %2:%3-%4 %5:%6</a></b>")
            .arg(m_pBookNames->at(loc.book - 1), loc.ChapterAsStr(), loc.VerseAsStr(),
                 m_pBookNames->at(bookTo - 1), QString::number(chapterTo), loc.EndVerseAsStr());
}

void CrossReferenceBox::ParseMultiChapterCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, int chapterTo,
                                                        QStringList &results, QStringList &refs)
{
    QSqlQuery query(m_pModules->at(moduleIdx).translation);
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3")
               .arg(loc.BookAsStr(), loc.ChapterAsStr(), loc.VerseAsStr()));
    QString result;
    bool isFirst = true;
    while (query.next()) {
        QString space = isFirst ? "" : " ";
        result += space % query.record().value("Scripture").toString();
        isFirst = false;
    }
    if (chapterTo - loc.chapter > 1) {
        query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter > %2 AND Chapter < %3")
                   .arg(loc.BookAsStr(), loc.ChapterAsStr(), QString::number(chapterTo)));
        while (query.next()) {
            result += " " % query.record().value("Scripture").toString();
        }
    }
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse <= %3")
               .arg(loc.BookAsStr(), QString::number(chapterTo), loc.EndVerseAsStr()));
    while (query.next()) {
        result += " " % query.record().value("Scripture").toString();
    }
    results << result;
    refs << QString("<b><a href='#' style='text-decoration:none'>—%1 %2:%3-%4:%5</a></b>")
            .arg(m_pBookNames->at(loc.book - 1), loc.ChapterAsStr(), loc.VerseAsStr(), QString::number(chapterTo),
                 loc.EndVerseAsStr());
}

void CrossReferenceBox::ParseMultiVerseCrossReference(int moduleIdx, const qbv::TabbedLocation &loc, QStringList &results,
                                                      QStringList &refs)
{
    QSqlQuery query(m_pModules->at(moduleIdx).translation);
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3 AND Verse <= %4")
               .arg(loc.BookAsStr(), loc.ChapterAsStr(), loc.VerseAsStr(), loc.EndVerseAsStr()));
    QString result;
    bool isFirst = true;
    while (query.next()) {
        QString space = isFirst ? "" : " ";
        result += space % query.record().value("Scripture").toString();
        isFirst = false;
    }
    results << result;
    refs << QString("<b><a href='#' style='text-decoration:none'>—%1 %2:%3-%4</a></b>")
            .arg(m_pBookNames->at(loc.book - 1), loc.ChapterAsStr(), loc.VerseAsStr(),  loc.EndVerseAsStr());
}

void CrossReferenceBox::ParseSingleVerseCrossReference(int moduleIdx, const QString &xRef, QStringList &results,
                                                       QStringList &refs)
{
    QStringList bcv = xRef.split('.');
    QSqlQuery query(m_pModules->at(moduleIdx).translation);
    query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse = %3")
               .arg(bcv[0], bcv[1], bcv[2]));
    if (query.next()) {
        results << query.record().value("Scripture").toString();
        refs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                .arg(bcv[0], bcv[1], bcv[2], m_pBookNames->at(bcv[0].toInt() - 1), bcv[1], bcv[2]);
    }
}

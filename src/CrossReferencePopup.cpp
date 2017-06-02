#include "hdr/CrossReferencePopup.h"
#include "ui_CrossReferencePopup.h"

CrossReferencePopup::CrossReferencePopup(QSqlDatabase db,
                                         QStringList hrefBookChapter,
                                         QStringList books,
                                         QFont font,
                                         QWidget* parent)
    : QDialog(parent), ui(new Ui::CrossReferencePopup)
{
    ui->setupUi(this);
    ui->textBrowser->setFont(font);
    QStringList hrefSplit = hrefBookChapter[0].split(":");
    QString bookName = books[hrefBookChapter[1].toInt()];
    QString chapter = hrefBookChapter[2];
    QString verse = hrefSplit[1];
    QString title = bookName + " " + chapter + ":" + verse;
    QDialog::setWindowTitle(title);
    loadPassages(db, hrefSplit[2], books);
}

QString formatReferences(QString text, QRegExp rfRegex)
{
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>").replace("<Fr>", "</font>");
    text.remove(rfRegex);
    //text.remove(QRegExp("<TS>.*<Ts>"));
    return text;
}

QString formatStrong(QString text)
{
    QRegularExpression regex("<W[HG][0-9]{1,4}>");
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            QString modified = original.mid(2, original.size() - 3);
            text.replace(original, " <a href=\"" + modified + "\">" + modified + "</a>");
        }
    }
    return text;
}

void CrossReferencePopup::loadPassages(QSqlDatabase db, QString passageString, QStringList bookNames)
{
    QStringList passageList = passageString.split(",");
    QSqlQuery query(db);
    QString references;
    QRegExp rfRegex = QRegExp("<RF>[^<]*<Rf>");
    for (int i = 0; i < passageList.count(); ++i) {
        if (!passageList[i].contains("-"))
        {
            QStringList bookChapterVerse = passageList[i].split(".");
            QString book = bookChapterVerse[0];
            QString chapter = bookChapterVerse[1];
            QString verse = bookChapterVerse[2];
            QString queryString = "SELECT Scripture FROM Bible"
                                  " WHERE Book = " + book +
                                  " AND Chapter = " + chapter +
                                  " AND Verse = " + verse;
            query.exec(queryString);
            if (query.next())
                references += formatReferences(query.record().value(0).toString(), rfRegex) +
                              QString("<a href=\"%1,%2,%3\" style='text-decoration: none'><b>—%4 %5:%6</b></a><br><br>")
                              .arg(book, chapter, verse, bookNames[book.toInt() - 1], chapter, verse);
        } else {
            QStringList fromTo = passageList[i].split("-");
            QStringList bookChapterVerseFrom = fromTo[0].split(".");
            QStringList bookChapterVerseTo = fromTo[1].split(".");
            QString book = bookChapterVerseFrom[0];
            QString chapterFrom = bookChapterVerseFrom[1];
            QString verseFrom = bookChapterVerseFrom[2];
            QString chapterTo = bookChapterVerseTo[1];
            QString verseTo = bookChapterVerseTo[2];
            if (chapterFrom == chapterTo) {
                QString queryString = "SELECT Scripture FROM Bible"
                                      " WHERE Book = " + book +
                                      " AND Chapter = " + chapterFrom  +
                                      " AND Verse >= " + verseFrom +
                                      " AND Verse <= " + verseTo;
                query.exec(queryString);
                QString passage;
                while (query.next())
                    passage += query.record().value(0).toString() + " ";
                references += formatReferences(passage.left(passage.size() - 1), rfRegex) +
                        QString("<a href=\"%1,%2,%3-%4\" style='text-decoration: none'><b>—%5 %6:%7-%8</b></a><br><br>")
                        .arg(book, chapterFrom, verseFrom, verseTo,
                        bookNames[book.toInt() - 1], chapterFrom, verseFrom, verseTo);
            } else {
                QString queryString = "SELECT Scripture FROM Bible"
                                      " WHERE Book = " + book +
                                      " AND Chapter = " + chapterFrom  +
                                      " AND Verse >= " + verseFrom;
                query.exec(queryString);
                QString passage;
                while (query.next())
                    passage += query.record().value(0).toString() + " ";
                queryString = "SELECT Scripture FROM Bible"
                              " WHERE Book = " + book +
                              " AND Chapter = " + chapterTo  +
                              " AND Verse <= " + verseTo;
                query.exec(queryString);
                while (query.next())
                    passage += query.record().value(0).toString() + " ";
                references += formatReferences(passage.left(passage.size() - 1), rfRegex) +
                        QString("<a href=\"%1,%2,%3-%4,%5\" style='text-decoration: none'>")
                        .arg(book, chapterFrom, verseFrom, chapterTo, verseTo ) +
                        QString("<b>—%1 %2:%3-%4:%5</b></a><br><br>")
                        .arg(bookNames[book.toInt() - 1], chapterFrom, verseFrom, chapterTo, verseTo);
            }
        }

        ui->textBrowser->setHtml(formatStrong(references));
    }
}


CrossReferencePopup::~CrossReferencePopup()
{
    delete ui;
}


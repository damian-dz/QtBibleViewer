#include "hdr/CrossReferencePopup.h"
#include "ui_CrossReferencePopup.h"

#include "hdr/StrongPopup.h"

#include <QStringBuilder>
#include <QTime>

CrossReferencePopup::CrossReferencePopup(const QPair<QSqlDatabase, const QSqlDatabase &> &dbs,
                                         const QStringList &hrefBookChapter,
                                         const QStringList &books,
                                         const QFont &font,
                                         QWidget* parent)
    : QDialog(parent),
      ui(new Ui::CrossReferencePopup)
{
    ui->setupUi(this);
    ui->textBrowser->setFont(font);
    QStringList hrefSplit = hrefBookChapter[0].split(":");
    QString bookName = books[hrefBookChapter[1].toInt()];
    QString chapter = hrefBookChapter[2];
    QString verse = hrefSplit[1];
    QString title = bookName + " " + chapter + ":" + verse;
    QDialog::setWindowTitle(title);
    QTime t;
    t.start();
    loadPassages(dbs.first, hrefSplit[2], books);
    qDebug() << t.elapsed();
    dbDct = &dbs.second;
    this->font = font;
}

CrossReferencePopup::~CrossReferencePopup()
{
    delete ui;
}

void CrossReferencePopup::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        StrongPopup strongDialog(*dbDct, argString, font, this);
        strongDialog.exec();
    }
}

QString formatReferences(QString text, const QRegExp &rfRegex)
{
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(rfRegex);
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
            text.replace(original, QStringLiteral(" <a href ='") %
                         modified % QStringLiteral("'>") % modified %
                         QStringLiteral("</a>"));
        }
    }
    return text;
}

void CrossReferencePopup::loadPassages(const QSqlDatabase &db,
                                       const QString &passageString,
                                       const QStringList &bookNames)
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
            query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse = %3")
                       .arg(book, chapter, verse));
            if (query.next())
                references += formatReferences(query.record().value(0).toString(), rfRegex) %
                              QStringLiteral("<a href=\"%1,%2,%3\" style='text-decoration:none'><b>—%4 %5:%6</b></a><br><br>").arg(book, chapter, verse, bookNames[book.toInt() - 1], chapter, verse);
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
                query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3 AND Verse <= %4").arg(book, chapterFrom, verseFrom, verseTo));
                QString passage;
                while (query.next())
                    passage += query.record().value(0).toString() + " ";
                references += formatReferences(passage.left(passage.size() - 1), rfRegex) %
                        QStringLiteral("<a href='%1,%2,%3-%4' style='text-decoration:none'><b>—%5 %6:%7-%8</b></a><br><br>")
                        .arg(book, chapterFrom, verseFrom, verseTo,
                        bookNames[book.toInt() - 1], chapterFrom, verseFrom, verseTo);
            } else {
                query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3")
                           .arg(book, chapterFrom, verseFrom));
                QString passage;
                while (query.next())
                    passage += query.record().value(0).toString() % " ";
                query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse <= %3")
                           .arg(book, chapterTo, verseTo));
                while (query.next())
                    passage += query.record().value(0).toString() % " ";
                references += formatReferences(passage.left(passage.size() - 1), rfRegex) %
                        QStringLiteral("<a href='%1,%2,%3-%4,%5' style='text-decoration:none'>")
                        .arg(book, chapterFrom, verseFrom, chapterTo, verseTo ) %
                        QStringLiteral("<b>—%1 %2:%3-%4:%5</b></a><br><br>")
                        .arg(bookNames[book.toInt() - 1], chapterFrom, verseFrom, chapterTo, verseTo);
            }
        }
        ui->textBrowser->setHtml(formatStrong(references));
    }
}

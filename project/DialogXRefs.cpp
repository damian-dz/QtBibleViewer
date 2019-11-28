#include "DialogXRefs.h"

DialogXRefs::DialogXRefs(const QSqlDatabase &dbBib,
                         const QStringList &verseInfo,
                         const QStringList &bookNames,
                         const QPixmap &background,
                         bool useBckgrnd,
                         const QFont &font,
                         QWidget *parent)
    : QDialog(parent)
{
    generateMainLayout(font);
    if (useBckgrnd) {
        setBrowserBackground(background);
    }
    QStringList verseInfoSplit = verseInfo[0].split(":");
    QString bookName = bookNames[verseInfo[1].toInt()];
    QString chapter = verseInfo[2];
    QString verse = verseInfoSplit[1];
    QString title = bookName + " " + chapter + ":" + verse;
    QWidget::setWindowTitle(title);
    loadPassages(dbBib, verseInfoSplit[2], bookNames);
}

DialogXRefs::~DialogXRefs()
{

}

void DialogXRefs::generateMainLayout(const QFont &font)
{
    ui_TextBrowser_Main = new QTextBrowser;
    ui_TextBrowser_Main->setFont(font);
    ui_TextBrowser_Main->setOpenLinks(false);

    QVBoxLayout *mainVerLayout = new QVBoxLayout;
    mainVerLayout->setContentsMargins(5, 5, 5 ,5);
    mainVerLayout->addWidget(ui_TextBrowser_Main);

    QWidget::resize(640, 480);
    QWidget::setMinimumSize(300, 200);
    QWidget::setLayout(mainVerLayout);
}

void DialogXRefs::setBrowserBackground(const QPixmap &background)
{
    QPalette palette;
    palette.setBrush(ui_TextBrowser_Main->viewport()->backgroundRole(), QBrush(background));
    ui_TextBrowser_Main->viewport()->setPalette(palette);
}

QString formatReferences(QString text, const QRegularExpression &refRgx)
{
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"))
            .replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"))
            .replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(refRgx);
    return text;
}

QString formatStrong(QString text)
{
    QRegularExpression regex("<W[HG]\\d{1,4}>");
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);
    while (iter.hasNext()) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString original = match.captured(0);
            QString modified = original.mid(2, original.size() - 3);
            text.replace(original, " <a href ='" % modified % "'>" % modified % "</a>");
        }
    }
    return text;
}

void DialogXRefs::loadPassages(const QSqlDatabase &db, const QString &passageStr, const QStringList &bookNames)
{
    QStringList passageList = passageStr.split(",");
    QSqlQuery query(db);
    QString references;
    QRegularExpression refRgx("<RF>[^<]*<Rf>");
    for (int i = 0; i < passageList.count(); ++i) {
        if (!passageList[i].contains("-")) {
            QStringList bookChapterVerse = passageList[i].split(".");
            QString book = bookChapterVerse[0];
            QString chapter = bookChapterVerse[1];
            QString verse = bookChapterVerse[2];
            if (query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse = %3")
                           .arg(book, chapter, verse))) {
                if (query.next()) {
                    references += formatReferences(query.record().value(0).toString(), refRgx) %
                            QString("<a href='%1,%2,%3' style='text-decoration:none'><b>—%4 %5:%6</b></a><br><br>")
                            .arg(book, chapter, verse, bookNames[book.toInt() - 1], chapter, verse);
                }
            }
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
                if (query.exec(QStringLiteral("SELECT Scripture FROM Bible "
                                              "WHERE Book = %1 "
                                              "AND Chapter = %2 "
                                              "AND Verse >= %3 "
                                              "AND Verse <= %4").arg(book, chapterFrom, verseFrom, verseTo))) {
                    QString passage;
                    while (query.next()) {
                        passage += query.record().value(0).toString() % " ";
                    }
                    references += formatReferences(passage.left(passage.size() - 1), refRgx) %
                            QString("<a href='%1,%2,%3-%4' style='text-decoration:none'><b>—%5 %6:%7-%8</b></a><br><br>")
                            .arg(book, chapterFrom, verseFrom, verseTo,
                                 bookNames[book.toInt() - 1], chapterFrom, verseFrom, verseTo);
                }
            } else {
                if (!query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse >= %3")
                                .arg(book, chapterFrom, verseFrom))) {
                    return;
                }
                QString passage;
                while (query.next()) {
                    passage += query.record().value(0).toString() % " ";
                }
                if (!query.exec(QStringLiteral("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = %2 AND Verse <= %3")
                                .arg(book, chapterTo, verseTo))) {
                    return;
                }
                while (query.next()) {
                    passage += query.record().value(0).toString() % " ";
                }
                references += formatReferences(passage.left(passage.size() - 1), refRgx) %
                        QStringLiteral("<a href='%1,%2,%3-%4,%5' style='text-decoration:none'>")
                        .arg(book, chapterFrom, verseFrom, chapterTo, verseTo) %
                        QString("<b>—%1 %2:%3-%4:%5</b></a><br><br>")
                        .arg(bookNames[book.toInt() - 1], chapterFrom, verseFrom, chapterTo, verseTo);
            }
        }
        ui_TextBrowser_Main->setHtml(formatStrong(references));
    }
}

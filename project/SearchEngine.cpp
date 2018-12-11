#include "MainWindow.h"

QString multipleWordQueryString(const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj)
{
    QString queryString = "SELECT * FROM Bible"
                          " WHERE (LOWER(Scripture) LIKE '%" % wordsLow[0] % "%'"
                          " OR UPPER(Scripture) LIKE '%" % wordsUpp[0] % "%')";
    for (int i = 1; i < wordsLow.count(); ++i) {
        queryString += " " + conj + " (LOWER(Scripture) LIKE '%" % wordsLow[i] % "%'"
                                    " OR UPPER(Scripture) LIKE '%" % wordsUpp[i] % "%')";
    }
    return queryString;
}

bool containsAllWords(const QString &text, const QList<QRegExp> &words)
{
    bool hasAll = true;
    for (int i = 0; i < words.count(); ++i) {
        if (!text.contains(words[i])) {
            hasAll = false;
            break;
        }
    }
    return hasAll;
}

bool containsAnyWord(const QString &text, const QList<QRegExp> &words)
{
    bool hasAny = false;
    for (int i = 0; i < words.count(); ++i) {
        if (text.contains(words[i])) {
            hasAny = true;
            break;
        }
    }
    return hasAny;
}

void MainWindow::performSearch()
{
    QTime watch;
    watch.start();
    QString text = ui_Sea_LineEdit_Search->text();
    QString textLow = text.toLower();
    QString textUpp = text.toUpper();
    QStringList words = text.split(" ");
    QStringList wordsLow = textLow.split(" ");
    QStringList wordsUpp = textUpp.split(" ");
    int bookFirst = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
    int bookLast = ui_Sea_ComboBox_SearchTo->currentIndex() + 1;
    QString conj = ui_Sea_RadioButton_Any->isChecked() ? "OR" : "AND";
    Qt::CaseSensitivity sensitivity = ui_Sea_CheckBox_Case->isChecked() ?
                                      Qt::CaseSensitive : Qt::CaseInsensitive;
    bool wholeWords = ui_Sea_CheckBox_WholeWords->isChecked() ? true : false;
    bool containsAll = ui_Sea_RadioButton_All->isChecked() ? true : false;
    QString queryString = multipleWordQueryString(wordsLow, wordsUpp, conj) %
                          " AND Book >= " % QString::number(bookFirst) %
                          " AND Book <= " % QString::number(bookLast);
    int idx = ui_Sea_ComboBox_Translation->currentIndex();
    QSqlQuery query(m_modules[idx].database);
    if (query.exec(queryString)) {
        m_resVerses.clear();
        m_resRefs.clear();
        if (ui_Sea_RadioButton_Exact->isChecked()) {
            iterateRecords(query, text, sensitivity, wholeWords, m_modules[idx].hasStrong);
        } else {
            iterateRecords(query, words, sensitivity, wholeWords, containsAll);
        }
        m_elapsedTime = QString::number(watch.elapsed() / 1000.) % " s";
        m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
        displaySearchResults(0, m_numResPerPage);
        m_crntStartRes = m_numResPerPage;
        ui_Sea_Button_Prev->setDisabled(true);
        ui_Sea_Button_Next->setEnabled(m_resVerses.count() > m_numResPerPage);
    }
}

void MainWindow::performSearchByStrong()
{
    QTime watch;
    watch.start();
    QString text = ui_Sea_LineEdit_Search->text().toUpper();
    m_dispRgx = QRegExp("\\b" % text % "\\b", Qt::CaseSensitive);
    text = "<W" % text % ">";
    int bookFirst = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
    int bookLast = ui_Sea_ComboBox_SearchTo->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible WHERE Scripture LIKE '%" % text % "%'"
                          " AND Book >= " % QString::number(bookFirst) +
                          " AND Book <= " % QString::number(bookLast);
    int idx = ui_Sea_ComboBox_Translation->currentIndex();
    QSqlQuery query(m_modules[idx].database);
    if (query.exec(queryString)) {
        m_resVerses.clear();
        m_resRefs.clear();
        while (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            m_resVerses << scripture;
            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
        }
        m_elapsedTime = QString::number(watch.elapsed() / 1000.) % " s";
        m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
        displaySearchResults(0, m_numResPerPage);
        m_crntStartRes = m_numResPerPage;
        ui_Sea_Button_Prev->setDisabled(true);
        ui_Sea_Button_Next->setEnabled(m_resVerses.count() > m_numResPerPage);
    }
}

typedef bool (&AllAny)(const QString &text, const QList<QRegExp> &words);

void MainWindow::iterateRecords(QSqlQuery &query, const QStringList &words,
                                Qt::CaseSensitivity sensitivity, bool wholeWords, bool containsAll)
{
    AllAny containsFunc = containsAll ? containsAllWords : containsAnyWord;
    QString boundary = wholeWords ? "\\b" : "";
    QList<QRegExp> wordsRgx;
    for (int i = 0; i < words.count(); ++i) {
        wordsRgx << QRegExp(boundary % words[i] % boundary, sensitivity);
    }
    while (query.next()) {
        QSqlRecord record = query.record();
        QString book = record.value(0).toString();
        QString chapter = record.value(1).toString();
        QString verse = record.value(2).toString();
        QString scripture = record.value(3).toString();
        if (containsFunc(scripture, wordsRgx)) {
            m_resVerses << scripture;
            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
        }
    }
    QStringList dispWords;
    for (QString word : words) {
        dispWords << boundary % word % boundary;
    }
    m_dispRgx = QRegExp(dispWords.join("|"), sensitivity);
}

void MainWindow::iterateRecords(QSqlQuery &query, const QString &text,
                                Qt::CaseSensitivity sensitivity, bool wholeWords, bool hasStrong)
{
    QString boundary = wholeWords ? "\\b" : "";
    QRegExp textRgx(boundary % text % boundary, sensitivity);
    QString strong = hasStrong ? "|<W[HG][0-9]{1,4}>" : "";
    QRegExp patterns("<.{2,3}>|<RF>.*<Rf>" + strong);
    while (query.next()) {
        QSqlRecord record = query.record();
        QString rawText = record.value(3).toString();
        QString strippedText = rawText.remove(patterns);
        if (strippedText.contains(textRgx)) {
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            m_resVerses << record.value(3).toString();
            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
        }
    }
    m_dispRgx = textRgx;
}

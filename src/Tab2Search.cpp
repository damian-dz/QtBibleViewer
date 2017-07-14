#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include "hdr/AuxiliaryMethods.h"
#include "hdr/StrongPopup.h"

/* Search Tab */
void MainWindow::on_searchFromComboBox_currentIndexChanged(int index)
{
    if (languageChanging)
        return;
    if (index > ui->searchToComboBox->currentIndex())
        ui->searchToComboBox->setCurrentIndex(index);
    ui->divisionComboBox->setCurrentIndex(ui->divisionComboBox->count() - 1);
}

void MainWindow::on_searchToComboBox_currentIndexChanged(int index)
{
    if (languageChanging)
        return;
    if (index < ui->searchFromComboBox->currentIndex())
        ui->searchFromComboBox->setCurrentIndex(index);
    ui->divisionComboBox->setCurrentIndex(ui->divisionComboBox->count() - 1);
}

void MainWindow::on_divisionComboBox_currentIndexChanged(int index)
{
    ui->searchFromComboBox->blockSignals(true);
    ui->searchToComboBox->blockSignals(true);
    switch (index) {
    case 0:
        ui->searchFromComboBox->setCurrentIndex(0);
        ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
        break;
    case 1:
        ui->searchFromComboBox->setCurrentIndex(0);
        ui->searchToComboBox->setCurrentIndex(38);
        break;
    case 2:
        ui->searchFromComboBox->setCurrentIndex(0);
        ui->searchToComboBox->setCurrentIndex(4);
        break;
    case 3:
        ui->searchFromComboBox->setCurrentIndex(5);
        ui->searchToComboBox->setCurrentIndex(16);
        break;
    case 4:
        ui->searchFromComboBox->setCurrentIndex(17);
        ui->searchToComboBox->setCurrentIndex(21);
        break;
    case 5:
        ui->searchFromComboBox->setCurrentIndex(22);
        ui->searchToComboBox->setCurrentIndex(26);
        break;
    case 6:
        ui->searchFromComboBox->setCurrentIndex(27);
        ui->searchToComboBox->setCurrentIndex(38);
        break;
    case 7:
        ui->searchFromComboBox->setCurrentIndex(39);
        ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
        break;
    case 8:
        ui->searchFromComboBox->setCurrentIndex(39);
        ui->searchToComboBox->setCurrentIndex(43);
        break;
    case 9:
        ui->searchFromComboBox->setCurrentIndex(44);
        ui->searchToComboBox->setCurrentIndex(57);
        break;
    case 10:
        ui->searchFromComboBox->setCurrentIndex(58);
        ui->searchToComboBox->setCurrentIndex(ui->searchToComboBox->count() - 1);
    }
    ui->searchFromComboBox->blockSignals(false);
    ui->searchToComboBox->blockSignals(false);
}

void MainWindow::on_enterLineEdit_textChanged(const QString &arg1)
{
    ui->searchButton->setDisabled(arg1.trimmed().isEmpty());
}

QString formatResult(QString text, const QRegExp &regex, bool hasStrong)
{
    text.remove(regex);
    text.replace(QStringLiteral("<FI>"), QStringLiteral("<i>"));
    text.replace(QStringLiteral("<Fi>"), QStringLiteral("</i>"));
    text.replace(QStringLiteral("<FR>"), QStringLiteral("<font color=#C80000>"));
    text.replace(QStringLiteral("<Fr>"), QStringLiteral("</font>"));
    text.remove(QStringLiteral("<CM>"));
    if (hasStrong) {
        QRegularExpression regex("<W[^<]*>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() -3);
                text.replace(original, QStringLiteral(" <a href ='") %
                             modified % QStringLiteral("'>") % modified %
                             QStringLiteral("</a>"));
                //text.replace(original, QStringLiteral(" <a href='%1'>%2</a>").arg(modified, modified));
            }
        }
    }
    return text;
}

void MainWindow::displayResults(int startIndex, int endIndex)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int index = startIndex;
    QString resultString;
    QRegExp regex("<RF>.*<Rf>");
    while (index < verseList.count() && index < endIndex) {
        resultString += formatResult(verseList[index], regex, modules[dbIndex].hasStrong);
        resultString += referenceList[index++];
    }
    ui->resultsTextBrowser->setHtml(resultString);
    QList<QTextEdit::ExtraSelection> extraSelections;
    QColor color(Qt::yellow);
    while (ui->resultsTextBrowser->find(displayRegex)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = ui->resultsTextBrowser->textCursor();
        extraSelections.append(extra);
    }
    ui->resultsTextBrowser->setExtraSelections(extraSelections);
    regex = QRegExp("—([A-Za-złŁńó]+|[1-3] [A-Za-z]+|[A-Za-zśń]+ [A-Za-z]+ [A-Za-zśń]+) [0-9]{1,3}:[0-9]{1,3}");
    color = Qt::white;
    while (ui->resultsTextBrowser->find(regex, QTextDocument::FindBackward)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = ui->resultsTextBrowser->textCursor();
        extraSelections.append(extra);
    }
    ui->resultsTextBrowser->setExtraSelections(extraSelections);
    ui->resultsTextBrowser->moveCursor(QTextCursor::Start);
    QString statusMessage;
    if (verseList.count() == 1)
        statusMessage = tr("Verse 1 (1 in total); ") + timeElapsed;
    else
        statusMessage = tr("Verses ") + QString::number(startIndex + 1) + "-" + QString::number(index) +
            " ("  + QString::number(verseList.count()) + tr(" in total); ") + timeElapsed;
    statusLabel->setText(statusMessage);
}

void MainWindow::searchWithRegex(const QRegExp &pattern)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(rgxMarkupStrongNotes);
        if (text.contains(pattern)) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" % book % "," % chapter % "," % verse %
                             "\" style='text-decoration: none'>—" %
                             bookNames[book.toInt() - 1] % " " % chapter % ":" % verse % "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithRegex(const QList<QRegExp> &patterns, int wordCount)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(rgxMarkupStrongNotes);
        bool hasAll = true;
        for (int i = 0; i < wordCount; i++) {
            if (!text.contains(patterns[i])) {
                hasAll = false;
                break;
            }
        }
        if (hasAll) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" % book % "," % chapter % "," % verse %
                             "\" style='text-decoration: none'>—" %
                             bookNames[book.toInt() - 1] % " " + chapter % ":" % verse % "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithString(const QString &phrase, Qt::CaseSensitivity sensitivity)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    //QRegExp regexMarkup = QRegExp("<[^<]*>");
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(rgxMarkupStrongNotes);
        if (text.contains(phrase, sensitivity)) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" % book % "," % chapter % "," % verse %
                             "\" style='text-decoration: none'>—" +
                             bookNames[book.toInt() - 1] % " " % chapter % ":" % verse % "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithString(const QStringList &words, int wordCount, Qt::CaseSensitivity sensitivity)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(rgxMarkupStrongNotes);
        bool hasAll = true;
        for (int i = 0; i < wordCount; i++) {
            if (!text.contains(words[i], sensitivity)) {
                hasAll = false;
                break;
            }
        }
        if (hasAll) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" % book % "," % chapter % "," % verse %
                             "\" style='text-decoration: none'>—" %
                             bookNames[book.toInt() - 1] % " " % chapter % ":" % verse % "</a></b><br><br>";
            resultCount++;
        }
    }
}

QStringList getCombinations(const QString &text)
{
    const QString specialCharacters = "ćĆłŁóÓśŚźŹżŻ";
    QStringList textSplit = text.toLower().split(" ");
    QStringList combinationList;
    QMap<int, QChar> characterMap;
    QChar firstLetter;
    for (int pos = 0; pos < textSplit.count(); ++pos) {
        for (int i = 0; i < specialCharacters.length(); ++i) {
            firstLetter = textSplit[pos][0];
            if (specialCharacters[i] == firstLetter)
                characterMap.insert(pos, firstLetter);
        }
    }
    int totalCombinations = 2 << (characterMap.count() - 1);
    int characterCount = characterMap.count();
    for (int com = 0; com < totalCombinations; ++com) {
        QVector<int> bitVector;
        for (int i = 0; i < characterCount; ++i)
            bitVector.append((com >> i) & 1);
        QMapIterator<int, QChar> iterator(characterMap);
        while (iterator.hasNext()) {
            for (int i = 0; i < characterCount; ++i) {
                iterator.next();
                int index = iterator.key();
                if (bitVector[i] == 0)
                    firstLetter = iterator.value();
                else
                    firstLetter = iterator.value().toUpper();
                textSplit[index][0] = firstLetter;
            }
        }
        combinationList << textSplit.join(" ");
    }
    return combinationList;
}

QString getFromWhereLikeQueryString(QString text)
{
    QStringList combinationList = getCombinations(text);
    if (combinationList.count() == 0)
        return "SELECT * FROM Bible WHERE Scripture LIKE '%" + text + "%'";
    QString searchQuery = "SELECT * FROM Bible WHERE (";
    for (int i = 0; i < combinationList.count(); ++i) {
        if (i != 0)
            searchQuery += " OR ";
        searchQuery += "Scripture LIKE '%" + combinationList[i] + "%'";
        if (i == combinationList.count() - 1)
            searchQuery += ")"        ;
    }
    return searchQuery;
}


QString multipleWordsQueryString(QStringList words, QString oprtr)
{
    QStringList list = words;
    for (int i = 0; i < list.count(); ++i) {
        list[i] = list[i].toLower();
        if (AuxiliaryMethods::startsWithNonLatin(list[i]))
            list[i] = list[i].right(list[i].length() - 1);
    }
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Scripture LIKE '%" % list[0] % "%'";
    for (int i = 1; i < list.count(); ++i)
        queryString += " " + oprtr + " Scripture LIKE '%" % list[i] % "%'";
    return queryString;
}

void MainWindow::searchWithLIKE(const QString &word, Qt::CaseSensitivity sensitivity)
{
    qDebug() << "searchWithLIKE(const QString &word, Qt::CaseSensitivity sensitivity)";
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = AuxiliaryMethods::singleWordQueryString(word, "*") %
                          " AND Book >= " % QString::number(bookFirstNumber) %
                          " AND Book <= " % QString::number(bookLastNumber);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    while (query.next()) {
        QSqlRecord record = query.record();
        QString book = record.value(0).toString();
        QString chapter = record.value(1).toString();
        QString verse = record.value(2).toString();
        QString scripture = record.value(3).toString();
        QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
        if (scriptureNoNotes.contains(word, sensitivity)) {
            verseList << scripture;
            referenceList << QStringLiteral("<b><a href='") % book %
                             QStringLiteral(",") % chapter %
                             QStringLiteral(",") % verse %
                             QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                             QStringLiteral(" ") % chapter %
                             QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
        }
    }
}

bool containsAllWords(QString text, const QStringList &words, Qt::CaseSensitivity sensitivity)
{
    bool hasAll = true;
    for (int i = 0; i < words.count(); ++i) {
        if (!text.contains(words[i], sensitivity)) {
            hasAll = false;
            break;
        }
    }
    return hasAll;
}

bool containsAllWords(QString text, const QList<QRegExp> &words)
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

bool containsAnyWord(QString text, const QStringList &words, Qt::CaseSensitivity sensitivity)
{
    bool hasAny = false;
    for (int i = 0; i < words.count(); ++i) {
        if (text.contains(words[i], sensitivity)) {
            hasAny = true;
            break;
        }
    }
    return hasAny;
}

bool containsAnyWord(QString text, const QList<QRegExp> &words)
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

void MainWindow::searchWithLIKE(const QStringList &words, Qt::CaseSensitivity sensitivity, QString oprtr)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = multipleWordsQueryString(words, oprtr) +
                          " AND Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    if (oprtr == "AND") {
        while (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
            if (containsAllWords(scriptureNoNotes, words, sensitivity)) {
                verseList << scripture;
                referenceList << QStringLiteral("<b><a href='") % book %
                                 QStringLiteral(",") % chapter %
                                 QStringLiteral(",") % verse %
                                 QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                                 QStringLiteral(" ") % chapter %
                                 QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
            }
        }
    } else if (oprtr == "OR") {
        while (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
            if (containsAnyWord(scriptureNoNotes, words, sensitivity)) {
                verseList << scripture;
                referenceList << QStringLiteral("<b><a href='") % book %
                                 QStringLiteral(",") % chapter %
                                 QStringLiteral(",") % verse %
                                 QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                                 QStringLiteral(" ") % chapter %
                                 QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
            }
        }
    }
}

void MainWindow::searchWithLIKE(const QList<QRegExp> &wordsRgx, const QStringList &words, QString oprtr)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = multipleWordsQueryString(words, oprtr) +
                          " AND Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    if (oprtr == "AND") {
        while (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
            if (containsAllWords(scriptureNoNotes, wordsRgx)) {
                verseList << scripture;
                referenceList << QStringLiteral("<b><a href='") % book %
                                 QStringLiteral(",") % chapter %
                                 QStringLiteral(",") % verse %
                                 QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                                 QStringLiteral(" ") % chapter %
                                 QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
            }
        }
    } else if (oprtr == "OR") {
        while (query.next()) {
            QSqlRecord record = query.record();
            QString book = record.value(0).toString();
            QString chapter = record.value(1).toString();
            QString verse = record.value(2).toString();
            QString scripture = record.value(3).toString();
            QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
            if (containsAnyWord(scriptureNoNotes, wordsRgx)) {
                verseList << scripture;
                referenceList << QStringLiteral("<b><a href='") % book %
                                 QStringLiteral(",") % chapter %
                                 QStringLiteral(",") % verse %
                                 QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                                 QStringLiteral(" ") % chapter %
                                 QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
            }
        }
    }
}

void MainWindow::searchWithLIKE(const QRegExp &text, const QStringList &words)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = multipleWordsQueryString(words, "AND") +
                          " AND Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    QRegExp rgxMarkupStrongNotes("<..>|<W[HG][0-9]{1,4}>|<RF>.*<Rf>");
    while (query.next()) {
        QSqlRecord record = query.record();
        QString book = record.value(0).toString();
        QString chapter = record.value(1).toString();
        QString verse = record.value(2).toString();
        QString scripture = record.value(3).toString();
        QString scriptureNoNotes = scripture.remove(rgxMarkupStrongNotes);
        if (scriptureNoNotes.contains(text)) {
            verseList << query.record().value(3).toString();
            referenceList << QStringLiteral("<b><a href='") % book %
                             QStringLiteral(",") % chapter %
                             QStringLiteral(",") % verse %
                             QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                             QStringLiteral(" ") % chapter %
                             QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
        }

    }
}

void MainWindow::searchByStrong(const QString &number)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Scripture LIKE '%<W" + number + ">%'" +
                          " AND Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QSqlQuery query(modules[dbIndex].database);
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        QString book = record.value(0).toString();
        QString chapter = record.value(1).toString();
        QString verse = record.value(2).toString();
        verseList << record.value(3).toString();
        referenceList << QStringLiteral("<b><a href='") % book %
                         QStringLiteral(",") % chapter %
                         QStringLiteral(",") % verse %
                         QStringLiteral("' style='text-decoration:none'>—") % bookNames[book.toInt() - 1] %
                         QStringLiteral(" ") % chapter %
                         QStringLiteral(":") % verse % QStringLiteral("</a></b><br><br>");
    }

}

void MainWindow::on_searchButton_clicked()
{
    ui->searchButton->setDisabled(true);
    resultsPerPage = ui->resultsPerPageComboBox->currentText().toInt();
    Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
    if (ui->caseSensitiveCheckBox->isChecked())
        sensitivity = Qt::CaseSensitive;
    enteredText = ui->enterLineEdit->text().simplified();
    QStringList searchWordList = enteredText.split(" ");
    int wordCount = searchWordList.count();
    verseList.clear();
    referenceList.clear();
    QTime watch;
    watch.start();
    if (wordCount > 1) {
        if (ui->exactPhraseRadioButton->isChecked()) {
            float avgCharCount = 0;
            for (int i = 0; i < wordCount; ++i)
                avgCharCount += searchWordList[i].length();
            avgCharCount /= wordCount;
            if (ui->wholeWordsCheckBox->isChecked()) {
               QRegExp regex("\\b" + enteredText + "\\b", sensitivity);
               if (avgCharCount > 3)
                   searchWithLIKE(regex, searchWordList);
               else
                   searchWithRegex(regex);
                displayRegex = regex;
            } else {
                QRegExp regex(enteredText, sensitivity);
                if (avgCharCount > 3)
                    searchWithLIKE(regex, searchWordList);
                else
                    searchWithString(enteredText, sensitivity);
                displayRegex = regex;
            }
        } else if (ui->allWordsRadioButton->isChecked()) {
            if (ui->wholeWordsCheckBox->isChecked()) {
                QString displayRegexString;
                QList<QRegExp> regexList;
                displayRegexString += "\\b";
                for (int i = 0; i < wordCount - 1; i++) {
                    displayRegexString += searchWordList[i] + "\\b|\\b";
                    regexList << QRegExp("\\b" + searchWordList[i] + "\\b", sensitivity);
                }
                displayRegexString += searchWordList[wordCount - 1] + "\\b";
                regexList << QRegExp("\\b" + searchWordList[wordCount - 1] + "\\b", sensitivity);
                searchWithLIKE(regexList, searchWordList, "AND");
                //searchWithRegex(regexList, wordCount);
                displayRegex = QRegExp(displayRegexString, sensitivity);
            } else {
                QString displayRegexString;
                for (int i = 0; i < wordCount - 1; ++i)
                    displayRegexString += searchWordList[i] + "|";
                displayRegexString += searchWordList[wordCount - 1];
                searchWithLIKE(searchWordList, sensitivity, "AND");
                //searchWithString(searchWordList, wordCount, sensitivity);
                displayRegex = QRegExp(displayRegexString, sensitivity);
            }
        } else if (ui->anyWordsRadioButton->isChecked()) {
            QString regexString;
            QList<QRegExp> regexList;
            if (ui->wholeWordsCheckBox->isChecked()) {
                regexString += "\\b";
                for (int i = 0; i < wordCount - 1; ++i) {
                    regexString += searchWordList[i] + "\\b|\\b";
                    regexList << QRegExp("\\b" + searchWordList[i] + "\\b", sensitivity);
                }
                regexString += searchWordList[wordCount - 1] + "\\b";
                regexList << QRegExp("\\b" + searchWordList[wordCount - 1] + "\\b", sensitivity);
            } else {
                for (int i = 0; i < wordCount - 1; ++i) {
                    regexString += searchWordList[i] + "|";
                    regexList << QRegExp(searchWordList[i], sensitivity);
                }
                regexString += searchWordList[wordCount - 1];
                regexList << QRegExp(searchWordList[wordCount - 1], sensitivity);
            }
            QRegExp regex(regexString, sensitivity);
            searchWithLIKE(regexList, searchWordList, "OR");
            //searchWithRegex(regex);
            displayRegex = regex;
        }
    } else if (!ui->byStrongsNumberRadioButton->isChecked()) {
        if (ui->wholeWordsCheckBox->isChecked()) {
            QRegExp regex("\\b" + enteredText + "\\b", sensitivity);
            if (enteredText.length() > 3)
                searchWithLIKE(regex, searchWordList);
            else
                searchWithRegex(regex);
            displayRegex = regex;
        } else {
            QRegExp regex(enteredText, sensitivity);
            if (enteredText.length() > 3)
                searchWithLIKE(regex, searchWordList);
            else
                searchWithLIKE(enteredText, sensitivity);
            displayRegex = regex;
        }
    } else {
        searchByStrong(enteredText);
        displayRegex = QRegExp("\\b" + enteredText + "\\b", Qt::CaseInsensitive);
    }
    timeElapsed = QString::number(watch.elapsed() / 1000.0) + " s";
    if (verseList.count() == 0) {
        ui->resultsTextBrowser->clear();
        statusLabel->setText(tr("No matches; ") + timeElapsed);
    } else
        displayResults(0, resultsPerPage);
    pageNumber = 1;
    ui->prevButton->setDisabled(true);
    ui->nextButton->setEnabled(verseList.count() > resultsPerPage);
    ui->searchButton->setEnabled(true);
}

void MainWindow::on_enterLineEdit_returnPressed()
{
    on_searchButton_clicked();
}

void MainWindow::on_prevButton_clicked()
{
    pageNumber -= 2;
    int startIndex = resultsPerPage * pageNumber;
    pageNumber++;
    int endIndex = resultsPerPage * pageNumber;
    ui->prevButton->setDisabled(pageNumber == 1);
    ui->nextButton->setEnabled(endIndex < verseList.count());
    displayResults(startIndex, endIndex);
}

void MainWindow::on_nextButton_clicked()
{
    ui->prevButton->setEnabled(true);
    int startIndex = resultsPerPage * pageNumber;
    pageNumber++;
    int endIndex = resultsPerPage * pageNumber;
    ui->nextButton->setDisabled(endIndex >= verseList.count());
    displayResults(startIndex, endIndex);
}

void MainWindow::on_resultsTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar firstChar = argString[0];
    if (firstChar == 'H' || firstChar == 'G') {
        QFont font;
        if (!fontFamily.isEmpty())
           font.setFamily(fontFamily);
        font.setPointSize(fontSize);
        StrongPopup strong(dbDct, argString, font, this);
        strong.exec();
    } else {
        int dbIndex = ui->translationComboBox->currentIndex();
        ui->tabWidget->setCurrentIndex(0);
        ui->translationTabWidget->setCurrentIndex(dbIndex);
        QStringList indices = argString.split(",");
        highlightPassage(indices, dbIndex);
    }
}

void MainWindow::highlightPassage(const QStringList &indices, int dbIndex)
{
    int book = indices[0].toInt() - 1;
    int chapter = indices[1].toInt() - 1;
    int verse = indices[2].toInt();
    ui->bookListWidget->setCurrentRow(book);
    ui->chapterListWidget->setCurrentRow(chapter);
    QString plainText = chapterBrowsers[dbIndex]->toPlainText();
    QRegExp startRgx = QRegExp("\\b" + QString::number(verse) + " ");
    int start = plainText.indexOf(startRgx);
    int verseCount = ui->verseLastComboBox->count();
    int *verseNumbers = new int[verseCount];
    for (int i = 0; i < verseCount; ++i)
        verseNumbers[i] = ui->verseLastComboBox->itemText(i).toInt();
    int limit = -100;
    for (int i = 0; i < verseCount; ++i) {
        if (verseNumbers[i] > verse) {
            limit = verseNumbers[i];
            break;
        }
    }
    delete[] verseNumbers;
    QRegExp endRgx = QRegExp("\\b" + QString::number(limit) + " ");
    int end = plainText.indexOf(endRgx);
    if (end == - 1)
        end = plainText.length() - 1;
    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    chapterBrowsers[dbIndex]->setTextCursor(cursor);
    chapterBrowsers[dbIndex]->setFocus();
}

void MainWindow::on_randomVerseButton_clicked()
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int min = ui->searchFromComboBox->currentIndex() + 1;
    if (min < 40 && !modules[dbIndex].hasOldTestament) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The current transation contains only the New Testament."));
        ui->divisionComboBox->setCurrentIndex(7);
        return;
    }
    int max = ui->searchToComboBox->currentIndex() + 2;
    uint seed = QTime::currentTime().msec();
    qsrand(seed);
    int bookNumber = qrand() % (max - min) + min;
    QSqlQuery query(modules[dbIndex].database);
    QString book = QString::number(bookNumber);
    QString verseText;
    while (verseText.isNull()) {
        QString queryString = "SELECT Min(Chapter), Max(Chapter) FROM Bible"
                              " WHERE Book = " + book;
        query.exec(queryString);
        if (query.next()) {
            QSqlRecord record = query.record();
            min = record.value(0).toInt();
            max = record.value(1).toInt();
        }
        max++;
        int chapterNumber = qrand() % (max - min) + min;
        QString chapter = QString::number(chapterNumber);
        queryString = "SELECT Min(Verse), Max(Verse) FROM Bible"
                      " WHERE Book = " + book +
                      " AND Chapter = " + chapter;
        query.exec(queryString);
        if (query.next()) {
            QSqlRecord record = query.record();
            min = record.value(0).toInt();
            max = record.value(1).toInt();
        }
        max++;
        int verseNumber = qrand() % (max - min) + min;
        QString verse = QString::number(verseNumber);
        queryString = "SELECT Scripture FROM Bible"
                      " WHERE Book = " + book +
                      " AND Chapter = " + chapter +
                      " AND Verse =  " + verse;
        query.exec(queryString);
        if (query.next()) {
            QString randomResult = query.record().value(0).toString();
            ui->randomVerseTextBrowser->clear();
            verseText = formatResult(randomResult, QRegExp("<RF>.*<Rf>"),
                                   modules[dbIndex].hasStrong);
            verseText += "<b><a href='" % book % "," % chapter % "," % verse %
                         "' style='text-decoration:none'>—" %
                         bookNames[bookNumber - 1] + " " %
                         chapter % ":" % verse % "</a></b>";
            ui->randomVerseTextBrowser->setHtml(verseText);
        }
    }
}

void MainWindow::on_randomVerseTextBrowser_anchorClicked(const QUrl &arg1)
{
    sentByRandom = true;
    on_resultsTextBrowser_anchorClicked(arg1);
    sentByRandom = false;
}

void MainWindow::on_translationComboBox_currentIndexChanged(int index)
{
    ui->byStrongsNumberRadioButton->setEnabled(modules[index].hasStrong);
    if (!ui->byStrongsNumberRadioButton->isEnabled()
            && ui->byStrongsNumberRadioButton->isChecked())
        ui->exactPhraseRadioButton->setChecked(true);
}

void MainWindow::on_byStrongsNumberRadioButton_toggled(bool checked)
{
    if (checked) {
        QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
        QValidator *validator = new QRegExpValidator(regex, this);
        ui->enterLineEdit->setValidator(validator);
        if (currentLanguage == "ENG")
            ui->enterLabel->setText("Enter a Number:");
        else if (currentLanguage == "PL")
            ui->enterLabel->setText("Wpisz Numer:");
    } else {
        QRegExp regex("[^\n]+");
        QValidator *validator = new QRegExpValidator(regex, this);
        ui->enterLineEdit->setValidator(validator);
        if (currentLanguage == "ENG")
            ui->enterLabel->setText("Enter a Word or Phrase:");
        else if (currentLanguage == "PL")
            ui->enterLabel->setText("Wpisz Słowo lub Frazę:");
    }
    ui->caseSensitiveCheckBox->setDisabled(checked);
    ui->wholeWordsCheckBox->setDisabled(checked);
}

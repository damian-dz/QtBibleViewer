#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

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

QString formatResult(QString text, QRegExp regex, bool hasStrong)
{
    text.remove(regex);
    text.replace("<FI>", "<i>");
    text.replace("<Fi>", "</i>");
    text.replace("<FR>", "<font color=#C80000>");
    text.replace("<Fr>", "</font>");
    text.remove("<CM>");
    if (hasStrong) {
        QRegularExpression regex("<W[^<]*>");
        QRegularExpressionMatchIterator iter = regex.globalMatch(text);
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            if (match.hasMatch()) {
                QString original = match.captured(0);
                QString modified = original.mid(2, original.size() -3);
                text.replace(original, " <a href = \"" + modified + "\">" + modified + "</a>");
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
    QRegExp regex = QRegExp("<RF>.*<Rf>");
    while (index < verseList.count() && index < endIndex) {
        resultString += formatResult(verseList[index], regex, std::get<3>(databases[dbIndex]));
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
        statusMessage = verseInTotal + timeElapsed;
    else
        statusMessage = verses + QString::number(startIndex + 1) + "-" + QString::number(index) +
            " ("  + QString::number(verseList.count()) + inTotal + timeElapsed;
    statusLabel->setText(statusMessage);
}


void MainWindow::on_enterLineEdit_textChanged(const QString &arg1)
{
    if (arg1.trimmed().isEmpty())
        ui->searchButton->setDisabled(true);
    else
        ui->searchButton->setEnabled(true);
}

void MainWindow::searchWithRegex(QRegExp pattern)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp regexNotes = QRegExp("<RF>.*<Rf>");
    QRegExp regexMarkup = QRegExp("<..>|<W[HG][0-9]{1,4}>");
    //QRegExp regexMarkup = QRegExp("<[^<]*>");
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(regexNotes);
        text.remove(regexMarkup);
        if (text.contains(pattern)) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" + book + "," + chapter + "," + verse +
                             "\" style='text-decoration: none'>—" +
                             bookNames[book.toInt() - 1] + " " + chapter + ":" + verse + "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithRegex(QList<QRegExp> patterns, int wordCount)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp regexNotes = QRegExp("<RF>.*<Rf>");
    QRegExp regexMarkup = QRegExp("<..>|<W[HG][0-9]{1,4}>");
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(regexNotes);
        text.remove(regexMarkup);
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
            referenceList << "<b><a href = \"" + book + "," + chapter + "," + verse +
                             "\" style='text-decoration: none'>—" +
                             bookNames[book.toInt() - 1] + " " + chapter + ":" + verse + "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithString(QString word, Qt::CaseSensitivity sensitivity)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp regexNotes = QRegExp("<RF>.*<Rf>");
    QRegExp regexMarkup = QRegExp("<..>|<W[HG][0-9]{1,4}>");
    //QRegExp regexMarkup = QRegExp("<[^<]*>");
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(regexNotes);
        text.remove(regexMarkup);
        if (text.contains(word, sensitivity)) {
            QString book = query.record().value(0).toString();
            QString chapter = query.record().value(1).toString();
            QString verse = query.record().value(2).toString();
            verseList << query.record().value(3).toString();
            referenceList << "<b><a href = \"" + book + "," + chapter + "," + verse +
                             "\" style='text-decoration: none'>—" +
                             bookNames[book.toInt() - 1] + " " + chapter + ":" + verse + "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::searchWithString(QStringList words, int wordCount, Qt::CaseSensitivity sensitivity)
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int bookFirstNumber = ui->searchFromComboBox->currentIndex() + 1;
    int bookLastNumber = ui->searchToComboBox->currentIndex() + 1;
    QString queryString = "SELECT * FROM Bible"
                          " WHERE Book >= " + QString::number(bookFirstNumber) +
                          " AND Book <= " + QString::number(bookLastNumber);
    QRegExp regexNotes = QRegExp("<RF>.*<Rf>");
    QRegExp regexMarkup = QRegExp("<..>|<W[HG][0-9]{1,4}>");
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    query.exec(queryString);
    int resultCount = 0;
    while (query.next()) {
        QString text = query.record().value(3).toString();
        text.remove(regexNotes);
        text.remove(regexMarkup);
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
            referenceList << "<b><a href = \"" + book + "," + chapter + "," + verse +
                             "\" style='text-decoration: none'>—" +
                             bookNames[book.toInt() - 1] + " " + chapter + ":" + verse + "</a></b><br><br>";
            resultCount++;
        }
    }
}

void MainWindow::on_searchButton_clicked()
{
    ui->searchButton->setDisabled(true);
    resultsPerPage = ui->resultsPerPageComboBox->currentText().toInt();
    int dbIndex = ui->translationComboBox->currentIndex();
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
            if (ui->wholeWordsCheckBox->isChecked()) {
                QRegExp regex = QRegExp("\\b" + enteredText + "\\b", sensitivity);
                searchWithRegex(regex);
                displayRegex = regex;
            } else {
                searchWithString(enteredText, sensitivity);
                displayRegex = QRegExp(enteredText, sensitivity);
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
                searchWithRegex(regexList, wordCount);
                displayRegex = QRegExp(displayRegexString, sensitivity);
            } else {
                QString displayRegexString;
                for (int i = 0; i < wordCount - 1; i++)
                    displayRegexString += searchWordList[i] + "|";
                displayRegexString += searchWordList[wordCount - 1];
                searchWithString(searchWordList, wordCount, sensitivity);
                displayRegex = QRegExp(displayRegexString, sensitivity);
            }
        } else if (ui->anyWordsRadioButton->isChecked()) {
            QString regexString;
            if (ui->wholeWordsCheckBox->isChecked()) {
                regexString += "\\b";
                for (int i = 0; i < wordCount - 1; i++)
                    regexString += searchWordList[i] + "\\b|\\b";
                regexString += searchWordList[wordCount - 1] + "\\b";
            } else {
                for (int i = 0; i < wordCount - 1; i++)
                    regexString += searchWordList[i] + "|";
                regexString += searchWordList[wordCount - 1];
            }
            QRegExp regex = QRegExp(regexString, sensitivity);
            searchWithRegex(regex);
            displayRegex = regex;
        }
    } else {
        if (ui->wholeWordsCheckBox->isChecked()) {
            QRegExp regex = QRegExp("\\b" + enteredText + "\\b", sensitivity);
            searchWithRegex(regex);
            displayRegex = regex;
        } else {
            searchWithString(enteredText, sensitivity);
            displayRegex = QRegExp(enteredText, sensitivity);
        }
    }
    timeElapsed = QString::number(watch.elapsed() / 1000.0) + " s";
    if (verseList.count() == 0) {
        ui->resultsTextBrowser->clear();
        statusLabel->setText(noMatches + timeElapsed);
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
    if (pageNumber == 1)
        ui->prevButton->setDisabled(true);
    if (endIndex < verseList.count())
        ui->nextButton->setEnabled(true);
    displayResults(startIndex, endIndex);
}

void MainWindow::on_nextButton_clicked()
{
    ui->prevButton->setEnabled(true);
    int startIndex = resultsPerPage * pageNumber;
    pageNumber++;
    int endIndex = resultsPerPage * pageNumber;
    if (endIndex >= verseList.count())
        ui->nextButton->setDisabled(true);
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
        StrongPopup strong(dbDct, argString, font);
        strong.exec();
    } else {
        int dbIndex = ui->translationComboBox->currentIndex();
        ui->tabWidget->setCurrentIndex(0);
        ui->translationTabWidget->setCurrentIndex(dbIndex);
        QStringList indices = argString.split(",");
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
}

void MainWindow::on_randomVerseButton_clicked()
{
    int dbIndex = ui->translationComboBox->currentIndex();
    int min = ui->searchFromComboBox->currentIndex() + 1;
    if (min < 40 && !std::get<2>(databases[dbIndex])) {
        QMessageBox::critical(this, "Error", "The current transation contains only the New Testament.");
        ui->divisionComboBox->setCurrentIndex(7);
        return;
    }
    int max = ui->searchToComboBox->currentIndex() + 2;
    uint seed = QTime::currentTime().msec();
    qsrand(seed);
    int bookNumber = qrand() % (max - min) + min;
    QSqlQuery query(std::get<0>(databases[dbIndex]));
    QString book = QString::number(bookNumber);
    QString verseText;
    while (verseText.isNull()) {
        QString queryString = "SELECT Min(Chapter), Max(Chapter) FROM Bible"
                              " WHERE Book = " + book;
        query.exec(queryString);
        while (query.next()) {
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
        while (query.next()) {
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
        while (query.next()) {
            randomResult = query.record().value(0).toString();
            ui->randomVerseTextBrowser->clear();
            verseText = formatResult(randomResult, QRegExp("<RF>.*<Rf>"),
                                   std::get<3>(databases[dbIndex]));
            verseText += "<b><a href = \"" + book + "," + chapter + "," + verse +
                         "\" style='text-decoration: none'>—" +
                         ui->bookListWidget->item(bookNumber - 1)->text() + " " +
                         chapter + ":" + verse + "</a></b>";
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

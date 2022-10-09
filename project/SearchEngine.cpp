#include "SearchEngine.h"
#include "Formatting.h"

bool SearchEngine::containsAllWords(const QString &text, const QList<QRegularExpression> &words)
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

bool SearchEngine::containsAnyWord(const QString &text, const QList<QRegularExpression> &words)
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

QRegularExpression SearchEngine::iterateRecords(QSqlQuery &query, const QStringList &words,
                                     QRegularExpression::PatternOption sensitivity, bool wholeWords, bool containsAll,
                                     QStringList &results, QStringList &refs, const QStringList &bookNames)
{
    qDebug() << "iterateRecords()";
    AllAny containsFunc = containsAll ? containsAllWords : containsAnyWord;
    QString boundary = wholeWords ? "\\b" : "";
    QList<QRegularExpression> wordsRgx;
    for (int i = 0; i < words.count(); ++i) {
        wordsRgx << QRegularExpression(boundary % words[i] % boundary, sensitivity
                                       | QRegularExpression::UseUnicodePropertiesOption);
    }
    QRegularExpression patterns("{H}.*{h}");
    while (query.next()) {
        QSqlRecord record = query.record();
        QString book = record.value(1).toString();
        QString chapter = record.value(2).toString();
        QString verse = record.value(3).toString();
        QString scripture = record.value(4).toString();
        scripture.remove(patterns);
        //qDebug() << scripture;
        if (containsFunc(scripture, wordsRgx)) {
            results << scripture;
            refs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                           .arg(book, chapter, verse, bookNames[book.toInt() - 1], chapter, verse);
        }
    }
    QStringList dispWords;
    for (QString word : words) {
        dispWords << boundary % word % boundary;
    }
    return QRegularExpression(dispWords.join('|'), sensitivity);
}

QRegularExpression SearchEngine::iterateRecords(QSqlQuery &query, const QString &text,
                                     QRegularExpression::PatternOption sensitivity, bool wholeWords, bool hasStrong,
                                     QStringList &results, QStringList &refs, const QStringList &bookNames)
{
    qDebug() << "iterateRecords2()";
    QString boundary = wholeWords ? "\\b" : "";
    QRegularExpression textRgx(boundary % text % boundary, sensitivity | QRegularExpression::UseUnicodePropertiesOption);
    QString strong = hasStrong ? "|<W[HG][0-9]{1,4}>" : "";
    QRegularExpression patterns("{TN}.*{tn}|{H}.*{h}|{[A-Za-z]{2}}|<.{1,2}>" + strong);
    while (query.next()) {
        QSqlRecord record = query.record();
        QString inputText = record.value(4).toString();
        QString outputText(inputText);
        inputText.remove(patterns);
        if (inputText.contains(textRgx)) {
            QString book = record.value(1).toString();
            QString chapter = record.value(2).toString();
            QString verse = record.value(3).toString();
            results << outputText;
            refs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
                            .arg(book, chapter, verse, bookNames[book.toInt() - 1], chapter, verse);
        }
    }
    return QRegularExpression(textRgx.pattern(), sensitivity);
}

void SearchEngine::escape(QStringList &words)
{
    for (QString &word : words) {
        word.replace("'", "''");
    }
}

void SearchEngine::multipleWordQuery(QSqlQuery &query, const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj, int book1, int book2)
{
    QString command = "SELECT * FROM Bible";
    QString whereOr = " WHERE (LOWER(Scripture) LIKE '%?%'"
                      " OR UPPER(Scripture) LIKE '%?%')";
    command += whereOr;
    for (int i = 1; i < wordsLow.count(); ++i) {
        command += " " % conj % whereOr;
    }
    command += " AND Book >= ? AND Book <= ?";
    query.prepare(command);
    for (int i = 0; i < wordsLow.count(); ++i) {
        query.addBindValue(wordsLow[i]);
        query.addBindValue(wordsUpp[i]);
    }
    query.addBindValue(book1);
    query.addBindValue(book2);

    qDebug() << "ERR:" << query.lastError();
}




QString SearchEngine::multipleWordQueryString(const QStringList &wordsLow, const QStringList &wordsUpp, const QString &conj)
{
    QString queryString = "SELECT * FROM Bible"
                          " WHERE (LOWER(Scripture) LIKE '%" % wordsLow[0] % "%'"
                          " OR UPPER(Scripture) LIKE '%" % wordsUpp[0] % "%')";
    for (int i = 1; i < wordsLow.count(); ++i) {
        queryString += " " % conj % " (LOWER(Scripture) LIKE '%" % wordsLow[i] % "%'"
                                    " OR UPPER(Scripture) LIKE '%" % wordsUpp[i] % "%')";
    }
    return queryString;
}

QRegularExpression SearchEngine::search(QString text, SearchOptions options, QSqlDatabase &module, bool hasStrong, QStringList &refs,
                                 const QStringList &bookNames, QStringList &results)
{
    results.clear();
    QString textLow = text.toLower();
    QString textUpp = text.toUpper();
    QStringList words = text.split(" ");
    QStringList wordsLow = textLow.split(" ");
    QStringList wordsUpp = textUpp.split(" ");
    escape(wordsLow);
    escape(wordsUpp);
    QString conj = options.searchMode == SearchMode::anyWords ? "OR" : "AND";
    QRegularExpression::PatternOption sensitivity = options.caseSensitive ?
                QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
    bool containsAll = options.searchMode == SearchMode::allWords ? true : false;
    QString command = multipleWordQueryString(wordsLow, wordsUpp, conj) %
                      " AND Book >= " % QString::number(options.bookFrom) %
                      " AND Book <= " % QString::number(options.bookTo);
    //qDebug() << "CMD:" << command;
    QSqlQuery query(module);
   // multipleWordQuery(query, wordsLow, wordsUpp, conj, options.bookFrom, options.bookTo);

    qDebug() << "Last Query:" << query.lastQuery();
    QRegularExpression regex;
    if (query.exec(command)) {
        qDebug() << "Bound Values:" << query.boundValues();
        qDebug() << "Size:" << query.at() + 1;
        if (options.searchMode == SearchMode::exactPhrase) {
            regex = iterateRecords(query, text, sensitivity, options.wholeWordsOnly, hasStrong, results, refs,
                                   bookNames);
        } else {
            regex = iterateRecords(query, words, sensitivity, options.wholeWordsOnly, containsAll, results,
                                   refs, bookNames);
        }
    }
    return regex;
}


int SearchEngine::ComputeLevenshteinDistance(const QString &source, const QString &target)
{
    if ((source == nullptr) || (target == nullptr)) return 0;
    if ((source.length() == 0) || (target.length() == 0)) return 0;
    if (source == target) return source.length();

    int sourceWordCount = source.length();
    int targetWordCount = target.length();

    // Step 1
    if (sourceWordCount == 0)
        return targetWordCount;

    if (targetWordCount == 0)
        return sourceWordCount;

    int numCols = targetWordCount + 1;
    int *distance = new int[(sourceWordCount + 1) * numCols];

    // Step 2
    for (int i = 0; i <= sourceWordCount; ++i)  {
        distance[i * numCols] = i;
    }
    for (int j = 0; j <= targetWordCount; ++j) {
        distance[j] = j;
    }

    for (int i = 1; i <= sourceWordCount; i++) {
        for (int j = 1; j <= targetWordCount; j++) {
            // Step 3
            int cost = (target[j - 1] == source[i - 1]) ? 0 : 1;

            // Step 4
            distance[i * numCols + j]
                    = qMin(qMin(distance[(i - 1) * numCols + j] + 1,
                           distance[i * numCols + j - 1] + 1), distance[(i - 1) * numCols + j - 1] + cost);
        }
    }
    int result = distance[sourceWordCount * numCols + targetWordCount];
    delete[] distance;
    return result;
}

double SearchEngine::CalculateSimilarity(const QString &source, const QString &target, bool caseSensitive)
{
    if ((source == nullptr) || (target == nullptr)) return 0.0;
    if ((source.length() == 0) || (target.length() == 0)) return 0.0;
    if (source == target) return 1.0;

    int stepsToSame = caseSensitive ?
        ComputeLevenshteinDistance(source, target) :
        ComputeLevenshteinDistance(source.toLower(), target.toLower());
    return 1.0 - (stepsToSame / (double)qMax(source.length(), target.length()));
}

int SearchEngine::FindBestMatchIndex(QString pattern, QStringList values)
{
    if (values.empty() || values.length() == 0) return -1;

    for (int i = 0; i < values.length(); i++) {
        if (pattern == values[i]) return i;
    }

    QString patternLower = pattern.toLower();
    for (int i = 0; i < values.length(); i++) {
        if (patternLower == values[i].toLower()) return i;
    }

    for (int i = 0; i < values.length(); i++) {
        if (values[i].startsWith(pattern)) return i;
    }

    for (int i = 0; i < values.length(); i++) {
        if (values[i].toLower().startsWith(patternLower)) return i;
    }

    double lastSimilarityCs = 0.0;
    int lastIndexCs = 0;

    for (int i = 0; i < values.length(); i++) {
        double similarity = CalculateSimilarity(pattern, values[i]);
        if (similarity > lastSimilarityCs) {
            lastIndexCs = i;
            lastSimilarityCs = similarity;
        }
    }

    double lastSimilarityCi = 0.0;
    int lastIndexCi = 0;
    for (int i = 0; i < values.length(); i++) {
        double similarity = CalculateSimilarity(pattern, values[i]);
        if (similarity > lastSimilarityCi) {
            lastIndexCi = i;
            lastSimilarityCi = similarity;
        }
    }
    return lastSimilarityCs > lastSimilarityCi ? lastIndexCs : lastIndexCi;
}


qbv::Location SearchEngine::parseLocationStr(QString raw,
                                             const QStringList& bookNames,
                                             const QStringList& shortBookNames)
{
    QString trimmed = raw.trimmed();
    int i = 0;
    bool isDigit = false;
    QList<int> indices;
    if (trimmed.length() > 0 && trimmed[0].isDigit()) {
        while (i < trimmed.length() && trimmed[i].isDigit()) i++;
    }
    while (i < trimmed.length() && indices.count() < 6) {
        if ((trimmed[i].isDigit() && !isDigit) || (!trimmed[i].isDigit() && isDigit)) {
            isDigit = !isDigit;
            indices.append(i);
        }
        ++i;
    }
    QString bookStr = trimmed.mid(0, indices[0]);
    int chapter = -1;
    int verse1 = -1;
    int verse2 = -1;
    if (indices.count() == 1) {
        chapter = trimmed.mid(indices[0]).toInt();
    } else if (indices.count() == 2) {
        chapter = trimmed.mid(indices[0], indices[1] - indices[0]).toInt();
    } else if (indices.count() == 3) {
        chapter = trimmed.mid(indices[0], indices[1] - indices[0]).toInt();
        verse1 = trimmed.mid(indices[2]).toInt();
    } else if (indices.count() == 4) {
        chapter = trimmed.mid(indices[0], indices[1] - indices[0]).toInt();
        verse1 = trimmed.mid(indices[2], indices[3] - indices[2]).toInt();
    } else if (indices.count() == 5) {
        chapter = trimmed.mid(indices[0], indices[1] - indices[0]).toInt();
        verse1 = trimmed.mid(indices[2], indices[3] - indices[2]).toInt();
        verse2 = trimmed.mid(indices[4]).toInt();
    } else if (indices.count() > 5) {
        chapter = trimmed.mid(indices[0], indices[1] - indices[0]).toInt();
        verse1 = trimmed.mid(indices[2], indices[3] - indices[2]).toInt();
        verse2 = trimmed.mid(indices[4], indices[5] - indices[4]).toInt();
    }

    int bookIdx = -1;
    QString bookStrLower = bookStr.toLower();
    for (int i = 0; i < shortBookNames.length(); ++i) {
        if (bookStrLower == shortBookNames[i].toLower()) {
            bookIdx = i;
            return qbv::Location(bookIdx + 1, chapter, verse1, verse2);
        }
    }

    bookIdx = FindBestMatchIndex(bookStr, bookNames);
    return qbv::Location(bookIdx + 1, chapter, verse1, verse2);
}


//void MainWindow::performSearch()
//{
//    QTime watch;
//    watch.start();
//    QString text = ui_Sea_LineEdit_Search->text();
//    QString textLow = text.toLower();
//    QString textUpp = text.toUpper();
//    QStringList words = text.split(" ");
//    QStringList wordsLow = textLow.split(" ");
//    QStringList wordsUpp = textUpp.split(" ");
//    int bookFirst = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
//    int bookLast = ui_Sea_ComboBox_SearchTo->currentIndex() + 1;
//    QString conj = ui_Sea_RadioButton_Any->isChecked() ? "OR" : "AND";
//    QRegularExpression::PatternOption sensitivity = ui_Sea_CheckBox_Case->isChecked() ?
//                                      QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
//    bool wholeWords = ui_Sea_CheckBox_WholeWords->isChecked() ? true : false;
//    bool containsAll = ui_Sea_RadioButton_All->isChecked() ? true : false;
//    QString queryString = multipleWordQueryString(wordsLow, wordsUpp, conj) %
//                          " AND Book >= " % QString::number(bookFirst) %
//                          " AND Book <= " % QString::number(bookLast);
//    int idx = ui_Sea_ComboBox_Translation->currentIndex();
//    QSqlQuery query(m_modules[idx].database);
//    if (query.exec(queryString)) {
//        m_resVerses.clear();
//        m_resRefs.clear();
//        if (ui_Sea_RadioButton_Exact->isChecked()) {
//            iterateRecords(query, text, sensitivity, wholeWords, m_modules[idx].hasStrong);
//        } else {
//            iterateRecords(query, words, sensitivity, wholeWords, containsAll);
//        }
//        m_elapsedTime = QString::number(watch.elapsed() / 1000.) % " s";
//        enableDisableSearchNavigationButtons();
//    }
//}

//void MainWindow::performSearchByStrong()
//{
//    QTime watch;
//    watch.start();
//    QString text = ui_Sea_LineEdit_Search->text().toUpper();
//    m_dispRgx = QRegExp("\\b" % text % "\\b", Qt::CaseSensitive);
//    text = "<W" % text % ">";
//    int bookFirst = ui_Sea_ComboBox_SearchFrom->currentIndex() + 1;
//    int bookLast = ui_Sea_ComboBox_SearchTo->currentIndex() + 1;
//    QString queryString = "SELECT * FROM Bible WHERE Scripture LIKE '%" % text % "%'"
//                          " AND Book >= " % QString::number(bookFirst) %
//                          " AND Book <= " % QString::number(bookLast);
//    int idx = ui_Sea_ComboBox_Translation->currentIndex();
//    QSqlQuery query(m_modules[idx].database);
//    if (query.exec(queryString)) {
//        m_resVerses.clear();
//        m_resRefs.clear();
//        while (query.next()) {
//            QSqlRecord record = query.record();
//            QString book = record.value(0).toString();
//            QString chapter = record.value(1).toString();
//            QString verse = record.value(2).toString();
//            QString scripture = record.value(3).toString();
//            m_resVerses << scripture;
//            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
//                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
//        }
//        m_elapsedTime = QString::number(watch.elapsed() / 1000.) % " s";
//        enableDisableSearchNavigationButtons();
//    }
//}

//typedef bool (&AllAny)(const QString &text, const QList<QRegularExpression> &words);

//void MainWindow::iterateRecords(QSqlQuery &query, const QStringList &words,
//                                QRegularExpression::PatternOption sensitivity, bool wholeWords, bool containsAll)
//{
//    AllAny containsFunc = containsAll ? containsAllWords : containsAnyWord;
//    QString boundary = wholeWords ? "\\b" : "";
//    QList<QRegularExpression> wordsRgx;
//    for (int i = 0; i < words.count(); ++i) {
//        wordsRgx << QRegularExpression(boundary % words[i] % boundary, sensitivity | QRegularExpression::UseUnicodePropertiesOption);
//    }
//    while (query.next()) {
//        QSqlRecord record = query.record();
//        QString book = record.value(0).toString();
//        QString chapter = record.value(1).toString();
//        QString verse = record.value(2).toString();
//        QString scripture = record.value(3).toString();
//        if (containsFunc(scripture, wordsRgx)) {
//            m_resVerses << scripture;
//            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
//                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
//        }
//    }
//    QStringList dispWords;
//    for (QString word : words) {
//        dispWords << boundary % word % boundary;
//    }
//    Qt::CaseSensitivity cs = sensitivity == QRegularExpression::NoPatternOption ?
//                Qt::CaseSensitive : Qt::CaseInsensitive;
//    m_dispRgx = QRegExp(dispWords.join("|"), cs);
//}

//void MainWindow::iterateRecords(QSqlQuery &query, const QString &text,
//                                QRegularExpression::PatternOption sensitivity, bool wholeWords, bool hasStrong)
//{
//    QString boundary = wholeWords ? "\\b" : "";
//    QRegularExpression textRgx(boundary % text % boundary, sensitivity | QRegularExpression::UseUnicodePropertiesOption);
//    QString strong = hasStrong ? "|<W[HG][0-9]{1,4}>" : "";
//    QRegularExpression patterns("<.{2,3}>|<RF>.*<Rf>" + strong);
//    while (query.next()) {
//        QSqlRecord record = query.record();
//        QString rawText = record.value(3).toString();
//        QString strippedText = rawText.remove(patterns);
//        if (strippedText.contains(textRgx)) {
//            QString book = record.value(0).toString();
//            QString chapter = record.value(1).toString();
//            QString verse = record.value(2).toString();
//            m_resVerses << record.value(3).toString();
//            m_resRefs << QString("<b><a href='%1,%2,%3' style='text-decoration:none'>—%4 %5:%6</a></b>")
//                         .arg(book, chapter, verse, m_bookNames[book.toInt() - 1], chapter, verse);
//        }
//    }
//    Qt::CaseSensitivity cs = sensitivity == QRegularExpression::NoPatternOption ?
//                Qt::CaseSensitive : Qt::CaseInsensitive;
//    m_dispRgx = QRegExp(textRgx.pattern(), cs);
//}

//void MainWindow::enableDisableSearchNavigationButtons()
//{
//    m_numResPerPage = ui_Sea_ComboBox_ResPerPage->currentText().toInt();
//    displaySearchResults(0, m_numResPerPage);
//    m_crntStartRes = m_numResPerPage;

//    ui_Sea_Button_First->setDisabled(true);
//    ui_Sea_Button_Prev->setDisabled(true);
//    ui_Sea_Button_Next->setEnabled(m_resVerses.count() > m_numResPerPage);
//    ui_Sea_Button_Last->setEnabled(m_resVerses.count() > m_numResPerPage);

//    int numPages = m_resVerses.count() % m_numResPerPage > 0 ?
//                m_resVerses.count() / m_numResPerPage + 1 : m_resVerses.count() / m_numResPerPage;
//    if (m_resVerses.isEmpty()) {
//        m_labels[SEA_OF]->setText(tr("of —"));
//        ui_Sea_SpinBox_PageNum->setRange(0, 0);
//    } else {
//        m_labels[SEA_OF]->setText(tr("of ") + QString::number(numPages));
//        ui_Sea_SpinBox_PageNum->setRange(1, numPages);
//    }

//    ui_Sea_SpinBox_PageNum->setDisabled(m_resVerses.isEmpty());
//    ui_Sea_Button_GoTo->setDisabled(m_resVerses.isEmpty());
//}

//float avgDistance(const QVector<int> indices)
//{
//    float sum = 0.f;
//    for (int i = 0; i < indices.count() - 1; ++i) {
//        sum += indices[i + 1] - indices[i];
//    }
//    return sum / (indices.count() - 1);
//}

//int MainWindow::findPassageMatch(const QString &word, int len, bool caseSensitive)
//{
//    int idx = -1;
//    QMap<int, QMap<int, QChar>> matchMap;
//    for (int i = 0; i < m_bookNames.count(); ++i) {
//        QMap<int, QChar> idxMap;
//        QString wordb = caseSensitive ? QString(word) : QString(word.toLower());
//        QString wordsib = caseSensitive ? QString(m_bookNames[i]) : QString(m_bookNames[i].toLower());
//        int lenb = len;
//        for (int iWords = 0; iWords < m_bookNames[i].length(); ++iWords) {
//            for (int iWord = 0; iWord < lenb; ++iWord) {
//                if (wordb[iWord] == wordsib[iWords]) {
//                    idxMap.insert(iWords, wordb[iWord]);
//                    wordb.remove(iWord, 1);
//                    --iWord;
//                    --lenb;
//                    break;
//                }
//            }
//        }
//        if (idxMap.count() == len) {
//            matchMap.insert(i, idxMap);
//        }
//    }
//    if (!matchMap.isEmpty()) {
//        QMap<int, float> avgMap;
//        for (QMap<int, QMap<int, QChar>>::iterator i = matchMap.begin(); i != matchMap.end(); ++i) {
//            avgMap.insert(i.key(), avgDistance(i.value().keys().toVector()));
//        }
//        QList<QPair<float, int>> avgList;
//        for(QMap<int, float>::iterator i = avgMap.begin(); i != avgMap.end(); ++i) {
//            avgList.append(qMakePair(i.value(), i.key()));
//        }
//        std::sort(avgList.begin(), avgList.end());
//        idx = avgList[0].second;
//    } else if (len > 2) {
//        idx = findPassageMatch(word, len - 1, caseSensitive);
//    }
//    return idx;
//}

//int MainWindow::findPassageMatch(const QString &word)
//{
//    QVector<QString> simpleNames(m_bookNames.count());
//    for (int i = 0; i < simpleNames.count(); ++i) {
//        simpleNames[i] = QString(m_bookNames[i]).replace(" ", "");
//    }
//    int idx = -1;
//    QString wordt = word.simplified().replace(" ", "");
//    for (int i = 0; i < simpleNames.count(); ++i) {
//        if (simpleNames[i].startsWith(wordt, Qt::CaseSensitive)) {
//            idx = i;
//            break;
//        }
//    }
//    for (int i = 0; i < simpleNames.count(); ++i) {
//        if (simpleNames[i].startsWith(wordt, Qt::CaseInsensitive)) {
//            idx = i;
//            break;
//        }
//    }
//    if (idx == -1) {
//        for (int i = 0; i < simpleNames.count(); ++i) {
//            if (simpleNames[i].contains(wordt, Qt::CaseSensitive)) {
//                idx = i;
//                break;
//            }
//        }
//    }
//    if (idx == -1) {
//        for (int i = 0; i < simpleNames.count(); ++i) {
//            if (simpleNames[i].contains(wordt, Qt::CaseInsensitive)) {
//                idx = i;
//                break;
//            }
//        }
//    }
//    if (idx == -1) {
//        idx = findPassageMatch(word, word.length(), true);
//    }
//    if (idx == -1) {
//        idx = findPassageMatch(word, word.length(), false);
//    }
//    if (idx == -1) {
//        idx = findPassageRegex(wordt, simpleNames);
//    }
//    return idx;
//}

//int MainWindow::findPassageRegex(const QString &wordt, const QVector<QString> &simpleNames)
//{
//    int idx = -1;
//    QString rgxStr = "^.*";
//    for (int i = 0; i < wordt.length(); i++) {
//        rgxStr += "(?=.*" % wordt[i] % ")";
//    }
//    rgxStr += ".*$";
//    QRegularExpression regex(rgxStr);
//    if (idx == -1) {
//        for (int i = 0; i < simpleNames.count(); ++i) {
//            if (simpleNames[i].contains(regex)) {
//                idx = i;
//                break;
//            }
//        }
//    }
//    if (idx == -1) {
//        regex = QRegularExpression(rgxStr, QRegularExpression::CaseInsensitiveOption);
//        for (int i = 0; i < simpleNames.count(); ++i) {
//            if (simpleNames[i].contains(regex)) {
//                idx = i;
//                break;
//            }
//        }
//    }
//    return idx;
//}

//QVector<QString> MainWindow::getSimpleNames()
//{
//    QVector<QString> simpleNames(m_bookNames.count());
//    for (int i = 0; i < simpleNames.count(); ++i) {
//        simpleNames[i] = QString(m_bookNames[i]).replace(" ", "");
//    }
//    return simpleNames;
//}

//int MainWindow::matchPassage(const QString &bookStr)
//{
//    QVector<QString> simpleNames = getSimpleNames();
//    QList<int> subResults;
//    subResults << matchPassageStartsWith(bookStr, simpleNames);
//    subResults << matchPassageContains(bookStr, simpleNames, Qt::CaseSensitive);
//    subResults << matchPassageContains(bookStr, simpleNames, Qt::CaseInsensitive);
//    subResults << matchPassageCharDistance(bookStr, bookStr.length(), true);
//    subResults << matchPassageCharDistance(bookStr, bookStr.length(), false);
//    subResults << matchPassageRegex(bookStr, simpleNames, QRegularExpression::NoPatternOption);
//    subResults << matchPassageRegex(bookStr, simpleNames, QRegularExpression::CaseInsensitiveOption);

//    QHash<int, int> resCount;
//    for (int i = 0; i < subResults.count(); ++i) {
//        if (subResults[i] > -1) {
//            if (!resCount.contains(subResults[i])) {
//                resCount.insert(subResults[i], 1);
//            } else {
//                ++resCount[subResults[i]];
//            }
//        }
//    }
//    QList<QPair<float, int>> resList;
//    for (QHash<int, int>::iterator i = resCount.begin(); i != resCount.end(); ++i) {
//        resList.append(qMakePair(i.value(), i.key()));
//    }
//    std::sort(resList.rbegin(), resList.rend());
//    return resList[0].second;
//}

//int MainWindow::matchPassageV2(const QString &bookStr)
//{
//    QVector<QString> simpleNames = getSimpleNames();
//    int idx = -1;
//    idx = matchPassageStartsWith(bookStr, simpleNames);
//    if (idx > -1) {
//        return idx;
//    }r
//    idx = matchPassageContains(bookStr, simpleNames, Qt::CaseSensitive);
//    if (idx > -1) {
//        return idx;
//    }
//    idx = matchPassageContains(bookStr, simpleNames, Qt::CaseInsensitive);
//    if (idx > -1) {
//        return idx;
//    }
//    idx = matchPassageCharDistance(bookStr, bookStr.length(), true);
//    if (idx > -1) {
//        return idx;
//    }
//    idx = matchPassageCharDistance(bookStr, bookStr.length(), false);
//    if (idx > -1) {
//        return idx;
//    }
//    idx = matchPassageRegex(bookStr, simpleNames, QRegularExpression::NoPatternOption);
//    if (idx > -1) {
//        return idx;
//    }
//    return matchPassageRegex(bookStr, simpleNames, QRegularExpression::CaseInsensitiveOption);
//}

//int MainWindow::matchPassageStartsWith(const QString &word, const QVector<QString> &names)
//{
//    int idx = -1;
//    QString wordt = word.simplified().replace(" ", "");
//    for (int i = 0; i < names.count(); ++i) {
//        if (names[i].startsWith(wordt, Qt::CaseSensitive)) {
//            idx = i;
//            break;
//        }
//    }
//    if (idx == -1) {
//        for (int i = 0; i < names.count(); ++i) {
//            if (names[i].startsWith(wordt, Qt::CaseInsensitive)) {
//                idx = i;
//                break;
//            }
//        }
//    }
//    return idx;
//}

//int MainWindow::matchPassageContains(const QString &word, const QVector<QString> &names,
//                                     Qt::CaseSensitivity sensitivity)
//{
//    int idx = -1;
//    QString wordt = word.simplified().replace(" ", "");
//    for (int i = 0; i < names.count(); ++i) {
//        if (names[i].contains(wordt, sensitivity)) {
//            idx = i;
//            break;
//        }
//    }
//    return idx;
//}

//int MainWindow::matchPassageCharDistance(const QString &word, int len, bool caseSensitive)
//{
//    int idx = -1;
//    QMap<int, QMap<int, QChar>> matchMap;
//    for (int i = 0; i < m_bookNames.count(); ++i) {
//        QMap<int, QChar> idxMap;
//        QString wordb = caseSensitive ? QString(word) : QString(word.toLower());
//        QString wordsib = caseSensitive ? QString(m_bookNames[i]) : QString(m_bookNames[i].toLower());
//        int lenb = len;
//        for (int iWords = 0; iWords < m_bookNames[i].length(); ++iWords) {
//            for (int iWord = 0; iWord < lenb; ++iWord) {
//                if (wordb[iWord] == wordsib[iWords]) {
//                    idxMap.insert(iWords, wordb[iWord]);
//                    wordb.remove(iWord, 1);
//                    --iWord;
//                    --lenb;
//                    break;
//                }
//            }
//        }
//        if (idxMap.count() == len) {
//            matchMap.insert(i, idxMap);
//        }
//    }
//    if (!matchMap.isEmpty()) {
//        QMap<int, float> avgMap;
//        for (QMap<int, QMap<int, QChar>>::iterator i = matchMap.begin(); i != matchMap.end(); ++i) {
//            avgMap.insert(i.key(), avgDistance(i.value().keys().toVector()));
//        }
//        QList<QPair<float, int>> avgList;
//        for(QMap<int, float>::iterator i = avgMap.begin(); i != avgMap.end(); ++i) {
//            avgList.append(qMakePair(i.value(), i.key()));
//        }
//        std::sort(avgList.begin(), avgList.end());
//        idx = avgList[0].second;
//    } else if (len > 2) {
//        idx = findPassageMatch(word, len - 1, caseSensitive);
//    }
//    return idx;
//}

//int MainWindow::matchPassageRegex(const QString &word, const QVector<QString> &simpleNames,
//                                  QRegularExpression::PatternOption sensivity)
//{
//    int idx = -1;
//    QString rgxStr = "^.*";
//    QString wordt = word.simplified().replace(" ", "");
//    for (int i = 0; i < wordt.length(); i++) {
//        rgxStr += "(?=.*" % wordt[i] % ")";
//    }
//    rgxStr += ".*$";
//    QRegularExpression regex(rgxStr, sensivity);
//    for (int i = 0; i < simpleNames.count(); ++i) {
//        if (simpleNames[i].contains(regex)) {
//            idx = i;
//            break;
//        }
//    }
//    return idx;
//}


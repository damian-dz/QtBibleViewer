#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

#include <QClipboard>

void MainWindow::actionCopy()
{
    if (textBrowser)
        textBrowser->copy();
    else if (textEdit)
        textEdit->copy();
    else if (lineEdit)
        lineEdit->copy();
}

void MainWindow::actionCut()
{
    if (textEdit)
        textEdit->cut();
    else if (lineEdit)
        lineEdit->cut();
}

void MainWindow::actionClear()
{
    if (textEdit)
        textEdit->clear();
    else if (lineEdit)
        lineEdit->clear();
}

void MainWindow::actionPaste()
{
    if (textEdit)
        textEdit->paste();
    else if (lineEdit)
        lineEdit->paste();
}

void MainWindow::actionSelectAll()
{
    if (textBrowser)
        textBrowser->selectAll();
    else if (textEdit)
        textEdit->selectAll();
    else if (lineEdit)
        lineEdit->selectAll();
}

void MainWindow::showBasicContextMenu(const QPoint &pos)
{
    textBrowser = qobject_cast<QTextBrowser *>(QObject::sender());
    textEdit = nullptr;
    lineEdit = nullptr;
    QPoint globalPos = textBrowser->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = textBrowser->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[2]->setDisabled(textBrowser->toPlainText().isEmpty());
    contextMenu.exec(globalPos);
}

void MainWindow::showEditContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Cut"),
                          this,
                          SLOT(actionCut()),
                          QKeySequence("Ctrl+X"));
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addAction(tr("Paste"),
                          this,
                          SLOT(actionPaste()),
                          QKeySequence("Ctrl+V"));
    contextMenu.addAction(tr("Clear"),
                          this,
                          SLOT(actionClear()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    QList<QAction *> contextActions = contextMenu.actions();
    QString senderName = QObject::sender()->metaObject()->className();
    QPoint globalPos;
    if (senderName == "QLineEdit") {
        textEdit = nullptr;
        textBrowser = nullptr;
        lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
        globalPos = lineEdit->mapToGlobal(pos);
        bool isSelected = (lineEdit->selectedText().length() > 0);
        contextActions[0]->setEnabled(isSelected);
        contextActions[1]->setEnabled(isSelected);
        bool isEmpty = lineEdit->text().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    } else if (senderName == "QTextEdit") {
        lineEdit = nullptr;
        textBrowser = nullptr;
        textEdit = qobject_cast<QTextEdit *>(QObject::sender());
        globalPos = textEdit->mapToGlobal(pos);
        QTextCursor cursor = textEdit->textCursor();
        contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        contextActions[1]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
        bool isEmpty = textEdit->toPlainText().isEmpty();
        contextActions[3]->setDisabled(isEmpty);
        contextActions[5]->setDisabled(isEmpty);
    }
    contextActions[2]->setDisabled(qApp->clipboard()->text().isEmpty());
    contextMenu.exec(globalPos);
}

void MainWindow::getVerseRange()
{
    QString plainText = chapterBrowsers[currentTranslationTab]->toPlainText();
    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    int prev = 0;
    int crnt = 0;
    QRegularExpression regex("\n[0-9]{1,3}\\b");
    QRegularExpressionMatchIterator iter = regex.globalMatch(plainText);
    bool found = false;
    int firstVerse = ui->verseFirstComboBox->currentIndex() + 1;
    int lastVerse = ui->verseFirstComboBox->currentIndex() + 1;
    while (iter.hasNext() && !found) {
        QRegularExpressionMatch match = iter.next();
        if (match.hasMatch()) {
            QString crntMatch = match.captured();
            crnt = plainText.indexOf(crntMatch);
            if (start > prev && start <= crnt)
                firstVerse = crntMatch.right(crntMatch.size() - 1).toInt() - 1;
            if (end > prev && end <= crnt) {
                lastVerse = crntMatch.right(crntMatch.size() - 1).toInt() - 1;
                found = true;
            }
            prev = crnt;
        }
    }
    crnt = plainText.lastIndexOf("\n");
    if (start > prev && start <= crnt)
        firstVerse = ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt();
    if (end > prev && end <= crnt + 2)
        lastVerse = ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt();
    range = qMakePair(firstVerse, lastVerse);
}


void MainWindow::chapterBrowser_actionCopy()
{
    chapterBrowsers[currentTranslationTab]->copy();
}

void MainWindow::chapterBrowser_actionCopyFormatted()
{
    QString plainText = chapterBrowsers[currentTranslationTab]->toPlainText();
    QString start = QString::number(range.first) + "\\b";
    range.first > ui->verseFirstComboBox->currentText().toInt() ?
                start.prepend("(\n") : start.prepend("(");
    QString end = range.second == ui->verseLastComboBox->itemText(ui->verseLastComboBox->currentIndex()).toInt() ?
                ")\n" : ")\n" + QString::number(range.second + 1) + "\\b";
    QRegExp regex(start + ".*" + end);
    int pos = regex.indexIn(plainText);
    QString textToCopy;
    if (pos > -1)
        textToCopy = regex.cap(1);
    textToCopy.replace(QRegExp("\n"), " ");
    textToCopy.remove(QRegExp("\\*| \\*| \\* |\\* "));
    if (modules[currentTranslationTab].hasStrong) {
        textToCopy.remove(QRegExp(" [HG][0-9]{1,4}"));
        textToCopy.replace("  ", " ");
    }
    textToCopy = textToCopy.trimmed();
    textToCopy += "—" + bookNames[ui->bookListWidget->currentRow()] +
            " " + ui->chapterListWidget->currentItem()->text() +
            ":" + QString::number(range.first);
    if (range.first != range.second)
        textToCopy += "-" + QString::number(range.second);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(textToCopy);
    ui->statusBar->showMessage(tr("Text copied to clipboard."), 2500);
}

void MainWindow::chapterBrowser_actionSelectAll()
{
    chapterBrowsers[currentTranslationTab]->selectAll();
}

void MainWindow::chapterBrowser_actionAddToFavorites()
{
    ui->tabWidget->setCurrentIndex(4);
    insertIntoFavorites();
}

void MainWindow::showBibleContextMenu(const QPoint &pos)
{
    QPoint globalPos = chapterBrowsers[currentTranslationTab]->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Copy"),
                          this,
                          SLOT(chapterBrowser_actionCopy()),
                          QKeySequence("Ctrl+C"));
    contextMenu.addAction(tr("Copy with Reference"),
                          this,
                          SLOT(chapterBrowser_actionCopyFormatted()));
    contextMenu.addAction(tr("Select All"),
                          this,
                          SLOT(chapterBrowser_actionSelectAll()),
                          QKeySequence("Ctrl+A"));
    contextMenu.addSeparator();
    getVerseRange();
    QString addVerseMsg = range.first == range.second ?
                tr("Add Verse ") + QString::number(range.first) + tr(" to Favorites") :
                tr("Add Verses ") + QString::number(range.first) +
                "-" + QString::number(range.second) + tr(" to Favorites");
    contextMenu.addAction(addVerseMsg,
                          this,
                          SLOT(chapterBrowser_actionAddToFavorites()));
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Back"),
                          this,
                          SLOT(on_actionBack_triggered()),
                          QKeySequence(tr("Ctrl+Left")));
    contextMenu.addAction(tr("Forward"),
                          this,
                          SLOT(on_actionForward_triggered()),
                          QKeySequence(tr("Ctrl+Right")));
    QList<QAction *> contextActions = contextMenu.actions();
    QTextCursor cursor = chapterBrowsers[currentTranslationTab]->textCursor();
    contextActions[0]->setDisabled(cursor.selectionStart() == cursor.selectionEnd());
    contextActions[1]->setEnabled(contextActions[0]->isEnabled());
    contextActions[4]->setDisabled(range.first == 0 && range.second == 0);
    contextActions[6]->setEnabled(ui->actionBack->isEnabled());
    contextActions[7]->setEnabled(ui->actionForward->isEnabled());
    contextMenu.exec(globalPos);
}

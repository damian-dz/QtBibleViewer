#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::updateBooksAndDivisions()
{
    populateBookList();
    populateDivisionList();
    if (ui->bookListWidget->count() > 0) {
        ui->bookListWidget->blockSignals(true);
        int currentIndex = ui->bookListWidget->currentRow();
        ui->bookListWidget->clear();
        ui->bookListWidget->addItems(bookNames);
        ui->bookListWidget->setCurrentRow(currentIndex);
        ui->bookListWidget->blockSignals(false);
    }
    if (ui->searchFromComboBox->count() > 0 && ui->searchToComboBox->count() > 0) {
        ui->searchFromComboBox->blockSignals(true);
        ui->searchToComboBox->blockSignals(true);
        int currentIndex = ui->searchFromComboBox->currentIndex();
        ui->searchFromComboBox->clear();
        ui->searchFromComboBox->addItems(bookNames);
        ui->searchFromComboBox->setCurrentIndex(currentIndex);
        currentIndex = ui->searchToComboBox->currentIndex();
        ui->searchToComboBox->clear();
        ui->searchToComboBox->addItems(bookNames);
        ui->searchToComboBox->setCurrentIndex(currentIndex);
        ui->searchFromComboBox->blockSignals(false);
        ui->searchToComboBox->blockSignals(false);
    }
    if (ui->divisionComboBox->count() == 0) {
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(0);
    } else if (ui->divisionComboBox->itemText(0) != divisionNames[0]) {
        int currentIndex = ui->divisionComboBox->currentIndex();
        ui->divisionComboBox->clear();
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(currentIndex);
    }
    if (ui->compareBookListWidget->count() > 0) {
        ui->compareBookListWidget->blockSignals(true);
        int currentIndex = ui->compareBookListWidget->currentRow();
        ui->compareBookListWidget->clear();
        ui->compareBookListWidget->addItems(bookNames);
        ui->compareBookListWidget->setCurrentRow(currentIndex);
        ui->compareBookListWidget->blockSignals(false);
    }
}

void MainWindow::populateBookList()
{
    if (!bookNames.isEmpty())
        bookNames.clear();
    bookNames << tr("Genesis")
              << tr("Exodus")
              << tr("Leviticus")
              << tr("Numbers")
              << tr("Deuteronomy")
              << tr("Joshua")
              << tr("Judges")
              << tr("Ruth")
              << tr("1 Samuel")
              << tr("2 Samuel")
              << tr("1 Kings")
              << tr("2 Kings")
              << tr("1 Chronicles")
              << tr("2 Chronicles")
              << tr("Ezra")
              << tr("Nehemiah")
              << tr("Esther")
              << tr("Job")
              << tr("Psalms")
              << tr("Proverbs")
              << tr("Ecclesiastes")
              << tr("Song of Solomon")
              << tr("Isaiah")
              << tr("Jeremiah")
              << tr("Lamentations")
              << tr("Ezekiel")
              << tr("Daniel")
              << tr("Hosea")
              << tr("Joel")
              << tr("Amos")
              << tr("Obadiah")
              << tr("Jonah")
              << tr("Micah")
              << tr("Nahum")
              << tr("Habakkuk")
              << tr("Zephaniah")
              << tr("Haggai")
              << tr("Zechariah")
              << tr("Malachi")
              << tr("Matthew")
              << tr("Mark")
              << tr("Luke")
              << tr("John")
              << tr("Acts")
              << tr("Romans")
              << tr("1 Corinthians")
              << tr("2 Corinthians")
              << tr("Galatians")
              << tr("Ephesians")
              << tr("Philippians")
              << tr("Colossians")
              << tr("1 Thessalonians")
              << tr("2 Thessalonians")
              << tr("1 Timothy")
              << tr("2 Timothy")
              << tr("Titus")
              << tr("Philemon")
              << tr("Hebrews")
              << tr("James")
              << tr("1 Peter")
              << tr("2 Peter")
              << tr("1 John")
              << tr("2 John")
              << tr("3 John")
              << tr("Jude")
              << tr("Revelation");
}

void MainWindow::populateDivisionList()
{
    if (!divisionNames.isEmpty())
        divisionNames.clear();
    divisionNames << tr("Entire Bible")
                  << tr("Old Testament")
                  << tr("Pentateuch")
                  << tr("Historical Books")
                  << tr("Poetical Books")
                  << tr("Major Prophets")
                  << tr("Minor Prophets")
                  << tr("New Testament")
                  << tr("Gospels & Acts")
                  << tr("Pauline Epistles")
                  << tr("General Epistles & Revelation")
                  << tr("Custom Range");
}

void MainWindow::changeLanguageToEnglish()
{
    if (qApp->removeTranslator(translator))
        translatorInstalled = false;
    currentLanguage = "ENG";
    ui->actionEnglish->setChecked(true);
    ui->actionPolski->setChecked(false);
}

void MainWindow::changeLanguageToPolski()
{
    if (!translatorInstalled) {
        translator->load("pl", executionPath + "/translations");
        if (qApp->installTranslator(translator))
            currentLanguage = "PL";
        translatorInstalled = true;
    }
    ui->actionEnglish->setChecked(false);
    ui->actionPolski->setChecked(true);
}

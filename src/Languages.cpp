#include "hdr/MainWindow.h"
#include "ui_MainWindow.h"

void MainWindow::updateRangeComboBoxes()
{
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
}

void MainWindow::populateChapterMenuItemsEnglish()
{
    bookNames.clear();
    bookNames << "Genesis"
              << "Exodus"
              << "Leviticus"
              << "Numbers"
              << "Deuteronomy"
              << "Joshua"
              << "Judges"
              << "Ruth"
              << "1 Samuel"
              << "2 Samuel"
              << "1 Kings"
              << "2 Kings"
              << "1 Chronicles"
              << "2 Chronicles"
              << "Ezra"
              << "Nehemiah"
              << "Esther"
              << "Job"
              << "Psalms"
              << "Proverbs"
              << "Ecclesiastes"
              << "Song of Solomon"
              << "Isaiah"
              << "Jeremiah"
              << "Lamentations"
              << "Ezekiel"
              << "Daniel"
              << "Hosea"
              << "Joel"
              << "Amos"
              << "Obadiah"
              << "Jonah"
              << "Micah"
              << "Nahum"
              << "Habakkuk"
              << "Zephaniah"
              << "Haggai"
              << "Zechariah"
              << "Malachi"
              << "Matthew"
              << "Mark"
              << "Luke"
              << "John"
              << "Acts"
              << "Romans"
              << "1 Corinthians"
              << "2 Corinthians"
              << "Galatians"
              << "Ephesians"
              << "Philippians"
              << "Colossians"
              << "1 Thessalonians"
              << "2 Thessalonians"
              << "1 Timothy"
              << "2 Timothy"
              << "Titus"
              << "Philemon"
              << "Hebrews"
              << "James"
              << "1 Peter"
              << "2 Peter"
              << "1 John"
              << "2 John"
              << "3 John"
              << "Jude"
              << "Revelation";
    ui->bookListWidget->blockSignals(true);
    int currentIndex = ui->bookListWidget->currentRow();
    ui->bookListWidget->clear();
    ui->bookListWidget->addItems(bookNames);
    ui->bookListWidget->setCurrentRow(currentIndex);
    ui->bookListWidget->blockSignals(false);
    updateRangeComboBoxes();
}

void MainWindow::populateDivisionMenuItemsEnglish()
{
    divisionNames.clear();
    divisionNames << "Entire Bible"
              << "Old Testament"
              << "Pentateuch"
              << "Historical Books"
              << "Poetical Books"
              << "Major Prophets"
              << "Minor Prophets"
              << "New Testament"
              << "Gospels & Acts"
              << "Pauline Epistles"
              << "General Epistles & Revelation"
              << "Custom Range";
    if (ui->divisionComboBox->count() == 0) {
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(0);
    }
    else if (ui->divisionComboBox->itemText(0) != divisionNames[0]) {
        int currentIndex = ui->divisionComboBox->currentIndex();
        ui->divisionComboBox->clear();
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(currentIndex);
    }
}

void MainWindow::populateChapterMenuItemsPolski()
{
    bookNames.clear();
    bookNames << "Rodzaju"
              << "Wyjścia"
              << "Kapłańska"
              << "Liczb"
              << "Powtórzonego Prawa"
              << "Jozuego"
              << "Sędziów"
              << "Rut"
              << "1 Samuela"
              << "2 Samuela"
              << "1 Królów"
              << "2 Królów"
              << "1 Kronik"
              << "2 Kronik"
              << "Ezdrasza"
              << "Nehemiasza"
              << "Estery"
              << "Hioba"
              << "Psalmów"
              << "Przysłów"
              << "Kaznodziei"
              << "Pieśń nad Pieśniami"
              << "Izajasza"
              << "Jeremiasza"
              << "Lamentacji"
              << "Ezechiela"
              << "Daniela"
              << "Ozeasza"
              << "Joela"
              << "Amosa"
              << "Abdiasza"
              << "Jonasza"
              << "Micheasza"
              << "Nahuma"
              << "Habakuka"
              << "Sofoniasza"
              << "Aggeusza"
              << "Zachariasza"
              << "Malachiasza"
              << "Mateusza"
              << "Marka"
              << "Łukasza"
              << "Jana"
              << "Dzieje"
              << "Rzymian"
              << "1 Koryntian"
              << "2 Koryntian"
              << "Galatów"
              << "Efezjan"
              << "Filipian"
              << "Kolosan"
              << "1 Tesaloniczan"
              << "2 Tesaloniczan"
              << "1 Tymoteusza"
              << "2 Tymoteusza"
              << "Tytusa"
              << "Filemona"
              << "Hebrajczyków"
              << "Jakuba"
              << "1 Piotra"
              << "2 Piotra"
              << "1 Jana"
              << "2 Jana"
              << "3 Jana"
              << "Judy"
              << "Apokalipsa";
    ui->bookListWidget->blockSignals(true);
    int currentIndex = ui->bookListWidget->currentRow();
    ui->bookListWidget->clear();
    ui->bookListWidget->addItems(bookNames);
    ui->bookListWidget->setCurrentRow(currentIndex);
    ui->bookListWidget->blockSignals(false);
    updateRangeComboBoxes();
}

void MainWindow::populateDivisionMenuItemsPolski()
{
    divisionNames.clear();
    divisionNames << "Cała Biblia"
              << "Stary Testament"
              << "Pięcioksiąg"
              << "Księgi Historyczne"
              << "Księgi Poetyckie"
              << "Prorocy Więksi"
              << "Prorocy Mniejsi"
              << "Nowy Testament"
              << "Ewangelie i Dzieje"
              << "Listy Pawła"
              << "Listy Ogólne i Apokalipsa"
              << "Dowolny Zakres";
    if (ui->divisionComboBox->count() == 0) {
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(0);
    }
    else if (ui->divisionComboBox->itemText(0) != divisionNames[0]) {
        int currentIndex = ui->divisionComboBox->currentIndex();
        ui->divisionComboBox->clear();
        ui->divisionComboBox->addItems(divisionNames);
        ui->divisionComboBox->setCurrentIndex(currentIndex);
    }
}

void MainWindow::changeLanguageToEnglish()
{
    ui->actionPolski->setChecked(false);
    ui->actionEnglish->setChecked(true);
    ui->menuFile->setTitle("File");
    ui->actionOpen_Bible_Module->setText("Open Bible Module");
    ui->menuRecently_Opened->setTitle("Recently Opened");
    ui->actionExit->setText("Exit");
    ui->menuEdit->setTitle("Edit");
    ui->actionCopy->setText("Copy");
    ui->menuTools->setTitle("Tools");
    ui->actionSearch->setText("Search");
    ui->menuOptions->setTitle("Options");
    ui->actionPreferences->setText("Preferences");
    ui->menuLanguage->setTitle("Language");
    ui->menuHelp->setTitle("Help");
    ui->actionShow_Help->setText("Show Help");
    ui->actionAbout->setText("About");
    ui->tabWidget->setTabText(0, "Bible");
    ui->bookLabel->setText("Book:");
    ui->chapterLabel->setText("Chapter:");
    ui->verseLabel->setText("Verses:");
    ui->prevChapterButton->setToolTip("Previous chapter");
    ui->nextChapterButton->setToolTip("Next chapter");
    ui->copyButton->setText("Copy");
    ui->copyButton->setToolTip("Copies the selected range of verses to the clipboard.");
    ui->tabWidget->setTabText(1, "Details");
    ui->descriptionLabel->setText("Description:");
    ui->abbreviationLabel->setText("Abbreviation:");
    ui->commentsLabel->setText("Comments:");
    ui->versionLabel->setText("Version:");
    ui->versionDateLabel->setText("Version Date:");
    ui->publishDateLabel->setText("Publish Date:");
    ui->rightToLeftCheckBox->setText("Right to Left");
    ui->oldTestamentCheckBox->setText("Old Testament");
    ui->newTestamentCheckBox->setText("New Testament");
    ui->strongsNumbersCheckBox->setText("Strong's Numbers");
    ui->tabWidget->setTabText(2, "Search");
    ui->enterLabel->setText("Enter a Word or Phrase:");
    ui->searchButton->setText("Search");
    ui->divisionLabel->setText("Bible Section:");
    ui->searchOptions->setText("Search Options:");
    ui->translationLabel->setText("Translation:");
    ui->resultsPerPageLabel->setText("Results per Page:");
    ui->caseSensitiveCheckBox->setText("Case Sensitive");
    ui->caseSensitiveCheckBox->setToolTip("Distinguish between uppercase and lowercase letters.");
    ui->wholeWordsCheckBox->setText("Whole Words Only");
    ui->wholeWordsCheckBox->setToolTip("Discard word fragments while searching.");
    ui->exactPhraseRadioButton->setText("Exact Phrase");
    ui->allWordsRadioButton->setText("All of the Words");
    ui->anyWordsRadioButton->setText("Any of the Words");
    ui->resultsLabel->setText("Results:");
    ui->prevButton->setToolTip("Previous page");
    ui->nextButton->setToolTip("Next page");
    ui->randomVerseButton->setText("Random Verse");
    ui->randomVerseButton->setToolTip("Returns a radom verse from the specified range.");
    ui->tabWidget->setTabText(3, "Dictionary");
    ui->openDictionaryButton->setText("Open Dicionary Module");
    ui->searchDictionaryButton->setText("Search");
    ui->definitionLabel->setText("Definition:");
    ui->tabWidget->setTabText(4, "Topics");
    ui->openTopicModuleButton->setText("Open Topic Module");
    copyMessage = "Text copied to clipboard.";
    emptyMessage = "The search box cannot be empty.";
    inTotal = " in total); ";
    noMatches = "No matches; ";
    openBblFilter = "MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)";
    openCaption = "Open MYBIBLE Module";
    openDctFilter = "MYBIBLE Modules (*.dct.mybible);;All Files (*.*)";
    resultsMessage = " verses for \"";
    unavailable = "<center><h2>Unavailable in this module.</h2></center>";
    verses = "Verses ";
    verseInTotal = "Verse 1 (1 in total); ";
    warningCaption = "Error";
    languageChanging = true;
    populateDivisionMenuItemsEnglish();
    populateChapterMenuItemsEnglish();
    languageChanging = false;
    currentLanguage = "ENG";
}

void MainWindow::changeLanguageToPolski()
{
    ui->actionEnglish->setChecked(false);
    ui->actionPolski->setChecked(true);
    ui->menuFile->setTitle("Plik");
    ui->actionOpen_Bible_Module->setText("Otwórz Moduł Biblii");
    ui->menuRecently_Opened->setTitle("Ostatnio Otwarte");
    ui->actionExit->setText("Wyjście");
    ui->menuEdit->setTitle("Edycja");
    ui->actionCopy->setText("Kopiuj");
    ui->menuTools->setTitle("Narzędzia");
    ui->actionSearch->setText("Szukaj");
    ui->menuOptions->setTitle("Opcje");
    ui->actionPreferences->setText("Preferencje");
    ui->menuLanguage->setTitle("Język");
    ui->menuHelp->setTitle("Pomoc");
    ui->actionShow_Help->setText("Pokaż Pomoc");
    ui->actionAbout->setText("O Programie");
    ui->tabWidget->setTabText(0, "Biblia");
    ui->bookLabel->setText("Księga:");
    ui->chapterLabel->setText("Rozdział:");
    ui->verseLabel->setText("Wersety:");
    ui->prevChapterButton->setToolTip("Poprzedni rozdział");
    ui->nextChapterButton->setToolTip("Następny rozdział");
    ui->copyButton->setText("Kopiuj");
    ui->copyButton->setToolTip("Kopiuje wybrany zakres wersetów do schowka.");
    ui->tabWidget->setTabText(1, "Szczegóły");
    ui->descriptionLabel->setText("Opis:");
    ui->abbreviationLabel->setText("Oznaczenie:");
    ui->commentsLabel->setText("Komentarze:");
    ui->versionLabel->setText("Wersja:");
    ui->versionDateLabel->setText("Data Wersji:");
    ui->publishDateLabel->setText("Data Wydania:");
    ui->rightToLeftCheckBox->setText("Prawa do Lewej");
    ui->oldTestamentCheckBox->setText("Stary Testament");
    ui->newTestamentCheckBox->setText("Nowy Testament");
    ui->strongsNumbersCheckBox->setText("Numeracja Stronga");
    ui->tabWidget->setTabText(2, "Szukaj");
    ui->enterLabel->setText("Wpisz Słowo lub Frazę:");
    ui->searchButton->setText("Szukaj");
    ui->divisionLabel->setText("Część Biblii:");
    ui->searchOptions->setText("Opcje Wyszukiwania:");
    ui->translationLabel->setText("Przekład:");
    ui->resultsPerPageLabel->setText("Wyników na Stronę:");
    ui->caseSensitiveCheckBox->setText("Wielkość Liter");
    ui->caseSensitiveCheckBox->setToolTip("Rozróżniaj między wielkimi a małymi literami.");
    ui->wholeWordsCheckBox->setText("Tylko Całe Słowa");
    ui->wholeWordsCheckBox->setToolTip("Odrzuć fragmenty słów przy wyszukiwaniu.");
    ui->exactPhraseRadioButton->setText("Dosłowna Fraza");
    ui->allWordsRadioButton->setText("Wszystkie ze Słów");
    ui->anyWordsRadioButton->setText("Dowolne ze Słów");
    ui->resultsLabel->setText("Wyniki:");
    ui->prevButton->setToolTip("Poprzednia strona");
    ui->nextButton->setToolTip("Następna strona");
    ui->randomVerseButton->setText("Losowy Werset");
    ui->randomVerseButton->setToolTip("Zwraca losowy werset z wybranego zakresu.");
    ui->tabWidget->setTabText(3, "Słownik");
    ui->openDictionaryButton->setText("Otwórz Moduł Słownika");
    ui->searchDictionaryButton->setText("Szukaj");
    ui->definitionLabel->setText("Definicja:");
    ui->tabWidget->setTabText(4, "Tematy");
    ui->openTopicModuleButton->setText("Otwórz Moduł Tematyczny");
    copyMessage = "Tekst skopiowany do schowka.";
    emptyMessage = "Pole wyszukiwania nie może być puste.";
    inTotal = " w sumie); ";
    noMatches = "Brak wyników; ";
    openBblFilter = "Moduły MYBIBLE (*.bbl.mybible);;Wszystkie Pliki (*.*)";
    openCaption = "Otwórz Moduł MYBIBLE";
    openDctFilter = "Moduły MYBIBLE (*.dct.mybible);;Wszystkie Pliki (*.*)";
    resultsMessage = " wersetów dla \"";
    unavailable = "<center><h2>Niedostępne w tym module.</h2></center>";
    verses = "Wersety ";
    verseInTotal = "Werset 1 (1 w sumie); ";
    warningCaption = "Błąd";
    languageChanging = true;
    populateDivisionMenuItemsPolski();
    populateChapterMenuItemsPolski();
    languageChanging = false;
    currentLanguage = "PL";
}

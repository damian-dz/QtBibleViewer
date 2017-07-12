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
    if (ui->compareBookListWidget->count() > 0) {
        ui->compareBookListWidget->blockSignals(true);
        int currentIndex = ui->compareBookListWidget->currentRow();
        ui->compareBookListWidget->clear();
        ui->compareBookListWidget->addItems(bookNames);
        ui->compareBookListWidget->setCurrentRow(currentIndex);
        ui->compareBookListWidget->blockSignals(false);
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
    ui->menuFile->setTitle(QStringLiteral("File"));
    ui->actionOpen_Bible_Module->setText(QStringLiteral("Open Bible Module"));
    ui->menuRecently_Opened->setTitle(QStringLiteral("Recently Opened"));
    ui->actionExit->setText(QStringLiteral("Exit"));
    ui->menuEdit->setTitle(QStringLiteral("Edit"));
    ui->actionCopy->setText(QStringLiteral("Copy"));
    ui->actionBack->setText(QStringLiteral("Back"));
    ui->actionForward->setText(QStringLiteral("Forward"));
    ui->menuTools->setTitle(QStringLiteral("Tools"));
    ui->menuStatistics->setTitle(QStringLiteral("Statistics"));
    ui->actionWord_Frequency->setText(QStringLiteral("Word Frequency"));
    ui->actionSearch->setText(QStringLiteral("Search"));
    ui->menuOptions->setTitle(QStringLiteral("Options"));
    ui->actionPreferences->setText(QStringLiteral("Preferences"));
    ui->menuLanguage->setTitle(QStringLiteral("Language"));
    ui->menuView->setTitle(QStringLiteral("View"));
    ui->actionIncrease_Font_Size->setText(QStringLiteral("Increase Font Size"));
    ui->actionDecrease_Font_Size->setText(QStringLiteral("Decrease Font Size"));
    ui->menuHelp->setTitle(QStringLiteral("Help"));
    ui->actionShow_Help->setText(QStringLiteral("Show Help"));
    ui->actionAbout->setText(QStringLiteral("About"));
    ui->actionAbout_Qt->setText(QStringLiteral("About Qt"));
    ui->tabWidget->setTabText(0, QStringLiteral("Bible"));
    ui->bookLabel->setText(QStringLiteral("Book:"));
    ui->chapterLabel->setText(QStringLiteral("Chapter:"));
    ui->verseLabel->setText(QStringLiteral("Verses:"));
    ui->prevChapterButton->setToolTip(QStringLiteral("Previous chapter"));
    ui->nextChapterButton->setToolTip(QStringLiteral("Next chapter"));
    ui->copyButton->setText(QStringLiteral("Copy"));
    ui->copyButton->setToolTip(QStringLiteral("Copies the selected range of verses to the clipboard."));
    ui->tabWidget->setTabText(1, QStringLiteral("Details"));
    ui->descriptionLabel->setText(QStringLiteral("Description:"));
    ui->abbreviationLabel->setText(QStringLiteral("Abbreviation:"));
    ui->commentsLabel->setText(QStringLiteral("Comments:"));
    ui->versionLabel->setText(QStringLiteral("Version:"));
    ui->versionDateLabel->setText(QStringLiteral("Version Date:"));
    ui->publishDateLabel->setText(QStringLiteral("Publish Date:"));
    ui->rightToLeftCheckBox->setText(QStringLiteral("Right to Left"));
    ui->oldTestamentCheckBox->setText(QStringLiteral("Old Testament"));
    ui->newTestamentCheckBox->setText(QStringLiteral("New Testament"));
    ui->strongsNumbersCheckBox->setText(QStringLiteral("Strong's Numbers"));
    ui->tabWidget->setTabText(2, QStringLiteral("Search"));
    ui->enterLabel->setText(QStringLiteral("Enter a Word or Phrase:"));
    ui->searchButton->setText(QStringLiteral("Search"));
    ui->divisionLabel->setText(QStringLiteral("Bible Section:"));
    ui->searchOptions->setText(QStringLiteral("Search Options:"));
    ui->translationLabel->setText(QStringLiteral("Translation:"));
    ui->resultsPerPageLabel->setText(QStringLiteral("Results per Page:"));
    ui->caseSensitiveCheckBox->setText(QStringLiteral("Case Sensitive"));
    ui->caseSensitiveCheckBox->setToolTip(QStringLiteral("Distinguish between uppercase and lowercase letters."));
    ui->wholeWordsCheckBox->setText(QStringLiteral("Whole Words Only"));
    ui->wholeWordsCheckBox->setToolTip(QStringLiteral("Discard word fragments while searching."));
    ui->exactPhraseRadioButton->setText(QStringLiteral("Exact Phrase"));
    ui->allWordsRadioButton->setText(QStringLiteral("All of the Words"));
    ui->anyWordsRadioButton->setText(QStringLiteral("Any of the Words"));
    ui->byStrongsNumberRadioButton->setText(QStringLiteral("By Strong's Number"));
    ui->resultsLabel->setText(QStringLiteral("Results:"));
    ui->prevButton->setToolTip(QStringLiteral("Previous page"));
    ui->nextButton->setToolTip(QStringLiteral("Next page"));
    ui->randomVerseButton->setText(QStringLiteral("Random Verse"));
    ui->randomVerseButton->setToolTip(QStringLiteral("Returns a radom verse from the specified range."));
    ui->tabWidget->setTabText(3, QStringLiteral("Compare"));
    ui->compareBookLabel->setText(QStringLiteral("Book:"));
    ui->compareChapterLabel->setText(QStringLiteral("Chapter:"));
    ui->compareVerseLabel->setText(QStringLiteral("Verse:"));
    ui->tabWidget->setTabText(4, QStringLiteral("Favorites"));
    ui->favoritePassagesLabel->setText(QStringLiteral("Favorite Passages:"));
    ui->favoriteCommentLabel->setText(QStringLiteral("Comment:"));
    ui->deleteButton->setText(QStringLiteral("Delete"));
    ui->saveButton->setText(QStringLiteral("Save"));
    ui->tabWidget->setTabText(5, QStringLiteral("Dictionary"));
    ui->openDictionaryButton->setText(QStringLiteral("Open Dicionary Module"));
    ui->numberLabel->setText(QStringLiteral("Number:"));
    ui->allEntriesLabel->setText(QStringLiteral("All Entries:"));
    ui->definitionLabel->setText(QStringLiteral("Definition:"));
    ui->tabWidget->setTabText(6, QStringLiteral("Topics"));
    ui->openTopicModuleButton->setText(QStringLiteral("Open Topic Module"));
    ui->versesLabel->setText(QStringLiteral("Verses:"));
    areYouSure = "Delete entry ";
    cntxtActAddVerse = "Add Verse ";
    cntxtActAddVerses = "Add Verses ";
    cntxtActCopy = "Copy";
    cntxtActCopyFormatted = "Copy with Reference";
    cntxtActSelectAll = "Select All";
    cntxtActToFavorites = " to Favorites";
    confirmDeletion = "Confirm Deletion";
    containsOnlyNT = "The current transation contains only the New Testament.";
    contextActionBack = "Back";
    contextActionForward = "Forward";
    copyMessage = "Text copied to clipboard.";
    emptyMessage = "The search box cannot be empty.";
    entryExists = "An entry for this passage already exists.";
    entryUpdated = "Entry updated.";
    inTotal = " in total); ";
    noMatches = "No matches; ";
    openBblFilter = "MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)";
    openCaption = "Open MYBIBLE Module";
    openDctFilter = "MYBIBLE Modules (*.dct.mybible);;All Files (*.*)";
    replyNo = "No";
    replyYes = "Yes";
    resultsMessage = " verses for \"";
    titleError = "Error";
    unavailable = "Unavailable in this module.";
    verseInTotal = "Verse 1 (1 in total); ";
    verses = "Verses ";
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
    ui->menuFile->setTitle(QStringLiteral("Plik"));
    ui->actionOpen_Bible_Module->setText(QStringLiteral("Otwórz Moduł Biblii"));
    ui->menuRecently_Opened->setTitle(QStringLiteral("Ostatnio Otwarte"));
    ui->actionExit->setText(QStringLiteral("Wyjście"));
    ui->menuEdit->setTitle(QStringLiteral("Edycja"));
    ui->actionCopy->setText(QStringLiteral("Kopiuj"));
    ui->actionBack->setText(QStringLiteral("Do Tyłu"));
    ui->actionForward->setText(QStringLiteral("Do Przodu"));
    ui->menuTools->setTitle(QStringLiteral("Narzędzia"));
    ui->menuStatistics->setTitle(QStringLiteral("Statystyki"));
    ui->actionWord_Frequency->setText(QStringLiteral("Częstość Występowania Słowa"));
    ui->actionSearch->setText(QStringLiteral("Szukaj"));
    ui->menuOptions->setTitle(QStringLiteral("Opcje"));
    ui->actionPreferences->setText(QStringLiteral("Preferencje"));
    ui->menuLanguage->setTitle(QStringLiteral("Język"));
    ui->menuView->setTitle(QStringLiteral("Widok"));
    ui->actionIncrease_Font_Size->setText(QStringLiteral("Zwiększ Rozmiar Czcionki"));
    ui->actionDecrease_Font_Size->setText(QStringLiteral("Zmniejsz Rozmiar Czcionki"));
    ui->menuHelp->setTitle(QStringLiteral("Pomoc"));
    ui->actionShow_Help->setText(QStringLiteral("Pokaż Pomoc"));
    ui->actionAbout->setText(QStringLiteral("O Programie"));
    ui->actionAbout_Qt->setText(QStringLiteral("O Platformie Qt"));
    ui->tabWidget->setTabText(0, QStringLiteral("Biblia"));
    ui->bookLabel->setText(QStringLiteral("Księga:"));
    ui->chapterLabel->setText(QStringLiteral("Rozdział:"));
    ui->verseLabel->setText(QStringLiteral("Wersety:"));
    ui->prevChapterButton->setToolTip(QStringLiteral("Poprzedni rozdział"));
    ui->nextChapterButton->setToolTip(QStringLiteral("Następny rozdział"));
    ui->copyButton->setText(QStringLiteral("Kopiuj"));
    ui->copyButton->setToolTip(QStringLiteral("Kopiuje wybrany zakres wersetów do schowka."));
    ui->tabWidget->setTabText(1, QStringLiteral("Szczegóły"));
    ui->descriptionLabel->setText(QStringLiteral("Opis:"));
    ui->abbreviationLabel->setText(QStringLiteral("Oznaczenie:"));
    ui->commentsLabel->setText(QStringLiteral("Komentarze:"));
    ui->versionLabel->setText(QStringLiteral("Wersja:"));
    ui->versionDateLabel->setText(QStringLiteral("Data Wersji:"));
    ui->publishDateLabel->setText(QStringLiteral("Data Wydania:"));
    ui->rightToLeftCheckBox->setText(QStringLiteral("Prawa do Lewej"));
    ui->oldTestamentCheckBox->setText(QStringLiteral("Stary Testament"));
    ui->newTestamentCheckBox->setText(QStringLiteral("Nowy Testament"));
    ui->strongsNumbersCheckBox->setText(QStringLiteral("Numeracja Stronga"));
    ui->tabWidget->setTabText(2, QStringLiteral("Szukaj"));
    ui->enterLabel->setText(QStringLiteral("Wpisz Słowo lub Frazę:"));
    ui->searchButton->setText(QStringLiteral("Szukaj"));
    ui->divisionLabel->setText(QStringLiteral("Część Biblii:"));
    ui->searchOptions->setText(QStringLiteral("Opcje Wyszukiwania:"));
    ui->translationLabel->setText(QStringLiteral("Przekład:"));
    ui->resultsPerPageLabel->setText(QStringLiteral("Wyników na Stronę:"));
    ui->caseSensitiveCheckBox->setText(QStringLiteral("Wielkość Liter"));
    ui->caseSensitiveCheckBox->setToolTip(QStringLiteral("Rozróżniaj między wielkimi a małymi literami."));
    ui->wholeWordsCheckBox->setText(QStringLiteral("Tylko Całe Słowa"));
    ui->wholeWordsCheckBox->setToolTip(QStringLiteral("Odrzuć fragmenty słów przy wyszukiwaniu."));
    ui->exactPhraseRadioButton->setText(QStringLiteral("Dosłowna Fraza"));
    ui->allWordsRadioButton->setText(QStringLiteral("Wszystkie ze Słów"));
    ui->anyWordsRadioButton->setText(QStringLiteral("Dowolne ze Słów"));
    ui->byStrongsNumberRadioButton->setText(QStringLiteral("Po Numerze Stronga"));
    ui->resultsLabel->setText(QStringLiteral("Wyniki:"));
    ui->prevButton->setToolTip(QStringLiteral("Poprzednia strona"));
    ui->nextButton->setToolTip(QStringLiteral("Następna strona"));
    ui->randomVerseButton->setText(QStringLiteral("Losowy Werset"));
    ui->randomVerseButton->setToolTip(QStringLiteral("Zwraca losowy werset z wybranego zakresu."));
    ui->tabWidget->setTabText(3, QStringLiteral("Porównaj"));
    ui->compareBookLabel->setText(QStringLiteral("Księga:"));
    ui->compareChapterLabel->setText(QStringLiteral("Rozdział:"));
    ui->compareVerseLabel->setText(QStringLiteral("Werset:"));
    ui->tabWidget->setTabText(4, QStringLiteral("Ulubione"));
    ui->favoritePassagesLabel->setText(QStringLiteral("Ulubione Fragmenty:"));
    ui->favoriteCommentLabel->setText(QStringLiteral("Komentarz:"));
    ui->deleteButton->setText(QStringLiteral("Usuń"));
    ui->saveButton->setText(QStringLiteral("Zapisz"));
    ui->tabWidget->setTabText(5, QStringLiteral("Słownik"));
    ui->openDictionaryButton->setText(QStringLiteral("Otwórz Moduł Słownika"));
    ui->numberLabel->setText(QStringLiteral("Numer:"));
    ui->allEntriesLabel->setText(QStringLiteral("Wszystkie Hasła:"));
    ui->definitionLabel->setText(QStringLiteral("Definicja:"));
    ui->tabWidget->setTabText(6, QStringLiteral("Tematy"));
    ui->openTopicModuleButton->setText(QStringLiteral("Otwórz Moduł Tematyczny"));
    ui->versesLabel->setText(QStringLiteral("Wersety:"));
    areYouSure = "Usunąć wpis ";
    cntxtActAddVerse = "Dodaj Werset ";
    cntxtActAddVerses = "Dodaj Wersety ";
    cntxtActCopy = "Kopiuj";
    cntxtActCopyFormatted = "Kopiuj z Odnośnikiem";
    cntxtActSelectAll = "Zaznacz Wszystko";
    cntxtActToFavorites = " do Ulubionych";
    confirmDeletion = "Potwierdź Usunięcie";
    containsOnlyNT = "Wybrane tłumaczenie zawiera tylko Nowy Testament.";
    contextActionBack = "Do Tyłu";
    contextActionForward = "Do Przodu";
    copyMessage = "Tekst skopiowany do schowka.";
    emptyMessage = "Pole wyszukiwania nie może być puste.";
    entryExists = "Już istnieje wpis dla tego fragmentu.";
    entryUpdated = "Wpis zaktualizowany.";
    inTotal = " w sumie); ";
    noMatches = "Brak wyników; ";
    openBblFilter = "Moduły MYBIBLE (*.bbl.mybible);;Wszystkie Pliki (*.*)";
    openCaption = "Otwórz Moduł MYBIBLE";
    openDctFilter = "Moduły MYBIBLE (*.dct.mybible);;Wszystkie Pliki (*.*)";
    replyNo = "Nie";
    replyYes = "Tak";
    resultsMessage = " wersetów dla \"";
    titleError = "Błąd";
    unavailable = "Niedostępne w tym module.";
    verseInTotal = "Werset 1 (1 w sumie); ";
    verses = "Wersety ";
    warningCaption = "Błąd";
    languageChanging = true;
    populateDivisionMenuItemsPolski();
    populateChapterMenuItemsPolski();
    languageChanging = false;
    currentLanguage = "PL";
}

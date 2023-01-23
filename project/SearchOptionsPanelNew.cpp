#include "SearchOptionsPanelNew.h"

SearchOptionsPanelNew::SearchOptionsPanelNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent) :
    QGroupBox(parent),
    m_pConfig(&config),
    m_pDatabaseService(&databaseService),
    m_popup0("combobox-popup: 0"),
    ui_CheckBox_CaseSensitive(new QCheckBox),
    ui_CheckBox_WholeWordsOnly(new QCheckBox),
    ui_ComboBox_From(new QComboBox),
    ui_ComboBox_Range(new QComboBox),
    ui_ComboBox_ResPerPage(new QComboBox),
    ui_ComboBox_To(new QComboBox),
    ui_ComboBox_Translation(new QComboBox),
    ui_Label_From(new QLabel),
    ui_Label_Range(new QLabel),
    ui_Label_ResultsPerPassage(new QLabel),
    ui_Label_To(new QLabel),
    ui_Label_Translation(new QLabel),
    ui_RadioButton_ExactPhrase(new QRadioButton),
    ui_RadioButton_AllWords(new QRadioButton),
    ui_RadioButton_AnyWords(new QRadioButton),
    ui_RadioButton_ByStrongsNumber(new QRadioButton),
    ui_RadioButton_ByVerseNumber(new QRadioButton)
{
    ui_ComboBox_Translation->setStyleSheet(m_popup0);
    ui_ComboBox_Translation->addItems(m_pDatabaseService->BibleShortNames());

    ui_ComboBox_Range->setStyleSheet(m_popup0);
    ui_ComboBox_Range->setMaxVisibleItems(12);
    ui_ComboBox_Range->setCurrentIndex(0);

    ui_ComboBox_From->setStyleSheet(m_popup0);
    ui_ComboBox_From->addItems(m_pDatabaseService->BookNames());
    ui_ComboBox_From->setMaxVisibleItems(30);

    ui_ComboBox_To->setStyleSheet(m_popup0);
    ui_ComboBox_To->addItems(m_pDatabaseService->BookNames());
    ui_ComboBox_To->setMaxVisibleItems(30);
    ui_ComboBox_To->setCurrentIndex(ui_ComboBox_To->count() - 1);

    ui_RadioButton_ExactPhrase->setChecked(true);

    ui_ComboBox_ResPerPage->setStyleSheet(m_popup0);
    ui_ComboBox_ResPerPage->setMaximumWidth(40);
    ui_ComboBox_ResPerPage->addItems(QStringList({"10", "15", "25", "30", "40", "50", "75", "100"}));
    ui_ComboBox_ResPerPage->setCurrentIndex(2);

    QHBoxLayout *resPerPageHorLayout = new QHBoxLayout;
    resPerPageHorLayout->addWidget(ui_ComboBox_ResPerPage);
    resPerPageHorLayout->addWidget(ui_Label_ResultsPerPassage);

    QFormLayout *mainFormLayout = new QFormLayout;
    mainFormLayout->addRow(ui_Label_Translation, ui_ComboBox_Translation);
    mainFormLayout->addRow(ui_Label_Range, ui_ComboBox_Range);
    mainFormLayout->addRow(ui_Label_From, ui_ComboBox_From);
    mainFormLayout->addRow(ui_Label_To, ui_ComboBox_To);
    mainFormLayout->addRow(ui_CheckBox_CaseSensitive);
    mainFormLayout->addRow(ui_CheckBox_WholeWordsOnly);
    mainFormLayout->addRow(ui_RadioButton_ExactPhrase);
    mainFormLayout->addRow(ui_RadioButton_AllWords);
    mainFormLayout->addRow(ui_RadioButton_AnyWords);
    mainFormLayout->addRow(ui_RadioButton_ByStrongsNumber);
    mainFormLayout->addRow(ui_RadioButton_ByVerseNumber);
    mainFormLayout->addRow(resPerPageHorLayout);

    QWidget::setLayout(mainFormLayout);

    QObject::connect(ui_ComboBox_Range, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [=] (int idx) { OnRangeChanged(idx); });

    QObject::connect(ui_ComboBox_From, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [=] (int idx) { OnBookFromChanged(idx); });
    QObject::connect(ui_ComboBox_To, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [=] (int idx) { OnBookToChanged(idx); });

}

void SearchOptionsPanelNew::SetUiTexts()
{
    QGroupBox::setTitle((tr("Search Options")));

    ui_ComboBox_Range->clear();
    ui_ComboBox_Range->addItems(
                QStringList({ tr("Entire Bible"), tr("Old Testament"), tr("Pentateuch"),
                              tr("Historical Books"), tr("Poetical Books"),
                              tr("Major Prophets"), tr("Minor Prophets"), tr("New Testament"),
                              tr("Gospels & Acts"), tr("Pauline Epistles"),
                              tr("General Epistles & Revelation"), tr("Custom") }));

//    if (ui_ComboBox_From->count() > 0) {
//        ui_ComboBox_From->clear();
//        ui_ComboBox_From->addItems(*m_bookNames);
//    }

    ui_Label_Translation->setText(tr("Translation:"));
    ui_Label_Range->setText(tr("Range:"));
    ui_ComboBox_Range->setToolTip(tr("Limit the search to a specific Bible section."));
    ui_Label_From->setText(tr("From:"));
    ui_Label_To->setText(tr("To:"));
    ui_Label_ResultsPerPassage->setText(tr("Results per Page"));

    ui_CheckBox_CaseSensitive->setText(tr("Case-Sensitive"));
    ui_CheckBox_CaseSensitive->setToolTip(tr("Make the search case-sensitive."));
    ui_CheckBox_WholeWordsOnly->setText(tr("Whole Words Only"));
    ui_CheckBox_WholeWordsOnly->setToolTip(tr("Ignore word fragments."));

    ui_RadioButton_ExactPhrase->setText(tr("Exact Phrase"));
    ui_RadioButton_ExactPhrase->setToolTip(
                tr("Look for verses that contain the exact sequence of characters."));
    ui_RadioButton_AllWords->setText(tr("All of the Words"));
    ui_RadioButton_AllWords->setToolTip(
                tr("Look for verses that contain all of the words (in any order)."));
    ui_RadioButton_AnyWords->setText(tr("Any of the Words"));
    ui_RadioButton_AnyWords->setToolTip(
                tr("Look for verses that contain at least one of the words."));
    ui_RadioButton_ByStrongsNumber->setText(tr("Strong's Number"));
    ui_RadioButton_ByStrongsNumber->setToolTip(
                tr("Look for verses containing the specified Strong's Number (if available)."));
    ui_RadioButton_ByVerseNumber->setText(tr("Verse References"));
    ui_RadioButton_ByVerseNumber->setToolTip(
                tr("Look for verses with the specified locations."));
}

SearchOptions SearchOptionsPanelNew::GetSearchOptions()
{
    int translation = ui_ComboBox_Translation->currentIndex();
    int bookFrom = ui_ComboBox_From->currentIndex() + 1;
    int bookTo = ui_ComboBox_To->currentIndex() + 1;
    bool caseSensitive = ui_CheckBox_CaseSensitive->isChecked();
    bool wholeWordsOnly = ui_CheckBox_WholeWordsOnly->isChecked();
    SearchMode searchMode = SearchMode::exactPhrase;
    if (ui_RadioButton_AllWords->isChecked()) {
        searchMode = SearchMode::allWords;
    } else if (ui_RadioButton_AnyWords->isChecked()) {
        searchMode = SearchMode::anyWords;
    } else if (ui_RadioButton_ByStrongsNumber->isChecked()) {
        searchMode = SearchMode::byStrong;
    } else if (ui_RadioButton_ByVerseNumber->isChecked()) {
        searchMode = SearchMode::byVerses;
    }
    return { translation, bookFrom, bookTo, caseSensitive, wholeWordsOnly, searchMode };
}

int SearchOptionsPanelNew::GetBibleIndex() const
{
    return ui_ComboBox_Translation->currentIndex();
}

QString SearchOptionsPanelNew::GetBibleName() const
{
    return ui_ComboBox_Translation->currentText();
}

int SearchOptionsPanelNew::GetNumResultsPerPage() const
{
    return ui_ComboBox_ResPerPage->currentText().toInt();
}

void SearchOptionsPanelNew::OnRangeChanged(int idx)
{
    ui_ComboBox_From->blockSignals(true);
    ui_ComboBox_To->blockSignals(true);
    switch (idx) {
       case 0:
           ui_ComboBox_From->setCurrentIndex(0);
           ui_ComboBox_To->setCurrentIndex(ui_ComboBox_To->count() - 1);
           break;
       case 1:
           ui_ComboBox_From->setCurrentIndex(0);
           ui_ComboBox_To->setCurrentIndex(38);
           break;
       case 2:
           ui_ComboBox_From->setCurrentIndex(0);
           ui_ComboBox_To->setCurrentIndex(4);
           break;
       case 3:
           ui_ComboBox_From->setCurrentIndex(5);
           ui_ComboBox_To->setCurrentIndex(16);
           break;
       case 4:
           ui_ComboBox_From->setCurrentIndex(17);
           ui_ComboBox_To->setCurrentIndex(21);
           break;
       case 5:
           ui_ComboBox_From->setCurrentIndex(22);
           ui_ComboBox_To->setCurrentIndex(26);
           break;
       case 6:
           ui_ComboBox_From->setCurrentIndex(27);
           ui_ComboBox_To->setCurrentIndex(38);
           break;
       case 7:
           ui_ComboBox_From->setCurrentIndex(39);
           ui_ComboBox_To->setCurrentIndex(ui_ComboBox_To->count() - 1);
           break;
       case 8:
           ui_ComboBox_From->setCurrentIndex(39);
           ui_ComboBox_To->setCurrentIndex(43);
           break;
       case 9:
           ui_ComboBox_From->setCurrentIndex(44);
           ui_ComboBox_To->setCurrentIndex(57);
           break;
       case 10:
           ui_ComboBox_From->setCurrentIndex(58);
           ui_ComboBox_To->setCurrentIndex(ui_ComboBox_To->count() - 1);
           break;
       default:
           break;
    }
    ui_ComboBox_From->blockSignals(false);
    ui_ComboBox_To->blockSignals(false);
}

void SearchOptionsPanelNew::OnBookFromChanged(int idx)
{
    if (idx > ui_ComboBox_To->currentIndex()) {
        ui_ComboBox_To->blockSignals(true);
        ui_ComboBox_To->setCurrentIndex(idx);
        ui_ComboBox_To->blockSignals(false);
    }
    ui_ComboBox_Range->blockSignals(true);
    ui_ComboBox_Range->setCurrentIndex(ui_ComboBox_Range->count() - 1);
    ui_ComboBox_Range->blockSignals(false);
}

void SearchOptionsPanelNew::OnBookToChanged(int idx)
{
    if (idx < ui_ComboBox_From->currentIndex()) {
        ui_ComboBox_From->blockSignals(true);
        ui_ComboBox_From->setCurrentIndex(idx);
        ui_ComboBox_From->blockSignals(false);
    }
    ui_ComboBox_Range->blockSignals(true);
    ui_ComboBox_Range->setCurrentIndex(ui_ComboBox_Range->count() - 1);
    ui_ComboBox_Range->blockSignals(false);
}

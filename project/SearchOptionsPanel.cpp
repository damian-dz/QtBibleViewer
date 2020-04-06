#include "SearchOptionsPanel.h"

SearchOptionsPanel::SearchOptionsPanel(const QStringList &bookNames, const QStringList &translationNames, QWidget *parent) :
    QGroupBox(parent),
    m_bookNames(&bookNames),
    m_popup0("combobox-popup: 0")
{
    QFormLayout *mainFormLayout = new QFormLayout;

    ui_ComboBox_Translation = new QComboBox;
    ui_ComboBox_Translation->setStyleSheet(m_popup0);
    ui_ComboBox_Translation->addItems(translationNames);
    ui_Label_Translation = new QLabel;

    ui_ComboBox_Range = new QComboBox;
    ui_Label_Range = new QLabel;

    ui_ComboBox_Range->setStyleSheet(m_popup0);
    ui_ComboBox_Range->setCurrentIndex(0);

    ui_ComboBox_From = new QComboBox;
    ui_ComboBox_From->setStyleSheet(m_popup0);
    ui_ComboBox_From->addItems(bookNames);
    ui_ComboBox_From->setMaxVisibleItems(30);
    ui_Label_From = new QLabel;

    ui_ComboBox_To = new QComboBox;
    ui_ComboBox_To->setStyleSheet(m_popup0);
    ui_ComboBox_To->addItems(bookNames);
    ui_ComboBox_To->setMaxVisibleItems(30);
    ui_ComboBox_To->setCurrentIndex(bookNames.count() - 1);
    ui_Label_To = new QLabel;

    ui_CheckBox_CaseSensitive = new QCheckBox;

    ui_CheckBox_WholeWordsOnly = new QCheckBox;

    ui_RadioButton_ExactPhrase = new QRadioButton;
    ui_RadioButton_ExactPhrase->setChecked(true);

    ui_RadioButton_AllWords = new QRadioButton;

    ui_RadioButton_AnyWords = new QRadioButton;

    ui_RadioButton_ByStrong = new QRadioButton;

    ui_ComboBox_ResPerPage = new QComboBox;
    ui_ComboBox_ResPerPage->setStyleSheet(m_popup0);
    ui_ComboBox_ResPerPage->setMaximumWidth(40);
    ui_Label_ResultsPerPassage = new QLabel;
    ui_ComboBox_ResPerPage->addItems(QStringList({"10", "15", "25", "30", "40", "50", "75", "100"}));
    ui_ComboBox_ResPerPage->setCurrentIndex(2);

    QHBoxLayout *resPerPageHorLayout = new QHBoxLayout;
    resPerPageHorLayout->addWidget(ui_ComboBox_ResPerPage);
    resPerPageHorLayout->addWidget(ui_Label_ResultsPerPassage);

    mainFormLayout->addRow(ui_Label_Translation, ui_ComboBox_Translation);
    mainFormLayout->addRow(ui_Label_Range, ui_ComboBox_Range);
    mainFormLayout->addRow(ui_Label_From, ui_ComboBox_From);
    mainFormLayout->addRow(ui_Label_To, ui_ComboBox_To);
    mainFormLayout->addRow(ui_CheckBox_CaseSensitive);
    mainFormLayout->addRow(ui_CheckBox_WholeWordsOnly);
    mainFormLayout->addRow(ui_RadioButton_ExactPhrase);
    mainFormLayout->addRow(ui_RadioButton_AllWords);
    mainFormLayout->addRow(ui_RadioButton_AnyWords);
    mainFormLayout->addRow(ui_RadioButton_ByStrong);
    mainFormLayout->addRow(resPerPageHorLayout);

    QWidget::setLayout(mainFormLayout);

    // setUiTexts();

    QObject::connect(ui_ComboBox_Range, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [=] (int index) { onRangeChanged(index); });
    QObject::connect(ui_ComboBox_ResPerPage, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
                     [=] (const QString &text) { emit numResultsPerPageChanged(text.toInt()); });
}

SearchOptions SearchOptionsPanel::getSearchOptions()
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
    } else if (ui_RadioButton_ByStrong->isChecked()) {
        searchMode = SearchMode::byStrong;
    }
    return { translation, bookFrom, bookTo, caseSensitive, wholeWordsOnly, searchMode };
}

int SearchOptionsPanel::getNumResultsPerPage()
{
    return ui_ComboBox_ResPerPage->currentText().toInt();
}

void SearchOptionsPanel::setUiTexts()
{
    QGroupBox::setTitle((tr("Search Options")));

    ui_ComboBox_Range->clear();
    ui_ComboBox_Range->addItems(QStringList({tr("Entire Bible"),
                                             tr("Old Testament"),
                                             tr("Pentateuch"),
                                             tr("Historical Books"),
                                             tr("Poetical Books"),
                                             tr("Major Prophets"),
                                             tr("Minor Prophets"),
                                             tr("New Testament"),
                                             tr("Gospels & Acts"),
                                             tr("Pauline Epistles"),
                                             tr("General Epistles & Revelation"),
                                             tr("Custom")}));

    if (ui_ComboBox_From->count() > 0) {
        ui_ComboBox_From->clear();
        ui_ComboBox_From->addItems(*m_bookNames);
    }

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
    ui_RadioButton_ExactPhrase->setToolTip(tr("Look for verses that contain the exact sequence of characters."));
    ui_RadioButton_AllWords->setText(tr("All of the Words"));
    ui_RadioButton_AllWords->setToolTip(tr("Look for verses that contain all of the words (in any order)."));
    ui_RadioButton_AnyWords->setText(tr("Any of the Words"));
    ui_RadioButton_AnyWords->setToolTip(tr("Look for verses that contain at least one of the words."));
    ui_RadioButton_ByStrong->setText(tr("By Strong's Number"));
    ui_RadioButton_ByStrong->setToolTip(tr("Look for verses containing the specified Strong's Number (if available)."));
}

void SearchOptionsPanel::onRangeChanged(int index)
{
    switch (index) {
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
}

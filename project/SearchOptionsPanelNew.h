#ifndef SEARCHOPTIONSPANELNEW_H
#define SEARCHOPTIONSPANELNEW_H

#include "AppConfig.h"
#include "DatabaseService.h"

class SearchOptionsPanelNew : public QGroupBox
{
    Q_OBJECT
public:
    SearchOptionsPanelNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    void SetUiTexts();

    SearchOptions GetSearchOptions();
    int GetBibleIndex() const;
    QString GetBibleName() const;
    int GetNumResultsPerPage() const;

private:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    const QString m_popup0;

    QCheckBox *ui_CheckBox_CaseSensitive;
    QCheckBox *ui_CheckBox_WholeWordsOnly;
    QComboBox *ui_ComboBox_From;
    QComboBox *ui_ComboBox_Range;
    QComboBox *ui_ComboBox_ResPerPage;
    QComboBox *ui_ComboBox_To;
    QComboBox *ui_ComboBox_Translation;
    QLabel *ui_Label_From;
    QLabel *ui_Label_Range;
    QLabel *ui_Label_ResultsPerPassage;
    QLabel *ui_Label_To;
    QLabel *ui_Label_Translation;
    QRadioButton *ui_RadioButton_ExactPhrase;
    QRadioButton *ui_RadioButton_AllWords;
    QRadioButton *ui_RadioButton_AnyWords;
    QRadioButton *ui_RadioButton_ByStrongsNumber;
    QRadioButton *ui_RadioButton_ByVerseNumber;

    void OnRangeChanged(int idx);

    void OnBookFromChanged(int idx);
    void OnBookToChanged(int idx);
};

#endif // SEARCHOPTIONSPANELNEW_H

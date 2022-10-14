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
    QRadioButton *ui_RadioButton_AllWords;
    QRadioButton *ui_RadioButton_AnyWords;
    QRadioButton *ui_RadioButton_ByStrong;
    QRadioButton *ui_RadioButton_ExactPhrase;

    void OnRangeChanged(int idx);
};

#endif // SEARCHOPTIONSPANELNEW_H

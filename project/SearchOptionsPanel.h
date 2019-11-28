#ifndef SEARCHOPTIONSPANEL_H
#define SEARCHOPTIONSPANEL_H

#include "SearchOptions.h"

#include "precomp.h"

class SearchOptionsPanel : public QGroupBox
{
    Q_OBJECT
public:
    SearchOptionsPanel(const QStringList &bookNames, const QStringList &translationNames, QWidget *parent = nullptr);

    SearchOptions getSearchOptions();
    int getNumResultsPerPage();
    void setUiTexts();

signals:
    void numResultsPerPageChanged(int numResults);

private:
    QComboBox *ui_ComboBox_From;
    QComboBox *ui_ComboBox_To;
    QComboBox *ui_ComboBox_Translation;
    QComboBox *ui_ComboBox_Range;
    QComboBox *ui_ComboBox_ResPerPage;
    QCheckBox *ui_CheckBox_CaseSensitive;
    QCheckBox *ui_CheckBox_WholeWordsOnly;
    QRadioButton *ui_RadioButton_ExactPhrase;
    QRadioButton *ui_RadioButton_AllWords;
    QRadioButton *ui_RadioButton_AnyWords;
    QRadioButton *ui_RadioButton_ByStrong;

    const QStringList *m_bookNames;

    QLabel *ui_Label_Translation;
    QLabel *ui_Label_Range;
    QLabel *ui_Label_From;
    QLabel *ui_Label_To;
    QLabel *ui_Label_ResultsPerPassage;

private slots:
    void onRangeChanged(int index);
};

#endif // SEARCHOPTIONSPANEL_H

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
    int GetNumResultsPerPage() const;
    int GetBookFrom() const;
    int GetBookTo() const;
    int GetTranslationIndex() const;
    void SetUiTexts();

signals:
    void numResultsPerPageChanged(int numResults);

private:
    const QStringList *m_bookNames;
    QString m_popup0;

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

private slots:
    void OnRangeChanged(int index);
};

#endif // SEARCHOPTIONSPANEL_H

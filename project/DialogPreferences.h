#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

#include "AppConfig.h"

class DialogPreferences : public QDialog
{
    Q_OBJECT
public:
    DialogPreferences(AppConfig *config,  const QStringList &languages, const QString &lang, QFont font,
                      QWidget *parent = nullptr);
    ~DialogPreferences();

    QString getLanguage() const;
    void UpdateSettings();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);
    void colorPushButtonClicked();

    void referenceFormattingToggled(bool);

private:
    AppConfig *m_pConfig;
    QComboBox *m_styleComboBox;
    QComboBox *m_tabPosComboBox;
    QCheckBox *m_backgroundCheckBox;
    QStackedWidget *m_stackedWidget;
    QTextBrowser *m_TextBrowser_FontPreview;
    QColor m_highlightColor;
    QSpinBox *m_maxRecentSpinBox;
    QComboBox *m_langComboBox;
    QComboBox *m_animateChartComboBox;

    QCheckBox *ui_CheckBox_IncludeNumbers;
    QRadioButton *ui_RadioButton_Before;
    QTextBrowser *ui_TextBrowser_Preview;

    QLineEdit *ui_TextEdit_SymBefore;
    QLineEdit *ui_TextEdit_SymAfter;

    void ConnectSignals();
    void generateGeneralWidget(const QStringList &languages, const QString &lang);
    void generateFontWidget(const QFont &font);
    void generateAppearanceWidget();
    void GenerateFormattingWidget();
    QString GetPreviewString(bool before, bool includeNumbers) const;
};


#endif // PDIALOGPREFERENCES_H

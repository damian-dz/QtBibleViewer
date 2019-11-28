#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

#include "AppConfig.h"

class DialogPreferences : public QDialog
{
    Q_OBJECT
public:
    DialogPreferences(AppConfig *pConfig,  const QStringList &languages, const QString &lang, QFont font,
                      QWidget *parent = nullptr);
    ~DialogPreferences();

    QString getLanguage();
    int getLanguageIndex();
    void updateSettings();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);
    void colorPushButtonClicked();

    void referenceFormattingToggled(bool);

private:
    void generateGeneralWidget(const QStringList &languages, const QString &lang);
    void generateFontWidget(const QFont &font);
    void generateAppearanceWidget();
    void generateFormattingWidget();
    QString getPreviewString(bool before, bool includeNumbers);

    AppConfig *m_pConfig;
    QComboBox *m_styleComboBox;
    QComboBox *m_tabPosComboBox;
    QCheckBox *m_backgroundCheckBox;
    QStackedWidget *m_stackedWidget;
    QTextBrowser *m_fontAbcTextBrowser;
    QColor m_highlightColor;
    QSpinBox *m_maxRecentSpinBox;
    QComboBox *m_langComboBox;
    QComboBox *m_animateChartComboBox;

    QCheckBox *ui_ComboBox_IncludeNumbers;
    QRadioButton *ui_RadioButton_Before;
    QTextBrowser *ui_TextBrowser_Preview;
};


#endif // PDIALOGPREFERENCES_H

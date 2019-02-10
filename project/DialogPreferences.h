#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

#include "AppConfig.h"

class DialogPreferences : public QDialog
{
    Q_OBJECT
public:
    DialogPreferences(AppConfig *pConfig,
                      int langIdx,
                      QFont font,
                      QWidget *parent = nullptr);
    ~DialogPreferences();

    int getLanguageIndex();
    void updateSettings();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);
    void colorPushButtonClicked();

private:
    void generateGeneralWidget(int langIdx, int maxPassages);
    void generateFontWidget(const QFont &font);
    void generateAppearanceWidget(const QString &style, bool useBackgrnd, int tabPos);

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
};


#endif // PDIALOGPREFERENCES_H

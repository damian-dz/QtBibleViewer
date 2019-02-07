#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

#include "precomp.h"

class DialogPreferences : public QDialog
{
    Q_OBJECT
public:
    DialogPreferences(int langIdx,
                      int maxPassages,
                      const QString &style,
                      bool useBckgrnd,
                      const QColor &hghlhtClr,
                      const QFont &font,
                      int tabPos,
                      QWidget *parent = nullptr);
    ~DialogPreferences();

    QString getWindowStyle();
    bool getUseBackground();
    QString getFontFamily();
    int getFontSize();
    QColor getHighlightColor();
    int getMaxRecentPassages();
    int getTabPosition();
    int getLanguageIndex();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);
    void colorPushButtonClicked();

private:
    void generateGeneralWidget(int langIdx, int maxPassages);
    void generateFontWidget(const QFont &font);
    void generateAppearanceWidget(const QString &style, bool useBackgrnd, int tabPos);

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

#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>

class PDialogPreferences : public QDialog
{
    Q_OBJECT

public:
    PDialogPreferences(int maxPassages,
                       const QString &style,
                       bool useBckgrnd,
                       const QColor &highClr,
                       const QFont &font,
                       int tabPos,
                       QWidget *parent = nullptr);
    ~PDialogPreferences();

    QString getWindowStyle();
    bool getUseBackground();
    QString getFontFamily();
    int getFontSize();
    QColor getHighlightColor();
    int getMaxRecentPassages();
    int getTabPosition();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);
    void colorPushButtonClicked();

private:
    void generateGeneralWidget(int maxPassages);
    void generateFontWidget(const QFont &font);
    void generateAppearanceWidget(const QString &style, bool useBackgrnd, int tabPos);

    QComboBox *m_styleComboBox;
    QComboBox *m_tabPosComboBox;
    QCheckBox *m_backgroundCheckBox;
    QStackedWidget *m_stackedWidget;
    QTextBrowser *m_fontAbcTextBrowser;
    QColor m_highlightColor;
    QSpinBox *m_maxRecentSpinBox;
};


#endif // PDIALOGPREFERENCES_H

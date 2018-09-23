#ifndef PDIALOGPREFERENCES_H
#define PDIALOGPREFERENCES_H

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

class PDialogPreferences : public QDialog
{
    Q_OBJECT

public:
    PDialogPreferences(const QFont &font, QWidget *parent = 0);
    ~PDialogPreferences();

    QString getFontFamily();
    int getFontSize();

public slots:
    void listWidgetCurrentRowChanged(int currentRow);
    void currentFontTypeChanged(const QFont &font);
    void currentFontSizeChanged(const QString &text);

private:
    void generateFontWidget(const QFont &font);

    QStackedWidget *m_stackedWidget;
    QTextBrowser *m_fontAbcTextBrowser;
};


#endif // PDIALOGPREFERENCES_H

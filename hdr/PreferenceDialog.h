#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(int size,
                              const QString &family,
                              const QString &lang,
                              bool firstChapter,
                              int recentCount,
                              QWidget *parent = 0);
    ~PreferenceDialog();
    QFont getFont();
    int getMaxRecentPassages();
    bool loadFirstChapter();

private slots:
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_fontSizeComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::PreferenceDialog *ui;
    void changeLanguage(const QString &language);
};

#endif // PREFERENCEDIALOG_H

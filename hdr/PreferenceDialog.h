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
    explicit PreferenceDialog(int size, QString family, QString lang, QWidget *parent = 0);
    QFont getFont();
    ~PreferenceDialog();

private slots:
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_fontSizeComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::PreferenceDialog *ui;
    void changeLanguage(QString language);
};

#endif // PREFERENCEDIALOG_H

#include "hdr/PreferenceDialog.h"
#include "ui_PreferenceDialog.h"

#include <QMessageBox>
#include <QPushButton>

PreferenceDialog::PreferenceDialog(int size, QString family, QString lang, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    changeLanguage(lang);
    ui->fontSizeComboBox->setCurrentIndex(ui->fontSizeComboBox->findText(QString::number(size)));
    QFont font;
    if (!family.isEmpty())
        font.setFamily(family);
    ui->fontComboBox->setCurrentFont(font);
    ui->previewTextBrowser->setText("ABCabc123");
}

QFont PreferenceDialog::getFont()
{
    QFont font;
    font.setPointSize(ui->fontSizeComboBox->currentText().toInt());
    font.setFamily(ui->fontComboBox->currentFont().toString());
    return font;
}

void PreferenceDialog::on_fontComboBox_currentFontChanged(const QFont &f)
{
    QFont font = f;
    font.setPointSize(ui->fontSizeComboBox->currentText().toInt());
    ui->previewTextBrowser->setFont(font);
}

void PreferenceDialog::on_fontSizeComboBox_currentTextChanged(const QString &arg1)
{
    QFont font;
    font.setFamily(ui->fontComboBox->currentFont().toString());
    font.setPointSize(arg1.toInt());
    ui->previewTextBrowser->setFont(font);
}

void PreferenceDialog::changeLanguage(QString language)
{
    if (language == "PL") {
        setWindowTitle("Preferencje");
        ui->fontFamilyLabel->setText("Rodzaj Czcionki");
        ui->fontSizeLabel->setText("Rozmiar Czcionki");
        ui->previewLabel->setText("Podgląd");
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Anuluj");
    }
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

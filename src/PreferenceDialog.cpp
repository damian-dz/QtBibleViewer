#include "hdr/PreferenceDialog.h"
#include "ui_PreferenceDialog.h"

#include <QMessageBox>
#include <QPushButton>

PreferenceDialog::PreferenceDialog(int size,
                                   const QString &family,
                                   bool firstChapter,
                                   int recentCount,
                                   QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    ui->fontSizeComboBox->setCurrentIndex(ui->fontSizeComboBox->findText(QString::number(size)));
    QFont font;
    if (!family.isEmpty())
        font.setFamily(family);
    ui->fontComboBox->setCurrentFont(font);
    ui->previewTextBrowser->setText("ABCabc123");
    ui->loadFirstChapterCheckBox->setChecked(firstChapter);
    ui->recentCountSpinBox->setValue(recentCount);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

QFont PreferenceDialog::getFont()
{
    QFont font;
    font.setPointSize(ui->fontSizeComboBox->currentText().toInt());
    font.setFamily(ui->fontComboBox->currentFont().toString());
    return font;
}

bool PreferenceDialog::loadFirstChapter()
{
    return ui->loadFirstChapterCheckBox->isChecked();
}

int PreferenceDialog::getMaxRecentPassages()
{
    return ui->recentCountSpinBox->value();
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

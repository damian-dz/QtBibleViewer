#include "PDialogPreferences.h"

#include <QApplication>


PDialogPreferences::PDialogPreferences(const QFont &font, QWidget *parent) :
    QDialog(parent)
{
    QDialog::resize(480, 320);
    QDialog::setWindowTitle(tr("Preferences"));

    auto mainVBoxLayout = new QVBoxLayout;

    auto mainHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(mainHBoxLayout);

    m_stackedWidget = new QStackedWidget;

    auto listWidget = new QListWidget;
    listWidget->setFont(QFont(qApp->font().family(), 10));
    listWidget->setMaximumWidth(180);
    listWidget->addItem(tr("General"));
    listWidget->addItem(tr("Font Settings"));
    listWidget->addItem(tr("Appearance"));
    listWidget->setCurrentRow(0);

    mainHBoxLayout->addWidget(listWidget);
    mainHBoxLayout->addWidget(m_stackedWidget);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainVBoxLayout->addWidget(dialogButtonBox);

    generateFontWidget(font);

    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(listWidgetCurrentRowChanged(int)));

    QDialog::setLayout(mainVBoxLayout);
}

PDialogPreferences::~PDialogPreferences()
{

}

void PDialogPreferences::generateFontWidget(const QFont &font)
{
    auto fontWidget = new QWidget;

    auto fontFormLayout = new QFormLayout;

    fontWidget->setLayout(fontFormLayout);

    auto fontTypeComboBox = new QFontComboBox;
    fontTypeComboBox->setMaximumWidth(200);
    fontTypeComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    fontTypeComboBox->setCurrentFont(font);

    fontFormLayout->addRow(tr("Font Type:"), fontTypeComboBox);

    auto fontSizeComboBox = new QComboBox;
    QStringList fontSizes;
    const int smallest = 8;
    const int biggest = 20;
    for (int i = smallest; i <= biggest; ++i) {
        fontSizes << QString::number(i);
    }
    fontSizeComboBox->addItems(fontSizes);
    fontSizeComboBox->setCurrentText(QString::number(font.pointSize()));
    fontSizeComboBox->setMaxVisibleItems(biggest - smallest + 1);
    fontSizeComboBox->setMaximumWidth(60);
    fontSizeComboBox->setStyleSheet("combobox-popup: 0;");

    fontFormLayout->addRow(tr("Font Size:"), fontSizeComboBox);

    m_fontAbcTextBrowser = new QTextBrowser;
    m_fontAbcTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
    m_fontAbcTextBrowser->setMaximumHeight(80);
    m_fontAbcTextBrowser->setText(tr("ABCabc123"));
    m_fontAbcTextBrowser->setFont(font);

    fontFormLayout->addRow(tr("Preview:"), m_fontAbcTextBrowser);

    auto textBrowser = new QTextBrowser;
    m_stackedWidget->addWidget(textBrowser);

    m_stackedWidget->addWidget(fontWidget);

    auto textBrowser2 = new QTextBrowser;
    m_stackedWidget->addWidget(textBrowser2);

    connect(fontTypeComboBox, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontTypeChanged(QFont)));
    connect(fontSizeComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(currentFontSizeChanged(QString)));
}

QString PDialogPreferences::getFontFamily()
{
    return m_fontAbcTextBrowser->font().family();
}

int PDialogPreferences::getFontSize()
{
    return m_fontAbcTextBrowser->font().pointSize();
}

void PDialogPreferences::listWidgetCurrentRowChanged(int currentRow)
{
    m_stackedWidget->setCurrentIndex(currentRow);
}

void PDialogPreferences::currentFontTypeChanged(const QFont &font)
{
    int fontSize = m_fontAbcTextBrowser->font().pointSize();
    m_fontAbcTextBrowser->setFont(QFont(font.family(), fontSize));
}

void PDialogPreferences::currentFontSizeChanged(const QString &text)
{
    QString fontType = m_fontAbcTextBrowser->font().family();
    m_fontAbcTextBrowser->setFont(QFont(fontType, text.toInt()));
}


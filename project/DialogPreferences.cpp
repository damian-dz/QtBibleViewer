#include "DialogPreferences.h"

DialogPreferences::DialogPreferences(int langIdx,
                                       int maxPassages,
                                       const QString &style,
                                       bool useBckgrnd,
                                       const QColor &hghlhtClr,
                                       const QFont &font,
                                       int tabPos,
                                       QWidget *parent) :
    QDialog(parent),
    m_highlightColor(hghlhtClr)
{
    QDialog::resize(480, 320);
    QDialog::setWindowTitle(tr("Preferences"));

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout;

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout;
    mainVBoxLayout->addLayout(mainHBoxLayout);

    m_stackedWidget = new QStackedWidget;

    QListWidget *listWidget = new QListWidget;
    listWidget->setFont(QFont(qApp->font().family(), 10));
    listWidget->setMaximumWidth(180);
    listWidget->addItem(tr("General"));
    listWidget->addItem(tr("Font Settings"));
    listWidget->addItem(tr("Appearance"));
    listWidget->setCurrentRow(0);

    mainHBoxLayout->addWidget(listWidget);
    mainHBoxLayout->addWidget(m_stackedWidget);

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainVBoxLayout->addWidget(dialogButtonBox);

    generateGeneralWidget(langIdx, maxPassages);
    generateFontWidget(font);
    generateAppearanceWidget(style, useBckgrnd, tabPos);

    QObject::connect(listWidget, SIGNAL(currentRowChanged(int)),
                     this, SLOT(listWidgetCurrentRowChanged(int)));

    QDialog::setLayout(mainVBoxLayout);
}

DialogPreferences::~DialogPreferences()
{

}

void DialogPreferences::generateGeneralWidget(int langIdx, int maxPassages)
{
    QWidget *generalWidget = new QWidget;

    QFormLayout *generalFormLayout = new QFormLayout;

    m_langComboBox = new QComboBox;
    m_langComboBox->addItem("English");
    m_langComboBox->addItem("español");
    m_langComboBox->addItem("polski");
    m_langComboBox->setCurrentIndex(langIdx);
    m_langComboBox->setStyleSheet("combobox-popup: 0");
    generalFormLayout->addRow(tr("Language:"), m_langComboBox);

    m_maxRecentSpinBox = new QSpinBox;
    m_maxRecentSpinBox->setMinimum(2);
    m_maxRecentSpinBox->setMaximum(100);
    m_maxRecentSpinBox->setMaximumWidth(60);
    m_maxRecentSpinBox->setValue(maxPassages);
    generalFormLayout->addRow(tr("Max. Recent Passages:"), m_maxRecentSpinBox);

    generalWidget->setLayout(generalFormLayout);

    m_stackedWidget->addWidget(generalWidget);
}

void DialogPreferences::generateFontWidget(const QFont &font)
{
    QWidget *fontWidget = new QWidget;

    QFormLayout *fontFormLayout = new QFormLayout;

    fontWidget->setLayout(fontFormLayout);

    QFontComboBox *fontTypeComboBox = new QFontComboBox;
    fontTypeComboBox->setMaximumWidth(200);
    fontTypeComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    fontTypeComboBox->setCurrentFont(font);

    fontFormLayout->addRow(tr("Font Type:"), fontTypeComboBox);

    QComboBox *fontSizeComboBox = new QComboBox;
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
    fontSizeComboBox->setStyleSheet("combobox-popup: 0");

    fontFormLayout->addRow(tr("Font Size:"), fontSizeComboBox);

    m_fontAbcTextBrowser = new QTextBrowser;
    m_fontAbcTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
    m_fontAbcTextBrowser->setMaximumHeight(80);
    m_fontAbcTextBrowser->setText(tr("ABCabc123"));
    m_fontAbcTextBrowser->setFont(font);

    fontFormLayout->addRow(tr("Preview:"), m_fontAbcTextBrowser);

    m_stackedWidget->addWidget(fontWidget);

    QObject::connect(fontTypeComboBox, SIGNAL(currentFontChanged(QFont)),
                     this, SLOT(currentFontTypeChanged(QFont)));
    QObject::connect(fontSizeComboBox, SIGNAL(currentTextChanged(QString)),
                     this, SLOT(currentFontSizeChanged(QString)));
}


void DialogPreferences::generateAppearanceWidget(const QString &style, bool useBackgrnd, int tabPos)
{
    QWidget *appearanceWidget = new QWidget;

    QFormLayout *appearanceFormLayout = new QFormLayout;

    appearanceWidget->setLayout(appearanceFormLayout);

    m_styleComboBox = new QComboBox;
    m_styleComboBox->addItems(QStyleFactory::keys());
    m_styleComboBox->setCurrentText(style);
    m_styleComboBox->setStyleSheet("combobox-popup: 0");
    appearanceFormLayout->addRow(tr("Window Style:"), m_styleComboBox);

    m_backgroundCheckBox = new QCheckBox(tr("Use background image"));
    m_backgroundCheckBox->setChecked(useBackgrnd);
    appearanceFormLayout->addWidget(m_backgroundCheckBox);

    QPushButton *colorButton = new QPushButton(tr("Change..."));
    colorButton->setStyleSheet("background-color:" + m_highlightColor.name());
    appearanceFormLayout->addRow(tr("Verse Highlight Color:"), colorButton);
    QObject::connect(colorButton, SIGNAL(clicked()), this, SLOT(colorPushButtonClicked()));

    m_tabPosComboBox = new QComboBox;
    QStringList positions;
    positions << tr("Top") << tr("Bottom") << tr("Left") << tr("Right");
    m_tabPosComboBox->addItems(positions);
    m_tabPosComboBox->setCurrentIndex(tabPos);
    m_tabPosComboBox->setStyleSheet("combobox-popup: 0");
    appearanceFormLayout->addRow(tr("Bible Tabs Position:"), m_tabPosComboBox);

    m_animateChartComboBox = new QComboBox;
    QStringList animateOptns;
    animateOptns << tr("No Animation") << tr("Grid Axis Anmiation")
                 << tr("Series Animation") << tr("All Animations");
    m_animateChartComboBox->addItems(animateOptns);
    m_animateChartComboBox->setCurrentIndex(tabPos);
    m_animateChartComboBox->setStyleSheet("combobox-popup: 0");
    appearanceFormLayout->addRow(tr("Chart Animation Type:"), m_animateChartComboBox);

    m_stackedWidget->addWidget(appearanceWidget);
}

QString DialogPreferences::getWindowStyle()
{
    return m_styleComboBox->currentText();
}

bool DialogPreferences::getUseBackground()
{
    return m_backgroundCheckBox->isChecked();
}

QString DialogPreferences::getFontFamily()
{
    return m_fontAbcTextBrowser->font().family();
}

int DialogPreferences::getFontSize()
{
    return m_fontAbcTextBrowser->font().pointSize();
}

QColor DialogPreferences::getHighlightColor()
{
    return m_highlightColor;
}

int DialogPreferences::getMaxRecentPassages()
{
    return m_maxRecentSpinBox->value();
}

int DialogPreferences::getTabPosition()
{
    return m_tabPosComboBox->currentIndex();
}

int DialogPreferences::getLanguageIndex()
{
    return m_langComboBox->currentIndex();
}

void DialogPreferences::listWidgetCurrentRowChanged(int currentRow)
{
    m_stackedWidget->setCurrentIndex(currentRow);
}

void DialogPreferences::currentFontTypeChanged(const QFont &font)
{
    int fontSize = m_fontAbcTextBrowser->font().pointSize();
    m_fontAbcTextBrowser->setFont(QFont(font.family(), fontSize));
}

void DialogPreferences::currentFontSizeChanged(const QString &text)
{
    QString fontType = m_fontAbcTextBrowser->font().family();
    m_fontAbcTextBrowser->setFont(QFont(fontType, text.toInt()));
}

void DialogPreferences::colorPushButtonClicked()
{
    QColor highlightColor = QColorDialog::getColor(m_highlightColor, this, QString(), QColorDialog::ShowAlphaChannel);
    if (highlightColor.isValid()) {
        m_highlightColor = highlightColor;
    }
}



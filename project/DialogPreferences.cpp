#include "DialogPreferences.h"

DialogPreferences::DialogPreferences(AppConfig *config, const QStringList &languages, const QString &lang, QFont font,
                                     QWidget *parent)
    : QDialog(parent),
      m_config(config)
{
    QListWidget *listWidget = new QListWidget;
    listWidget->setFont(QFont(qApp->font().family(), 10));
    listWidget->setMaximumWidth(180);
    listWidget->addItem(tr("General"));
    listWidget->addItem(tr("Fonts"));
    listWidget->addItem(tr("Appearance"));
    listWidget->addItem(tr("Formatting"));
    listWidget->setCurrentRow(0);

    m_stackedWidget = new QStackedWidget;

    QHBoxLayout *mainHorLayout = new QHBoxLayout;
    mainHorLayout->addWidget(listWidget);
    mainHorLayout->addWidget(m_stackedWidget);

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *mainVerLayout = new QVBoxLayout(this);
    mainVerLayout->addLayout(mainHorLayout);
    mainVerLayout->addWidget(dialogButtonBox);

    generateGeneralWidget(languages, lang);
    generateFontWidget(font);
    generateAppearanceWidget();
    generateFormattingWidget();
    connectSignals();

    QObject::connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(listWidgetCurrentRowChanged(int)));
    QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QWidget::resize(480, 320);
    QWidget::setLayout(mainVerLayout);
    QWidget::setWindowTitle(tr("Preferences"));
}

DialogPreferences::~DialogPreferences()
{

}

void DialogPreferences::generateGeneralWidget(const QStringList &languages, const QString &lang)
{
    m_langComboBox = new QComboBox;
    m_langComboBox->addItems(languages);
    m_langComboBox->setCurrentText(lang);
    m_langComboBox->setStyleSheet("combobox-popup: 0");

    m_maxRecentSpinBox = new QSpinBox;
    m_maxRecentSpinBox->setMinimum(2);
    m_maxRecentSpinBox->setMaximum(100);
    m_maxRecentSpinBox->setMaximumWidth(60);
    m_maxRecentSpinBox->setValue(m_config->general.max_recent_passages);

    QFormLayout *generalFormLayout = new QFormLayout;
    generalFormLayout->addRow(tr("Language:"), m_langComboBox);
    generalFormLayout->addRow(tr("Max. Recent Passages:"), m_maxRecentSpinBox);

    QWidget *generalWidget = new QWidget;
    generalWidget->setLayout(generalFormLayout);

    m_stackedWidget->addWidget(generalWidget);
}

void DialogPreferences::generateFontWidget(const QFont &font)
{
    QFontComboBox *fontTypeComboBox = new QFontComboBox;
    fontTypeComboBox->setMaximumWidth(200);
    fontTypeComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    fontTypeComboBox->setCurrentFont(font);

    QStringList fontSizes;
    constexpr int smallest = 4;
    constexpr int biggest = 20;
    for (int i = smallest; i <= biggest; ++i) {
        fontSizes << QString::number(i);
    }
    QComboBox *fontSizeComboBox = new QComboBox;
    fontSizeComboBox->addItems(fontSizes);
    fontSizeComboBox->setCurrentText(QString::number(font.pointSize()));
    fontSizeComboBox->setMaximumWidth(60);
    fontSizeComboBox->setMaxVisibleItems(biggest - smallest + 1);
    fontSizeComboBox->setStyleSheet("combobox-popup: 0");

    m_TextBrowser_FontPreview = new QTextBrowser;
    m_TextBrowser_FontPreview->setContextMenuPolicy(Qt::NoContextMenu);
    m_TextBrowser_FontPreview->setFont(font);
    m_TextBrowser_FontPreview->setMaximumHeight(80);
    m_TextBrowser_FontPreview->setText(tr("ABCabc123"));

    QFormLayout *fontFormLayout = new QFormLayout;
    fontFormLayout->addRow(tr("Font Type:"), fontTypeComboBox);
    fontFormLayout->addRow(tr("Font Size:"), fontSizeComboBox);
    fontFormLayout->addRow(tr("Preview:"), m_TextBrowser_FontPreview);

    QWidget *fontWidget = new QWidget;
    fontWidget->setLayout(fontFormLayout);

    m_stackedWidget->addWidget(fontWidget);

    QObject::connect(fontTypeComboBox, SIGNAL(currentFontChanged(QFont)),
                     this, SLOT(currentFontTypeChanged(QFont)));
    QObject::connect(fontSizeComboBox, SIGNAL(currentTextChanged(QString)),
                     this, SLOT(currentFontSizeChanged(QString)));
}

void DialogPreferences::generateAppearanceWidget()
{
    m_styleComboBox = new QComboBox;
    m_styleComboBox->addItems(QStyleFactory::keys());
    m_styleComboBox->setCurrentText(m_config->appearance.window_style);
    m_styleComboBox->setStyleSheet("combobox-popup: 0");

    m_backgroundCheckBox = new QCheckBox(tr("Use background image"));
    m_backgroundCheckBox->setChecked(m_config->appearance.use_background_image);

    QPushButton *colorButton = new QPushButton(tr("Change..."));
    colorButton->setStyleSheet("background-color:" + m_config->appearance.verse_highlight_color.name());

    QObject::connect(colorButton, SIGNAL(clicked()), this, SLOT(colorPushButtonClicked()));

    QStringList positions;
    positions << tr("Top") << tr("Bottom") << tr("Left") << tr("Right");
    m_tabPosComboBox = new QComboBox;
    m_tabPosComboBox->addItems(positions);
    m_tabPosComboBox->setCurrentIndex(m_config->appearance.module_tab_position);
    m_tabPosComboBox->setStyleSheet("combobox-popup: 0");

    QStringList animateOptns;
    animateOptns << tr("No Animation") << tr("Grid Axis Anmiation")
                 << tr("Series Animation") << tr("All Animations");
    m_animateChartComboBox = new QComboBox;
    m_animateChartComboBox->addItems(animateOptns);
    m_animateChartComboBox->setCurrentIndex(m_config->appearance.chart_animation);
    m_animateChartComboBox->setStyleSheet("combobox-popup: 0");

    QFormLayout *appearanceFormLayout = new QFormLayout;
    appearanceFormLayout->addRow(tr("Window Style:"), m_styleComboBox);
    appearanceFormLayout->addWidget(m_backgroundCheckBox);
    appearanceFormLayout->addRow(tr("Verse Highlight Color:"), colorButton);
    appearanceFormLayout->addRow(tr("Bible Tabs Position:"), m_tabPosComboBox);
    appearanceFormLayout->addRow(tr("Chart Animation Type:"), m_animateChartComboBox);

    QWidget *appearanceWidget = new QWidget;
    appearanceWidget->setLayout(appearanceFormLayout);

    m_stackedWidget->addWidget(appearanceWidget);
}

void DialogPreferences::generateFormattingWidget()
{
    ui_TextBrowser_Preview = new QTextBrowser;
    ui_TextBrowser_Preview->setText(getPreviewString(m_config->formatting.reference_before,
                                                     m_config->formatting.include_numbers));

    ui_RadioButton_Before = new QRadioButton(tr("Before Text"));

    QRadioButton *afterRadioButton = new QRadioButton(tr("After Text"));

    if (m_config->formatting.reference_before) {
        ui_RadioButton_Before->setChecked(true);
    } else {
        afterRadioButton->setChecked(true);
    }

    QHBoxLayout *beforeAfterHorLayout = new QHBoxLayout;
    beforeAfterHorLayout->addWidget(ui_RadioButton_Before);
    beforeAfterHorLayout->addWidget(afterRadioButton);

    ui_CheckBox_IncludeNumbers = new QCheckBox(tr("Include Verse Numbers"));
    ui_CheckBox_IncludeNumbers->setChecked(m_config->formatting.include_numbers);

    QFormLayout *formattingFormLayout = new QFormLayout;
    formattingFormLayout->addRow(tr("Reference:"), beforeAfterHorLayout);
    formattingFormLayout->addWidget(ui_CheckBox_IncludeNumbers);
    formattingFormLayout->addRow(tr("Preview:"), ui_TextBrowser_Preview);

    QWidget *formattingWidget = new QWidget;
    formattingWidget->setLayout(formattingFormLayout);

    m_stackedWidget->addWidget(formattingWidget);
}

QString DialogPreferences::getPreviewString(bool before, bool includeNumbers) const
{
    QString verseText = tr("In the beginning God created the heaven and the earth.");
    if (includeNumbers) {
        verseText = "[1] " % verseText;
    }
    QString reference = tr("Genesis 1:1");
    if (before) {
        verseText = reference % ": " % verseText;
    } else {
        verseText = verseText % "—" % reference;
    }
    return verseText;
}

QString DialogPreferences::getLanguage() const
{
    return m_langComboBox->currentText();
}

void DialogPreferences::updateSettings()
{
    m_config->appearance.chart_animation = m_animateChartComboBox->currentIndex();
    m_config->appearance.module_tab_position = m_tabPosComboBox->currentIndex();
    m_config->appearance.use_background_image = m_backgroundCheckBox->isChecked();
    m_config->appearance.window_style = m_styleComboBox->currentText();
    m_config->fonts.browser_family = m_TextBrowser_FontPreview->font().family();
    m_config->fonts.browser_size = m_TextBrowser_FontPreview->font().pointSize();
    m_config->formatting.reference_before = ui_RadioButton_Before->isChecked();
    m_config->formatting.include_numbers = ui_CheckBox_IncludeNumbers->isChecked();
    m_config->general.max_recent_passages = m_maxRecentSpinBox->value();
}

void DialogPreferences::listWidgetCurrentRowChanged(int currentRow)
{
    m_stackedWidget->setCurrentIndex(currentRow);
}

void DialogPreferences::currentFontTypeChanged(const QFont &font)
{
    int fontSize = m_TextBrowser_FontPreview->font().pointSize();
    m_TextBrowser_FontPreview->setFont(QFont(font.family(), fontSize));
}

void DialogPreferences::currentFontSizeChanged(const QString &text)
{
    QString fontType = m_TextBrowser_FontPreview->font().family();
    m_TextBrowser_FontPreview->setFont(QFont(fontType, text.toInt()));
}

void DialogPreferences::colorPushButtonClicked()
{
    QColor highlightColor = QColorDialog::getColor(
                m_config->appearance.verse_highlight_color, this, QString(), QColorDialog::ShowAlphaChannel);
    if (highlightColor.isValid()) {
        m_config->appearance.verse_highlight_color = highlightColor;
    }
}

void DialogPreferences::referenceFormattingToggled(bool)
{
    ui_TextBrowser_Preview->setText(getPreviewString(ui_RadioButton_Before->isChecked(),
                                                     ui_CheckBox_IncludeNumbers->isChecked()));
}

void DialogPreferences::connectSignals()
{
    QObject::connect(ui_RadioButton_Before, QOverload<bool>::of(&QAbstractButton::toggled),
                     [=] (bool checked) { referenceFormattingToggled(checked); });
    QObject::connect(ui_CheckBox_IncludeNumbers, QOverload<bool>::of(&QAbstractButton::toggled),
                     [=] (bool checked) { referenceFormattingToggled(checked); });
}



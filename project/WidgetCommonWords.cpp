#include "WidgetCommonWords.h"

WidgetCommonWords::WidgetCommonWords(const QSqlDatabase &db, int animType, QWidget *parent)
    : QWidget(parent),
      m_db(&db),
      m_isBeingOpened(true)
{
    QWidget::setAttribute(Qt::WA_DeleteOnClose);
    generateMainLayout(animType);
}

WidgetCommonWords::~WidgetCommonWords()
{

}

void WidgetCommonWords::resizeEvent(QResizeEvent *evt)
{
    if (!m_isBeingOpened) {
        ui_PushButton_Visualize->setDisabled(true);
        QTimer::singleShot(2000, this, SLOT(enableVisualizeButton()));
    } else {
        m_isBeingOpened = false;
    }
    evt->accept();
}

void WidgetCommonWords::generateMainLayout(int animType)
{
    QWidget::setWindowTitle(tr("Common/Rare Words"));
    QWidget::resize(800, 480);

    ui_mainVerLayout = new QVBoxLayout;

    QHBoxLayout *ignoreWordsHorLayout = new QHBoxLayout;

    QLabel *ignoreWordsLabel = new QLabel(tr("Ignore words shorter than"));
    ignoreWordsHorLayout->addWidget(ignoreWordsLabel);
    ui_SpinBox_LetterCount = new QSpinBox;
    ui_SpinBox_LetterCount->setMinimum(2);
    ui_SpinBox_LetterCount->setMaximum(20);
    ignoreWordsHorLayout->addWidget(ui_SpinBox_LetterCount);
    QLabel *lettersLabel = new QLabel(tr("letters"));
    ignoreWordsHorLayout->addWidget(lettersLabel);
    ignoreWordsHorLayout->addStretch();

    ui_RadioButton_Common = new QRadioButton(tr("Common"));
    ui_RadioButton_Common->setChecked(true);
    ignoreWordsHorLayout->addWidget(ui_RadioButton_Common);
    QRadioButton *rareRadioButton = new QRadioButton(tr("Rare"));
    ignoreWordsHorLayout->addWidget(rareRadioButton);

    ui_PushButton_Visualize = new QPushButton(tr("Visualize"));
    connect(ui_PushButton_Visualize, SIGNAL(clicked()), this, SLOT(on_visualizeButton_clicked()));
    ignoreWordsHorLayout->addWidget(ui_PushButton_Visualize);

    ui_mainVerLayout->addLayout(ignoreWordsHorLayout);

    ui_chart = new QChart;
    ui_chart->layout()->setContentsMargins(2, 2, 2, 2);
    ui_chart->setAnimationOptions(QChart::AnimationOption(animType));
    ui_chart->setTheme(QChart::ChartThemeBlueIcy);
    ui_chart->setAcceptHoverEvents(true);
    QChartView *chartView = new QChartView(ui_chart, this);
    chartView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui_mainVerLayout->addWidget(chartView);

    QWidget::setLayout(ui_mainVerLayout);
}

void WidgetCommonWords::on_visualizeButton_clicked()
{
    QHash<QString, int> uniqueWords;
    int threshold = ui_SpinBox_LetterCount->value() - 1;
    QString queryString = "SELECT Scripture FROM Bible";
    QRegExp rgxMarkupNotes(",|\\.|:|;|\\?|!|\\(|\\)|<.{1,4}>|<RF>.*<Rf>");
    QSqlQuery query(*m_db);
    if (query.exec(queryString)) {
        while (query.next()) {
            QString verseNoNotes = query.record().value(0).toString().remove(rgxMarkupNotes);
            QStringList separateWords = verseNoNotes.split(" ");
            for (int i = 0; i < separateWords.count(); ++i) {
                QString currentWord = separateWords[i];
                if (currentWord.length() > threshold) {
                    if (!uniqueWords.contains(currentWord)) {
                        uniqueWords.insert(currentWord, 1);
                    } else {
                        ++uniqueWords[currentWord];
                    }
                }
            }
        }
    }
    plotResults(uniqueWords);
}

void WidgetCommonWords::plotResults(const QHash<QString, int> &uniqueWords)
{
    QList<QPair<int, QString>> pairs;
    for (QHash<QString, int>::const_iterator i = uniqueWords.cbegin(); i != uniqueWords.cend(); ++i) {
        pairs.append(qMakePair(i.value(), i.key()));
    }

    QBarSet *barSet = new QBarSet("Common/Rare Words");
    QStringList categories;

    if (ui_RadioButton_Common->isChecked()) {
        std::sort(pairs.rbegin(), pairs.rend());
    } else {
        std::sort(pairs.begin(), pairs.end());
    }
    for (int i = qMin(9, pairs.count() - 1); i >= 0; --i) {
        barSet->append(pairs[i].first);
        categories.append(pairs[i].second);
    }

    ui_PushButton_Visualize->setDisabled(true);
    ui_chart->removeAllSeries();

    QHorizontalBarSeries *horSeries = new QHorizontalBarSeries;
    horSeries->setBarWidth(1);
    horSeries->append(barSet);

    QBarCategoryAxis *axisY = new QBarCategoryAxis;
    axisY->append(categories);

    ui_chart->addSeries(horSeries);
    ui_chart->createDefaultAxes();
    ui_chart->setAxisY(axisY, horSeries);
    QValueAxis *axisX = new QValueAxis;
    axisX->setLabelFormat("%d");
    ui_chart->setAxisX(axisX, horSeries);
    ui_chart->legend()->setVisible(false);

    QTimer::singleShot(2000, this, SLOT(enableVisualizeButton()));
}

void WidgetCommonWords::enableVisualizeButton()
{
    ui_PushButton_Visualize->setEnabled(true);
}


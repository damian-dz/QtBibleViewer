#include "WidgetWordFrequency.h"

WidgetWordFrequency::WidgetWordFrequency(const QSqlDatabase &db, const QStringList &bookNames,
                                         const QStringList &bookAbbraviations, int animType, QWidget *parent)
    : QWidget(parent),
      m_isBeingOpened(true),
      m_db(&db),
      m_pBookAbbreviations(&bookAbbraviations),
      m_pBookNames(&bookNames)
{
    QWidget::setAttribute(Qt::WA_DeleteOnClose);
   // loadBookAbbreviations();
    generateMainLayout();
    setUpChartsAndValidator(animType);
    connectSignalsToSlots();
}

WidgetWordFrequency::~WidgetWordFrequency()
{

}

void WidgetWordFrequency::generateMainLayout()
{
    QWidget::setWindowTitle(tr("Word Frequency"));
    QWidget::resize(1000, 560);

    ui_mainVerLayout = new QVBoxLayout;

    QHBoxLayout *horLayout = new QHBoxLayout;
    ui_wordLineEdit = new QLineEdit;
    horLayout->addWidget(ui_wordLineEdit);

    ui_visualizeButton = new QPushButton(tr("Visualize"));
    ui_visualizeButton->setDisabled(true);
    horLayout->addWidget(ui_visualizeButton);

    ui_mainVerLayout->addLayout(horLayout);
    QWidget::setLayout(ui_mainVerLayout);
}

void WidgetWordFrequency::setUpChartsAndValidator(int animType)
{
    m_chartOT = new QChart;
    m_chartOT->layout()->setContentsMargins(2, 2, 2, 2);
    m_chartOT->setAnimationOptions(QChart::AnimationOption(animType));
    m_chartOT->setTheme(QChart::ChartThemeBlueIcy);
    m_chartOT->setAcceptHoverEvents(true);
    m_chartViewOT = new QChartView(m_chartOT, this);
    m_chartViewOT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_mainVerLayout->addWidget(m_chartViewOT);

    m_currentLabel = new QLabel;
    m_currentLabel->setAlignment(Qt::AlignHCenter);
    ui_mainVerLayout->addWidget(m_currentLabel);

    m_chartNT = new QChart;
    m_chartNT->layout()->setContentsMargins(2, 2, 2, 2);
    m_chartNT->setAnimationOptions(QChart::AnimationOption(animType));
    m_chartNT->setTheme(QChart::ChartThemeBlueIcy);
    m_chartNT->setAcceptHoverEvents(true);
    m_chartViewNT = new QChartView(m_chartNT, this);
    m_chartViewNT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_mainVerLayout->addWidget(m_chartViewNT);

    QRegExp rgx("([^\\s]+)");
    QValidator *validator = new QRegExpValidator(rgx, this);
    ui_wordLineEdit->setValidator(validator);
}

void WidgetWordFrequency::showSaveContextMenu(const QPoint &pos)
{
    m_chartView = qobject_cast<QChartView *>(QObject::sender());
    QPoint globalPos = m_chartView->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Save as Image"),
                          this,
                          SLOT(chartView_actionSave()),
                          QKeySequence("Ctrl+S"));
    contextMenu.exec(globalPos);
}

void WidgetWordFrequency::chartView_actionSave()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save as Image"),
                                                    "/",
                                                    tr("PNG Files (*.png);;All Files (*.*)"));
    if (!filename.isNull() && !filename.isEmpty()) {
        QPixmap pixmap = m_chartView->grab();
        pixmap.save(filename, "PNG");
    }
}
void WidgetWordFrequency::connectSignalsToSlots()
{
    QObject::connect(ui_wordLineEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(on_wordLineEdit_textChanged(QString)));
    QObject::connect(ui_wordLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(on_wordLineEdit_returnPressed()));
    QObject::connect(ui_visualizeButton, SIGNAL(clicked()),
                     this, SLOT(on_visualizeButton_clicked()));
    QObject::connect(m_chartViewOT, SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(showSaveContextMenu(const QPoint &)));
    QObject::connect(m_chartViewNT, SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(showSaveContextMenu(const QPoint &)));
}

QString WidgetWordFrequency::GetDeclinedForm(double count)
{
    return count > 1.0 ? tr(" times") : count == 0.0 ? tr(" times") : tr(" time");
}

void WidgetWordFrequency::barSetHoveredOT(bool status, int index)
{
    if (status) {
        m_currentLabel->setText(m_categoriesLongOT[index] + ": " %
                                QString::number(m_setOT->at(index)) %
                                GetDeclinedForm(m_setOT->at(index)));
    } else {
        m_currentLabel->clear();
    }
}

void WidgetWordFrequency::barSetHoveredNT(bool status, int index)
{
    if (status) {
        m_currentLabel->setText(m_categoriesLongNT[index] + ": " +
                                QString::number(m_setNT->at(index)) +
                                GetDeclinedForm(m_setNT->at(index)));
    } else {
        m_currentLabel->clear();
    }
}

void WidgetWordFrequency::resizeEvent(QResizeEvent *event)
{
    if (!m_isBeingOpened) {
        ui_visualizeButton->setDisabled(true);
        QTimer::singleShot(2000, this, SLOT(enableVisualizeButton()));
    } else {
        m_isBeingOpened = false;
    }
    event->accept();
}

void WidgetWordFrequency::SearchAndPlot(const QString &word)
{
    ui_visualizeButton->setDisabled(true);
    ui_wordLineEdit->blockSignals(true);
    QHash<uchar, int> hashOT;
    QHash<uchar, int> hashNT;
    QString queryString = "SELECT Book, Scripture FROM Bible"
                          " WHERE (LOWER(Scripture) LIKE '%" % word.toLower() % "%'"
                          " OR UPPER(Scripture) LIKE '%" % word.toUpper() % "%')";
  //  QRegularExpression rgxMarkupNotes("<..>|<RF>.*<Rf>");
    QRegularExpression rgxMarkupNotes("{TN}.*{tn}|{..}|<.{1,2}>");
    QRegularExpression wordRgx("\\b" + word + "\\b", QRegularExpression::CaseInsensitiveOption);
    QSqlQuery query(*m_db);
    int countOT = 0;
    int countNT = 0;
    if (query.exec(queryString)) {
        while (query.next()) {
            QSqlRecord record = query.record();
            uchar bookNumber = uchar(record.value(0).toInt());
            QString verseNoNotes = record.value(1).toString().remove(rgxMarkupNotes);
            // qDebug() << verseNoNotes;
            if (verseNoNotes.contains(wordRgx)) {
                if (bookNumber < 40) {
                    if (hashOT.contains(bookNumber)) {
                        ++hashOT[bookNumber];
                    } else {
                        hashOT.insert(bookNumber, 1);
                    }
                    ++countOT;
                } else {
                    if (hashNT.contains(bookNumber)) {
                        ++hashNT[bookNumber];
                    } else {
                        hashNT.insert(bookNumber, 1);
                    }
                    ++countNT;
                }
            }
        }
    }

    m_setOT = new QBarSet(tr("Old Testament"));
    m_setOT->setBrush(QBrush(Qt::darkGreen));
    QStringList categoriesShortOT;
    int maxOT = 0;
    m_categoriesLongOT.clear();
    for (uchar i = 1; i < 40; ++i) {
        if (hashOT.contains(i)) {
            m_setOT->append(hashOT[i]);
            categoriesShortOT << m_pBookAbbreviations->at(i - 1);
            m_categoriesLongOT << m_pBookNames->at(i - 1);
            if (hashOT[i] > maxOT) {
                maxOT = hashOT[i];
            }
        }
    }

    QObject::connect(m_setOT, SIGNAL(hovered(bool, int)), this, SLOT(barSetHoveredOT(bool, int)));
    m_setNT = new QBarSet(tr("New Testament"));
    QStringList categoriesShortNT;
    int maxNT = 0;
    m_categoriesLongNT.clear();
    for (uchar i = 40; i <= 66; ++i) {
        if (hashNT.contains(i)) {
            m_setNT->append(hashNT[i]);
            categoriesShortNT <<  m_pBookAbbreviations->at(i - 1) ;
            m_categoriesLongNT << m_pBookNames->at(i - 1);
            if (hashNT[i] > maxNT) {
                maxNT = hashNT[i];
            }
        }
    }

    QObject::connect(m_setNT, SIGNAL(hovered(bool, int)), this, SLOT(barSetHoveredNT(bool, int)));
    maxOT = (maxOT + 3) & ~3;
    maxNT = (maxNT + 3) & ~3;
    if (maxOT == 0) {
        maxOT = 4;
    }
    if (maxNT == 0) {
        maxNT = 4;
    }

    m_chartOT->removeAllSeries();
    QBarSeries *seriesOT = new QBarSeries;
    seriesOT->setBarWidth(1);
    seriesOT->append(m_setOT);
    m_chartOT->addSeries(seriesOT);
    m_chartOT->setTitle(tr("Old Testament") + ": " + QString::number(countOT) + GetDeclinedForm(countOT));
    QBarCategoryAxis *axisOT = new QBarCategoryAxis;
    axisOT->setLabelsAngle(-90);
    axisOT->append(categoriesShortOT);
    QValueAxis *axisYOT = new QValueAxis;
    axisYOT->setLabelFormat("%d");
    axisYOT->setMinorTickCount(1);
    m_chartOT->createDefaultAxes();
    m_chartOT->setAxisX(axisOT, seriesOT);
    m_chartOT->setAxisY(axisYOT, seriesOT);

    //m_chartOT->addAxis(axisOT, Qt::AlignBottom);

   // m_chartOT->addAxis(axisYOT, Qt::AlignLeft);
   // m_chartOT->axes(Qt::Horizontal, seriesOT).clear();
  //  m_chartOT->axes(Qt::Horizontal, seriesOT).append(axisOT);
  //  m_chartOT->axes(Qt::Vertical, seriesOT).clear();
  //  m_chartOT->axes(Qt::Vertical, seriesOT).append(axisYOT);

    m_chartOT->legend()->setVisible(false);

    m_chartNT->removeAllSeries();
    QBarSeries *seriesNT = new QBarSeries;
    seriesNT->setBarWidth(1);
    seriesNT->append(m_setNT);
    m_chartNT->addSeries(seriesNT);
    m_chartNT->setTitle(tr("New Testament") + ": " + QString::number(countNT) + GetDeclinedForm(countNT));
    QBarCategoryAxis *axisNT = new QBarCategoryAxis;
    axisNT->setLabelsAngle(-90);
    axisNT->append(categoriesShortNT);
    QValueAxis *axisYNT = new QValueAxis;
    axisYNT->setLabelFormat("%d");
    axisYNT->setMinorTickCount(1);
    axisYNT->setMax(maxNT);
    m_chartNT->createDefaultAxes();
    m_chartNT->setAxisX(axisNT, seriesNT);
    m_chartNT->setAxisY(axisYNT, seriesNT);

   // m_chartOT->axes(Qt::Horizontal, seriesNT).append(axisNT);
   // m_chartOT->axes(Qt::Vertical, seriesNT).append(axisYNT);
  //  m_chartNT->addAxis(axisNT, Qt::AlignBottom);
  //  m_chartNT->addAxis(axisYNT, Qt::AlignLeft);
    m_chartNT->legend()->setVisible(false);

    QTimer::singleShot(2000, this, SLOT(enableButtonAndSignals()));
}

void WidgetWordFrequency::on_visualizeButton_clicked()
{
    QString text = ui_wordLineEdit->text();
    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
    if (regex.exactMatch(text)) {
        text = "<W" % text % ">";
    }
    SearchAndPlot(text);
}
void WidgetWordFrequency::on_wordLineEdit_returnPressed()
{
    if (ui_visualizeButton->isEnabled()) {
        on_visualizeButton_clicked();
    }
}
void WidgetWordFrequency::on_wordLineEdit_textChanged(const QString &arg1)
{
    ui_visualizeButton->setDisabled(arg1.trimmed().isEmpty());
}

void WidgetWordFrequency::enableVisualizeButton()
{
    ui_visualizeButton->setEnabled(true);
}

void WidgetWordFrequency::enableButtonAndSignals()
{
    ui_visualizeButton->setEnabled(true);
    ui_wordLineEdit->blockSignals(false);
}

void WidgetWordFrequency::loadBookAbbreviations()
{
//    m_pBookAbbreviations << tr("Gen")
//                    << tr("Exo")
//                    << tr("Lev")
//                    << tr("Num")
//                    << tr("Deu")
//                    << tr("Jos")
//                    << tr("Jdg")
//                    << tr("Rut")
//                    << tr("1Sa")
//                    << tr("2Sa")
//                    << tr("1Ki")
//                    << tr("2Ki")
//                    << tr("1Ch")
//                    << tr("2Ch")
//                    << tr("Ezr")
//                    << tr("Neh")
//                    << tr("Est")
//                    << tr("Job")
//                    << tr("Psa")
//                    << tr("Pro")
//                    << tr("Ecc")
//                    << tr("Sol")
//                    << tr("Isa")
//                    << tr("Jer")
//                    << tr("Lam")
//                    << tr("Eze")
//                    << tr("Dan")
//                    << tr("Hos")
//                    << tr("Joe")
//                    << tr("Amo")
//                    << tr("Oba")
//                    << tr("Jon")
//                    << tr("Mic")
//                    << tr("Nah")
//                    << tr("Hab")
//                    << tr("Zep")
//                    << tr("Hag")
//                    << tr("Zec")
//                    << tr("Mal")
//                    << tr("Mat")
//                    << tr("Mar")
//                    << tr("Luk")
//                    << tr("Joh")
//                    << tr("Act")
//                    << tr("Rom")
//                    << tr("1Co")
//                    << tr("2Co")
//                    << tr("Gal")
//                    << tr("Eph")
//                    << tr("Phi")
//                    << tr("Col")
//                    << tr("1Th")
//                    << tr("2Th")
//                    << tr("1Ti")
//                    << tr("2Ti")
//                    << tr("Tit")
//                    << tr("Phm")
//                    << tr("Heb")
//                    << tr("Jam")
//                    << tr("1Pe")
//                    << tr("2Pe")
//                    << tr("1Jo")
//                    << tr("2Jo")
//                    << tr("3Jo")
//                    << tr("Jud")
//                    << tr("Rev");
}

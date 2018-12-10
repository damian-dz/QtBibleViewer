#include "PWindowHistogram.h"

PWindowHistogram::PWindowHistogram(const QSqlDatabase &db, QWidget *parent)
    : QWidget(parent),
      m_isBeingOpened(true)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_db = &db;
    loadBookAbbreviations();
    generateMainLayout();
    setUpChartsAndValidator();
    connectSignalsToSlots();
}

PWindowHistogram::~PWindowHistogram()
{

}

void PWindowHistogram::generateMainLayout()
{
    QWidget::resize(1000, 560);
    QWidget::setWindowTitle(tr("Word Frequency"));

    ui_mainVerLayout = new QVBoxLayout;
    QWidget::setLayout(ui_mainVerLayout);

    QHBoxLayout *horLayout = new QHBoxLayout;
    ui_wordLineEdit = new QLineEdit;
    horLayout->addWidget(ui_wordLineEdit);
    QObject::connect(ui_wordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_wordLineEdit_textChanged(QString)));

    ui_visualizeButton = new QPushButton(tr("Visualize"));
    ui_visualizeButton->setDisabled(true);
    horLayout->addWidget(ui_visualizeButton);
    QObject::connect(ui_visualizeButton, SIGNAL(clicked()), this, SLOT(on_visualizeButton_clicked()));

    ui_mainVerLayout->addLayout(horLayout);
}

void PWindowHistogram::showSaveContextMenu(const QPoint &pos)
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
void PWindowHistogram::chartView_actionSave()
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
void PWindowHistogram::connectSignalsToSlots()
{
    QObject::connect(m_chartViewOT, SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(showSaveContextMenu(const QPoint &)));
    QObject::connect(m_chartViewNT, SIGNAL(customContextMenuRequested(const QPoint &)),
                     this, SLOT(showSaveContextMenu(const QPoint &)));
}
void PWindowHistogram::resizeEvent(QResizeEvent *event)
{
    if (!m_isBeingOpened) {
        ui_visualizeButton->setDisabled(true);
        QTimer::singleShot(1400, this, SLOT(enableVisualizeButton()));
    } else {
        m_isBeingOpened = false;
    }
    event->accept();
}

void PWindowHistogram::searchAndPlot(const QString &word)
{
    ui_visualizeButton->setDisabled(true);
    ui_wordLineEdit->blockSignals(true);
    QHash<uchar, int> hashOT;
    QHash<uchar, int> hashNT;
    QString queryString = "SELECT Book, Scripture FROM Bible"
                          " WHERE (LOWER(Scripture) LIKE '%" + word.toLower() + "%'"
                          " OR UPPER(Scripture) LIKE '%" + word.toUpper() + "%')";
    QRegExp rgxMarkupNotes("<..>|<RF>.*<Rf>");
    QSqlQuery query(*m_db);
    if (query.exec(queryString)) {
        while (query.next()) {
            QSqlRecord record = query.record();
            QString verseNoNotes = record.value(1).toString().remove(rgxMarkupNotes);
            uchar bookNumber = uchar(record.value(0).toInt());
            if (verseNoNotes.contains(word, Qt::CaseInsensitive)) {
                if (bookNumber < 40) {
                    if (hashOT.contains(bookNumber)) {
                        hashOT[bookNumber]++;
                    } else {
                        hashOT.insert(bookNumber, 1);
                    }
                } else {
                    if (hashNT.contains(bookNumber)) {
                        hashNT[bookNumber]++;
                    } else {
                        hashNT.insert(bookNumber, 1);
                    }
                }
            }
        }
    }

    QBarSet *setOT = new QBarSet(tr("Old Testament"));
    QStringList categoriesOT;
    int maxOT = 0;
    for (uchar i = 1; i < 40; ++i) {
        if (hashOT.contains(i)) {
            setOT->append(hashOT[i]);
            categoriesOT << m_abbreviations[i - 1];
            if (hashOT[i] > maxOT) {
                maxOT = hashOT[i];
            }
        }
    }

    QBarSet *setNT = new QBarSet(tr("New Testament"));
    QStringList categoriesNT;
    int maxNT = 0;
    for (uchar i = 40; i <= 66; ++i) {
        if (hashNT.contains(i)) {
            setNT->append(hashNT[i]);
            categoriesNT << m_abbreviations[i - 1];
            if (hashNT[i] > maxNT) {
                maxNT = hashNT[i];
            }
        }
    }
    maxOT = (maxOT + 3) & ~3;
    maxNT = (maxNT + 3) & ~3;
    if (maxOT == 0)
        maxOT = 4;
    if (maxNT == 0)
        maxNT = 4;
    setOT->setBrush(QBrush(Qt::darkGreen));
    m_chartOT->removeAllSeries();
    m_chartNT->removeAllSeries();
    QBarSeries *seriesOT = new QBarSeries;
    QBarSeries *seriesNT = new QBarSeries;
    seriesOT->setUseOpenGL(false);
    seriesNT->setUseOpenGL(false);
    seriesOT->setBarWidth(1);
    seriesNT->setBarWidth(1);
    seriesOT->append(setOT);
    seriesNT->append(setNT);
    m_chartOT->addSeries(seriesOT);
    m_chartNT->addSeries(seriesNT);
    QBarCategoryAxis *axisOT = new QBarCategoryAxis;
    QBarCategoryAxis *axisNT = new QBarCategoryAxis;
    axisOT->setLabelsAngle(-90);
    axisNT->setLabelsAngle(-90);
    axisOT->append(categoriesOT);
    axisNT->append(categoriesNT);
    m_chartOT->createDefaultAxes();
    m_chartOT->setAxisX(axisOT, seriesOT);
    m_chartOT->legend()->setVisible(false);
    QValueAxis *axisYOT = new QValueAxis;
    axisYOT->setLabelFormat("%d");
    axisYOT->setMinorTickCount(1);
    axisYOT->setMax(maxOT);
    QValueAxis *axisYNT = new QValueAxis;
    axisYNT->setLabelFormat("%d");
    axisYNT->setMinorTickCount(1);
    axisYNT->setMax(maxNT);
    m_chartOT->setAxisY(axisYOT, seriesOT);
    m_chartNT->createDefaultAxes();
    m_chartNT->setAxisX(axisNT, seriesNT);
    m_chartNT->setAxisY(axisYNT, seriesNT);
    m_chartNT->legend()->setVisible(false);
    QTimer::singleShot(1400, this, SLOT(enableButtonAndSignals()));
}

void PWindowHistogram::setUpChartsAndValidator()
{
    m_chartOT = new QChart;
    m_chartOT->setAnimationOptions(QChart::SeriesAnimations);
    m_chartViewOT = new QChartView(m_chartOT, this);
    m_chartViewOT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_mainVerLayout->addWidget(m_chartViewOT);

    m_chartNT = new QChart;
    m_chartNT->setAnimationOptions(QChart::SeriesAnimations);
    m_chartViewNT = new QChartView(m_chartNT, this);
    m_chartViewNT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_mainVerLayout->addWidget(m_chartViewNT);

    QRegExp rgx("([^\\s]+)");
    QValidator *validator = new QRegExpValidator(rgx, this);
    ui_wordLineEdit->setValidator(validator);
}

void PWindowHistogram::on_visualizeButton_clicked()
{
    QString text = ui_wordLineEdit->text();
    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
    if (regex.exactMatch(text)) {
        text = "<W" + text + ">";
    }
    searchAndPlot(text);
}
void PWindowHistogram::on_wordLineEdit_returnPressed()
{
    if (ui_visualizeButton->isEnabled()) {
        on_visualizeButton_clicked();
    }
}
void PWindowHistogram::on_wordLineEdit_textChanged(const QString &arg1)
{
    ui_visualizeButton->setDisabled(arg1.trimmed().isEmpty());
}

void PWindowHistogram::enableVisualizeButton()
{
    ui_visualizeButton->setEnabled(true);
}

void PWindowHistogram::enableButtonAndSignals()
{
    ui_visualizeButton->setEnabled(true);
    ui_wordLineEdit->blockSignals(false);
}

void PWindowHistogram::loadBookAbbreviations()
{
    m_abbreviations << tr("Gen")
                    << tr("Exo")
                    << tr("Lev")
                    << tr("Num")
                    << tr("Deu")
                    << tr("Jos")
                    << tr("Jdg")
                    << tr("Rut")
                    << tr("1Sa")
                    << tr("2Sa")
                    << tr("1Ki")
                    << tr("2Ki")
                    << tr("1Ch")
                    << tr("2Ch")
                    << tr("Ezr")
                    << tr("Neh")
                    << tr("Est")
                    << tr("Job")
                    << tr("Psa")
                    << tr("Pro")
                    << tr("Ecc")
                    << tr("Sol")
                    << tr("Isa")
                    << tr("Jer")
                    << tr("Lam")
                    << tr("Eze")
                    << tr("Dan")
                    << tr("Hos")
                    << tr("Joe")
                    << tr("Amo")
                    << tr("Oba")
                    << tr("Jon")
                    << tr("Mic")
                    << tr("Nah")
                    << tr("Hab")
                    << tr("Zep")
                    << tr("Hag")
                    << tr("Zec")
                    << tr("Mal")
                    << tr("Mat")
                    << tr("Mar")
                    << tr("Luk")
                    << tr("Joh")
                    << tr("Act")
                    << tr("Rom")
                    << tr("1Co")
                    << tr("2Co")
                    << tr("Gal")
                    << tr("Eph")
                    << tr("Phi")
                    << tr("Col")
                    << tr("1Th")
                    << tr("2Th")
                    << tr("1Ti")
                    << tr("2Ti")
                    << tr("Tit")
                    << tr("Phm")
                    << tr("Heb")
                    << tr("Jam")
                    << tr("1Pe")
                    << tr("2Pe")
                    << tr("1Jo")
                    << tr("2Jo")
                    << tr("3Jo")
                    << tr("Jud")
                    << tr("Rev");
}

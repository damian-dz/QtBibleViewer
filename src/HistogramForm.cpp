#include "hdr/HistogramForm.h"
#include "ui_HistogramForm.h"

#include "hdr/AuxiliaryMethods.h"

#include <QFileDialog>
#include <QMenu>

HistogramForm::HistogramForm(const QSqlDatabase &db, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HistogramForm),
      opening(true)
{
    ui->setupUi(this);
    this->db = &db;
    loadBookAbbreviations();
    setUpChartsAndValidator();
    connectSignalsToSlots();
}

HistogramForm::~HistogramForm()
{
    delete ui;
}

void HistogramForm::showSaveContextMenu(const QPoint &pos)
{
    chartView = qobject_cast<QChartView *>(QObject::sender());
    QPoint globalPos = chartView->mapToGlobal(pos);
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Save as Image"),
                          this,
                          SLOT(chartView_actionSave()),
                          QKeySequence("Ctrl+S"));
    contextMenu.exec(globalPos);
}

void HistogramForm::chartView_actionSave()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save as Image"),
                                                    "/",
                                                    tr("PNG Files (*.png);;All Files (*.*)"));
    if (filename.isNull() || filename.isEmpty())
        return;
    QPixmap pixmap = chartView->grab();
    pixmap.save(filename, "PNG");
}

void HistogramForm::connectSignalsToSlots()
{
    connect(chartViewOT, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showSaveContextMenu(const QPoint &)));
    connect(chartViewNT, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showSaveContextMenu(const QPoint &)));
}

void HistogramForm::resizeEvent(QResizeEvent *event)
{
    if (!opening) {
        ui->visualizeButton->setDisabled(true);
        QTimer::singleShot(1400, this, SLOT(enableVisualizeButton()));
    } else
        opening = false;
    event->accept();
}

void HistogramForm::searchAndPlot(const QString &word)
{
    ui->visualizeButton->setDisabled(true);
    ui->wordLineEdit->blockSignals(true);
    QHash<uchar, int> hashOT;
    QHash<uchar, int> hashNT;
    QString queryString = AuxiliaryMethods::singleWordQueryString(word, "Book, Scripture");
    QRegExp rgxMarkupNotes("<..>|<RF>.*<Rf>");
    QSqlQuery query(*db);
    query.exec(queryString);
    while (query.next()) {
        QSqlRecord record = query.record();
        QString verseNoNotes = record.value(1).toString().remove(rgxMarkupNotes);
        uchar bookNumber = record.value(0).toInt();
        if (verseNoNotes.contains(word, Qt::CaseInsensitive)) {
            if (bookNumber < 40) {
                if (hashOT.contains(bookNumber))
                    hashOT[bookNumber]++;
                else
                    hashOT.insert(bookNumber, 1);
            } else {
                if (hashNT.contains(bookNumber))
                    hashNT[bookNumber]++;
                else
                    hashNT.insert(bookNumber, 1);
            }
        }
    }
    QBarSet *setOT = new QBarSet("Old Testament");
    QBarSet *setNT = new QBarSet("New Testament");
    QStringList categoriesOT;
    QStringList categoriesNT;
    int maxOT = 0;
    for (int i = 1; i < 40; ++i) {
        if (hashOT.contains(i)) {
            setOT->append(hashOT[i]);
            categoriesOT << abbreviations[i - 1];
            if (hashOT[i] > maxOT)
                maxOT = hashOT[i];
        }
    }
    int maxNT = 0;
    for (int i = 40; i <= 66; ++i) {
        if (hashNT.contains(i)) {
            setNT->append(hashNT[i]);
            categoriesNT << abbreviations[i - 1];
            if (hashNT[i] > maxNT)
                maxNT = hashNT[i];
        }
    }
    maxOT = (maxOT + 3) & ~3;
    maxNT = (maxNT + 3) & ~3;
    if (maxOT == 0)
        maxOT = 4;
    if (maxNT == 0)
        maxNT = 4;
    setOT->setBrush(QBrush(Qt::darkGreen));
    chartOT->removeAllSeries();
    chartNT->removeAllSeries();
    QBarSeries *seriesOT = new QBarSeries;
    QBarSeries *seriesNT = new QBarSeries;
    seriesOT->setUseOpenGL(true);
    seriesNT->setUseOpenGL(true);
    seriesOT->setBarWidth(1);
    seriesNT->setBarWidth(1);
    seriesOT->append(setOT);
    seriesNT->append(setNT);
    chartOT->addSeries(seriesOT);
    chartNT->addSeries(seriesNT);
    QBarCategoryAxis *axisOT = new QBarCategoryAxis;
    QBarCategoryAxis *axisNT = new QBarCategoryAxis;
    axisOT->setLabelsAngle(-90);
    axisNT->setLabelsAngle(-90);
    axisOT->append(categoriesOT);
    axisNT->append(categoriesNT);
    chartOT->createDefaultAxes();
    chartOT->setAxisX(axisOT, seriesOT);
    chartOT->legend()->setVisible(false);
    QValueAxis *axisYOT = new QValueAxis;
    axisYOT->setLabelFormat("%d");
    axisYOT->setMinorTickCount(1);
    axisYOT->setMax(maxOT);
    QValueAxis *axisYNT = new QValueAxis;
    axisYNT->setLabelFormat("%d");
    axisYNT->setMinorTickCount(1);
    axisYNT->setMax(maxNT);
    chartOT->setAxisY(axisYOT, seriesOT);
    chartNT->createDefaultAxes();
    chartNT->setAxisX(axisNT, seriesNT);
    chartNT->setAxisY(axisYNT, seriesNT);
    chartNT->legend()->setVisible(false);
    QTimer::singleShot(1400, this, SLOT(enableButtonAndSignals()));
}

void HistogramForm::setUpChartsAndValidator()
{
    chartOT = new QChart;
    chartOT->setAnimationOptions(QChart::SeriesAnimations);
    chartViewOT = new QChartView(chartOT, this);
    chartViewOT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->verticalLayout->addWidget(chartViewOT);
    chartNT = new QChart;
    chartNT->setAnimationOptions(QChart::SeriesAnimations);    
    chartViewNT = new QChartView(chartNT, this);    
    chartViewNT->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->verticalLayout->addWidget(chartViewNT);
    QRegExp regex("^\s*([a-zA-ZąĄćĆęĘłŁńŃóÓśŚźŹżŻ0-9]+\s*)$");
    QValidator *validator = new QRegExpValidator(regex, this);
    ui->wordLineEdit->setValidator(validator);
}

void HistogramForm::on_visualizeButton_clicked()
{
    QString text = ui->wordLineEdit->text();
    QRegExp regex("^[HG][0-9]{1,4}$", Qt::CaseInsensitive);
    if (regex.exactMatch(text))
        text = "<W" + text + ">";
    searchAndPlot(text);
}

void HistogramForm::on_wordLineEdit_returnPressed()
{
    if (ui->visualizeButton->isEnabled())
        on_visualizeButton_clicked();
}

void HistogramForm::on_wordLineEdit_textChanged(const QString &arg1)
{   
    ui->visualizeButton->setDisabled(arg1.trimmed().isEmpty());
}

void HistogramForm::enableVisualizeButton()
{
    ui->visualizeButton->setEnabled(true);
}

void HistogramForm::enableButtonAndSignals()
{
    ui->visualizeButton->setEnabled(true);
    ui->wordLineEdit->blockSignals(false);
}

void HistogramForm::loadBookAbbreviations()
{
    abbreviations << tr("Gen")
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

#ifndef HISTOGRAMFORM_H
#define HISTOGRAMFORM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>
#include <QtSql>
#include <QVBoxLayout>

using namespace QtCharts;

namespace Ui {
class HistogramForm;
}

class HistogramForm : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramForm(const QSqlDatabase &db, QWidget *parent = 0);
    ~HistogramForm();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void enableButtonAndSignals();
    void enableVisualizeButton();
    void on_visualizeButton_clicked();
    void on_wordLineEdit_returnPressed();
    void on_wordLineEdit_textChanged(const QString &arg1);
    void showSaveContextMenu(const QPoint &pos);
    void chartView_actionSave();
    void connectSignalsToSlots();

private:
    Ui::HistogramForm *ui;
    bool opening;
    QChart *chartOT;
    QChart *chartNT;
    QChartView *chartViewOT;
    QChartView *chartViewNT;
    QChartView *chartView;
    const QSqlDatabase *db;
    QStringList abbreviations;
    void loadBookAbbreviations();
    void searchAndPlot(const QString &word);
    void setUpChartsAndValidator();
};

#endif // HISTOGRAMFORM_H

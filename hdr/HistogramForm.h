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
    explicit HistogramForm(const QSqlDatabase &db, const QString &lang, QWidget *parent = 0);
    ~HistogramForm();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void enableButtonAndSignals();
    void enableVisualizeButton();
    void on_visualizeButton_clicked();
    void on_wordLineEdit_returnPressed();
    void on_wordLineEdit_textChanged(const QString &arg1);
   //void on_saveButtonOT_clicked();

private:
    Ui::HistogramForm *ui;
    bool opening;
    QChart *chartOT;
    QChart *chartNT;
    QChartView *chartViewOT;
    QChartView *chartViewNT;
//    QHBoxLayout *horizontalLayoutOT;
//    QHBoxLayout *horizontalLayoutNT;
//    QPushButton *saveButtonOT;
//    QPushButton *saveButtonNT;
    QSqlDatabase db;
    QStringList abbreviations;
    void changeLanguage(const QString &language);
    void loadBookAbbreviations(const QString &language);
    void searchAndPlot(const QString &word);
    void setUpChartsAndValidator();
};

#endif // HISTOGRAMFORM_H

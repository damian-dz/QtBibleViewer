#ifndef PWINDOWHISTOGRAM_H
#define PWINDOWHISTOGRAM_H

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

class PWindowHistogram : public QWidget
{
    Q_OBJECT

public:
    explicit PWindowHistogram(const QSqlDatabase &db, QWidget *parent = 0);
    ~PWindowHistogram();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void generateMainLayout();
    void enableButtonAndSignals();
    void enableVisualizeButton();
    void on_visualizeButton_clicked();
    void on_wordLineEdit_returnPressed();
    void on_wordLineEdit_textChanged(const QString &arg1);
    void showSaveContextMenu(const QPoint &pos);
    void chartView_actionSave();
    void connectSignalsToSlots();


private:
    QVBoxLayout *ui_mainVerLayout;
    QLineEdit *ui_wordLineEdit;
    QPushButton *ui_visualizeButton;
    bool isBeingOpened;
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

#endif // PWINDOWHISTOGRAM_H

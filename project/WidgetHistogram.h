#ifndef PWINDOWHISTOGRAM_H
#define PWINDOWHISTOGRAM_H

#include "precomp.h"

using namespace QtCharts;

class WidgetHistogram : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetHistogram(const QSqlDatabase &m_db, const QStringList &fullNames, int animType, QWidget *parent = nullptr);
    ~WidgetHistogram();

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
    void barSetHoveredOT(bool status, int index);
    void barSetHoveredNT(bool status, int index);

private:
    QVBoxLayout *ui_mainVerLayout;
    QLineEdit *ui_wordLineEdit;
    QPushButton *ui_visualizeButton;
    bool m_isBeingOpened;
    QChart *m_chartOT;
    QChart *m_chartNT;
    QBarSet *m_setOT;
    QBarSet *m_setNT;
    QChartView *m_chartView;
    QChartView *m_chartViewOT;
    QLabel *m_currentLabel;
    QChartView *m_chartViewNT;
    const QSqlDatabase *m_db;
    const QStringList *m_fullNames;
    QStringList m_categoriesLongOT;
    QStringList m_categoriesLongNT;
    QStringList m_abbreviations;

    void generateMainLayout();
    void loadBookAbbreviations();
    void searchAndPlot(const QString &word);
    void setUpChartsAndValidator(int animType);
    QString getDeclinedForm(double count);
};

#endif // PWINDOWHISTOGRAM_H

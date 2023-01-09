#ifndef WIDGETWORDFREQUENCY_H
#define WIDGETWORDFREQUENCY_H

#include "precomp.h"

//using namespace QtCharts;

class WidgetWordFrequency : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetWordFrequency(const QSqlDatabase &m_db, const QStringList &bookNames,
                                 const QStringList &bookAbbraviations, int animType,
                                 QWidget *parent = nullptr);
    ~WidgetWordFrequency();

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
    const QStringList *m_pBookAbbreviations;
    const QStringList *m_pBookNames;
    QStringList m_categoriesLongOT;
    QStringList m_categoriesLongNT;

    void generateMainLayout();
    void loadBookAbbreviations();
    void SearchAndPlot(const QString &word);
    void setUpChartsAndValidator(int animType);
    QString GetDeclinedForm(double count);
};

#endif // WIDGETWORDFREQUENCY_H

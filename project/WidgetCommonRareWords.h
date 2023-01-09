#ifndef WIDGETCOMMONRAREWORDS_H
#define WIDGETCOMMONRAREWORDS_H

#include "precomp.h"

//using namespace QtCharts;

class WidgetCommonRareWords : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetCommonRareWords(const QSqlDatabase &db, int animType, QWidget *parent = nullptr);
    ~WidgetCommonRareWords();

protected:
    void resizeEvent(QResizeEvent *evt);

private slots:
    void on_visualizeButton_clicked();
    void enableVisualizeButton();

private:
    const QSqlDatabase *m_db;
    QVBoxLayout *ui_mainVerLayout;
    QSpinBox *ui_SpinBox_LetterCount;
    QRadioButton *ui_RadioButton_Common;
    QPushButton *ui_PushButton_Visualize;
    QChart *ui_chart;
    bool m_isBeingOpened;

    void GenerateMainLayout(int animType);
    void plotResults(const QHash<QString, int> &uniqueWords);
};

#endif // WIDGETCOMMONRAREWORDS_H

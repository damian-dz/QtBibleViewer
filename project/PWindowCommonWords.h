#ifndef PWINDOWCOMMONWORDS_H
#define PWINDOWCOMMONWORDS_H

#include "precomp.h"

using namespace QtCharts;

class PWindowCommonWords : public QWidget
{
    Q_OBJECT
public:
    explicit PWindowCommonWords(const QSqlDatabase &db, QWidget *parent = nullptr);
    ~PWindowCommonWords();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_visualizeButton_clicked();
    void enableVisualizeButton();

private:
    const QSqlDatabase *m_db;
    QVBoxLayout *ui_mainVerLayout;
    QSpinBox *ui_SpinBox_LetterCount;
    QPushButton *ui_PushButton_Visualize;
    QChart *ui_chart;
    bool m_isBeingOpened;

    void generateMainLayout();
    void plotResults(const QList<QPair<int, QString>> &pairs);
};

#endif // PWINDOWCOMMONWORDS_H

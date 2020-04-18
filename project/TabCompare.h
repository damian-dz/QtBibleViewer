#ifndef TABCOMPARE_H
#define TABCOMPARE_H

#include "AbstractTab.h"

class TabCompare : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabCompare(QWidget *parent = nullptr);

    virtual void connectSignals() override;
    virtual void addControls() override;
    virtual void setUiTexts() override;

signals:

public slots:
};

#endif // TABCOMPARE_H

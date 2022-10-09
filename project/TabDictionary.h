#ifndef TABDICTIONARY_H
#define TABDICTIONARY_H

#include "AbstractTab.h"

class TabDictionary : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabDictionary(QWidget *parent = nullptr);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;

signals:

public slots:
};

#endif // TABDICTIONARY_H

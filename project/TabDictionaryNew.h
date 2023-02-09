#ifndef TABDICTIONARYNEW_H
#define TABDICTIONARYNEW_H

#include "AbstractTab.h"

class TabDictionaryNew : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabDictionaryNew(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;
};

#endif // TABDICTIONARYNEW_H

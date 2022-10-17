#ifndef ABSTRACTTAB_H
#define ABSTRACTTAB_H

#include "precomp.h"

#include "AppConfig.h"
#include "DatabaseService.h"

class AbstractTab : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractTab(QWidget *parent = nullptr);
    explicit AbstractTab(AppConfig &config, qbv::DatabaseService &databaseService, QWidget *parent = nullptr);

    virtual void AddControls() = 0;
    virtual void ConnectSignals() = 0;
    virtual void SetUiTexts() = 0;
    virtual void Initialize();

    bool IsInitialized() const;
    QString LastStatusMsg() const;
    void SetLastStatusMsg(const QString &msg);

protected:
    AppConfig *m_pConfig;
    qbv::DatabaseService *m_pDatabaseService;

    bool m_isInitialized;
    QString m_lastStatusMsg;
};

#endif // ABSTRACTTAB_H

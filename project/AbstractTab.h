#ifndef ABSTRACTTAB_H
#define ABSTRACTTAB_H

#include "precomp.h"

class AbstractTab : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractTab(QWidget *parent = nullptr);

    virtual void AddControls() = 0;
    virtual void ConnectSignals() = 0;
    virtual void SetUiTexts() = 0;

    QString GetLastStatusMsg() const;
    bool IsInitialized() const;
    virtual void Initialize();
    void SetLastStatusMsg(const QString &msg);

signals:

public slots:

protected:
    bool m_isInitialized;
    QString m_lastStatusMsg;
};

#endif // ABSTRACTTAB_H

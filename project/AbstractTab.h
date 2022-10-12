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
    virtual void Initialize();

    bool IsInitialized() const;
    QString LastStatusMsg() const;
    void SetLastStatusMsg(const QString &msg);

signals:

public slots:

protected:
    bool m_isInitialized;
    QString m_lastStatusMsg;
};

#endif // ABSTRACTTAB_H

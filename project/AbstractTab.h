#ifndef ABSTRACTTAB_H
#define ABSTRACTTAB_H

#include "precomp.h"

class AbstractTab : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractTab(QWidget *parent = nullptr);

    virtual void addControls() = 0;
    virtual void connectSignals() = 0;
    virtual void setUiTexts() = 0;

    bool isInitialized() const;
    void initialize();

signals:

public slots:

protected:
    bool m_isInitialized;
};

#endif // ABSTRACTTAB_H

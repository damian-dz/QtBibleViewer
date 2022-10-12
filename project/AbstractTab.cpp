#include "AbstractTab.h"

/*!
 * \brief Provides the abstract interface for main tabs in the program.
 * \param parent specifies the parent widget for this object
 */
AbstractTab::AbstractTab(QWidget *parent) :
    QWidget(parent),
    m_isInitialized(false)
{
}

void AbstractTab::Initialize()
{
    AddControls();
    ConnectSignals();
    SetUiTexts();
    m_isInitialized = true;
}

bool AbstractTab::IsInitialized() const
{
    return m_isInitialized;
}

QString AbstractTab::LastStatusMsg() const
{
    return m_lastStatusMsg;
}

void AbstractTab::SetLastStatusMsg(const QString &msg)
{
    m_lastStatusMsg = msg;
}

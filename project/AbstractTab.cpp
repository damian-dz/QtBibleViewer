#include "AbstractTab.h"

AbstractTab::AbstractTab(QWidget *parent) :
    QWidget(parent),
    m_isInitialized(false)
{
}

void AbstractTab::initialize()
{
    addControls();
    connectSignals();
    setUiTexts();
    m_isInitialized = !m_isInitialized;
}

bool AbstractTab::isInitialized() const
{
    return m_isInitialized;
}

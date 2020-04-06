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

void AbstractTab::initialize()
{
    addControls();
    connectSignals();
    setUiTexts();
    m_isInitialized = true;
}

bool AbstractTab::isInitialized() const
{
    return m_isInitialized;
}

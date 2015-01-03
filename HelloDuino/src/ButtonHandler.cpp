#include "ButtonHandler.h"

CButtonHandler::CButtonHandler()
    : m_HoldThreshold(500)
    , m_PrevButtonPressed(false)
    , m_Counter(0)
    , m_ButtonState(Neutral)
{
}

void CButtonHandler::Update(bool ButtonPressed)
{
    // set to 'default' state
    m_ButtonState = Neutral;

    if(m_PrevButtonPressed != ButtonPressed)
    {
        // change in state
        m_PrevButtonPressed = ButtonPressed;
        if(!m_PrevButtonPressed)
        { // pressed -> released
            if(m_Counter<m_HoldThreshold)
            {
                // 'short' press
                m_ButtonState = Clicked;
            }
            m_Counter = 0;
        }
    }
    else if(m_PrevButtonPressed)
    {
        // continuously pressed
        if(m_Counter<m_HoldThreshold)
        {
            // not (yet)a a'long' press
            ++m_Counter;
        }
        else if(m_Counter == m_HoldThreshold)
        {
            m_ButtonState = Hold;
            ++m_Counter;//prevent re-triggering hold again
        }
    }
}

bool CButtonHandler::ButtonClicked() const
{
    return m_ButtonState == Clicked;
}

bool CButtonHandler::ButtonHold() const
{
    return m_ButtonState == Hold;
}

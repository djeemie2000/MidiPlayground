#include "Controller.h"

CController::CController()
    : m_MidiSerial()
{
    for(int Ctrl = 0; Ctrl<NumTriggers; ++Ctrl)
    {
        m_PrevInputValue[Ctrl] = 64;
        m_ControlChange[Ctrl].s_Controller = 70+Ctrl;//??
        m_ControlChange[Ctrl].s_Value = m_PrevInputValue[Ctrl];
    }
}

void CController::Begin()
{
    m_MidiSerial.Begin(115200);//31250);
}

void CController::Update(const bool* InputValue)
{
    for(int Ctrl = 0; Ctrl<NumTriggers; ++Ctrl)
    {
        if(m_PrevInputValue[Ctrl] != InputValue[Ctrl])
        {                
            m_ControlChange[Ctrl].s_Value = InputValue?127:0;

            m_MidiSerial.ControlChange(0, m_ControlChange[Ctrl].s_Controller, m_ControlChange[Ctrl].s_Value);

            m_PrevInputValue[Ctrl] = InputValue[Ctrl];
        }
    }
}

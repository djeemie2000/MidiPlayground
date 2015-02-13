#include "Controller.h"

CController::CController()
    : m_EditMode(ControllerMode)
    , m_MidiSerial()
    , m_Display()
{
    for(int Ctrl = 0; Ctrl<NumControllers; ++Ctrl)
    {
        m_PrevInputValue[Ctrl] = 64;
        m_ControlChange[Ctrl].s_Controller = 70+Ctrl;//??
        m_ControlChange[Ctrl].s_Value = m_PrevInputValue[Ctrl];
    }
}

void CController::Begin()
{
    m_MidiSerial.Begin(31250);
    m_Display.Begin();
    // display initial values:
    m_Display.Update(m_EditMode, m_ControlChange, NumControllers);
}

namespace
{

bool HasChanged(int Value, int NewValue, int Threshold)
{
    return NewValue<Value-Threshold || Value+Threshold<NewValue;
}

}

void CController::Update(const int* InputValue, int NumValues, bool AssignButtonPressed)
{
    bool UpdateDisplay = false;

    EEditMode EditMode = AssignButtonPressed ? AssignControllerMode : ControllerMode;
    if(EditMode != m_EditMode)
    {
      UpdateDisplay = true;
      m_EditMode = EditMode;
    }

    for(int Ctrl = 0; Ctrl<NumValues; ++Ctrl)
    {
        if(HasChanged(m_PrevInputValue[Ctrl], InputValue[Ctrl], 4))
        {
            int NewValue = InputValue[Ctrl]/8; // [0,1024[ to [0, 127]
            if(AssignButtonPressed)
            {
                m_ControlChange[Ctrl].s_Controller = NewValue;
            }
            else
            {
                m_ControlChange[Ctrl].s_Value = NewValue;

                m_MidiSerial.ControlChange(0, m_ControlChange[Ctrl].s_Controller, m_ControlChange[Ctrl].s_Value);
            }
            UpdateDisplay = true;
            m_PrevInputValue[Ctrl] = InputValue[Ctrl];
        }
    }

    if(UpdateDisplay)
    {
        m_Display.Update(m_EditMode, m_ControlChange, NumControllers);
    }
}

#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include "ControlChange.h"
#include <MidiSerial.h>

/*
 * Controller for a number of midi triggers (midi cc 0 or 127)
 * */
class CController
{
public:
    static const int NumTriggers = 12;

    CController();
    
    void Begin();

    void Update (const bool* InputValue);

private:
    bool            m_PrevInputValue[NumTriggers];
    SControlChange  m_ControlChange[NumTriggers];

    CMidiSerial m_MidiSerial;
};

#endif // STEPCONTROLLER_H

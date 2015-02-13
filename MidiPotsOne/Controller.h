#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include "Display.h"
#include "EditMode.h"
#include "ControlChange.h"
#include <MidiSerial.h>

/*
 * Controller for a number of midi controllers (each having an arameter and value)
 * */
class CController
{
public:
    static const int NumControllers = 4;

    CController();
    
    void Begin();

    void Update (const int* InputValue, int NumValues, bool AssignButtonPressed);
        

private:
    EEditMode       m_EditMode;
    int             m_PrevInputValue[NumControllers];
    SControlChange  m_ControlChange[NumControllers];

    CMidiSerial m_MidiSerial;
    CDisplay m_Display;
};

#endif // STEPCONTROLLER_H

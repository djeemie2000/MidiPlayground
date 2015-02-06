#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include "Step.h"
#include "Periodic.h"
#include "MidiNotePlayer.h"
#include "Display.h"
#include "EditMode.h"
#include "Stepping.h"
#include <MidiSerial.h>

class COneStepController
{
public:
    static const int NumSteps = 8;
    static const int EditAll = NumSteps;

    // TODO LiveMode / StepMode

    COneStepController();
    
    void Begin();

    int Update(int Rotary1Position,
               int Rotary2Position,
               int Rotary3Position,
               bool EditModeButtonPressed,
               bool SelectStepButtonPressed,
               unsigned long TimeStampMilliSeconds);
        
    bool GetStepState(int Step) const;
    bool GetStepEdit(int Step) const;
    bool GetStepInInterval(int Step) const;
    bool GetStepActive(int Step) const;


    void OnClockTick();//private??

private:
    bool m_EditModeButtonPressed;
    int m_Rotary1Position;
    int m_Rotary2Position;
    int m_Rotary3Position;
    bool m_SelectStepButtonPressed;

    EEditMode m_EditMode;
    CPeriodic m_Period;
    
    int m_PlayStep;
    int m_PlaySubStep;
    int m_EditStep;
    SStep m_Step[NumSteps];
    CStepping m_Stepping;

    int m_NumUpdates;

    CMidiNotePlayer m_MidiNotePlayer[NumSteps];
    CMidiNoteDisplay m_Display;
};

#endif // STEPCONTROLLER_H

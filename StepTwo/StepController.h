#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include "Step.h"
#include "Periodic.h"
#include "MidiNotePlayer.h"
#include "Display.h"
#include <MidiSerial.h>

class COneStepController
{
public:
    static const int NumSteps = 8;
    static const int EditAll = NumSteps;

//    enum EEditMode
//    {
//        Octave = 0,
//        Note,
//        Velocity,
//        Duration,
//        Active,
//        Tempo,
//        EditModeSize
//    };
    // TODO LiveMode / StepMode

    COneStepController();
    
    void Begin();

    int Update(int Rotary1Position,
               int Rotary2Position,
               int Rotary3Position,
               bool EditModeButtonPressed,
               bool SelectStepButtonPressed,
               unsigned long TimeStampMilliSeconds);
        
    bool GetState() const;

    bool GetStepState(int Step) const;


    void OnClockTick();//private??

private:
    bool m_EditModeButtonPressed;
    int m_Rotary1Position;
    int m_Rotary2Position;
    int m_Rotary3Position;
    bool m_SelectStepButtonPressed;

    CPeriodic m_Period;
    
    int m_PlayStep;
    int m_EditStep;
    SStep m_Step[NumSteps];
    CMidiNotePlayer m_MidiNotePlayer[NumSteps];
    CMidiNoteDisplay m_MidiNoteDisplay;
};


#endif // STEPCONTROLLER_H

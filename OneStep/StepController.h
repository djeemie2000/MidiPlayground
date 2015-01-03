#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include "Step.h"
#include "Periodic.h"
#include "MidiNotePlayer.h"
#include "MidiNoteDisplay.h"
#include <MidiSerial.h>

class COneStepController
{
public:
    static const int NumSteps = 8;
    static const int EditAll = NumSteps;

    enum EEditMode
    {
        Octave = 0,
        Note,
        Velocity,
        Duration,
        Active,
        Tempo,
        EditModeSize
    };

    COneStepController();
    
    void Begin();

    int Update(int RotaryPosition, bool EditModeButtonPressed, bool SelectStepButtonPressed, unsigned long TimeStampMilliSeconds);

    EEditMode GetEditMode() const;
        
    bool GetState() const;

private:
    bool m_EditModeButtonPressed;
    int m_RotaryPosition;
    bool m_SelectStepButtonPressed;

    EEditMode m_EditMode;

    CPeriodic m_Period;
    
    int m_PlayStep;
    int m_EditStep;
    SStep m_Step[NumSteps];
    CMidiNotePlayer m_MidiNotePlayer;
    CMidiNoteDisplay m_MidiNoteDisplay;
};


#endif // STEPCONTROLLER_H

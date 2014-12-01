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
    enum EEditMode
    {
        Octave = 0,
        Note,
        Velocity,
        Duration,
        Tempo,
        EditModeSize
    };

    COneStepController()
        : m_ButtonPressed(false)
        , m_RotaryPosition(0)//?? initial value??
        , m_EditMode(Note)
        , m_Period()
        , m_Step()
        , m_MidiNotePlayer()
        , m_MidiNoteDisplay()
    {}
    
    void Begin()
    {
      m_MidiNotePlayer.Begin();
      m_MidiNoteDisplay.Begin();
      // display initial values:
      m_MidiNoteDisplay.Update(m_Step.s_MidiNote, m_Step.s_Velocity, m_Step.s_Duration);
    }

    int Update(int RotaryPosition, bool ButtonPressed, unsigned long TimeStampMilliSeconds)
    {
        // button off -> on => toggle edit mode (octave, note, velocity, Duration, Tempo)
        if(m_ButtonPressed != ButtonPressed)
        {
            if(ButtonPressed)
            {
                m_EditMode = static_cast<EEditMode>( (m_EditMode+1)%EditModeSize );
            }
            m_ButtonPressed = ButtonPressed;
        }

        // RotaryPosition changed => apply change to current edit mode
        if(m_RotaryPosition != RotaryPosition)
        {
            int RotaryPositionChange = RotaryPosition - m_RotaryPosition;
            switch(m_EditMode)
            {
            case Octave:
                m_Step.UpdateOctave(RotaryPositionChange);
                break;
            case Note:
                m_Step.UpdateNote(RotaryPositionChange);
                break;
            case Velocity:
                m_Step.UpdateVelocity(RotaryPositionChange);
                break;
            case Duration:
                m_Step.UpdateDuration(RotaryPositionChange);
                break;
            case Tempo:
                m_Period.UpdateTempo(RotaryPositionChange);
                // => adjust period msec, durationperiod msec
                break;
            case EditModeSize:
            default:
                break;
            }

            m_RotaryPosition = RotaryPosition;

            // update display
            m_MidiNoteDisplay.Update(m_Step.s_MidiNote, m_Step.s_Velocity, m_Step.s_Duration);
        }

        // timestamp => note on / note off required?
        int Action = m_Period.Update(TimeStampMilliSeconds, m_Step.s_Duration);
        // action => on / off / do nothing
        if(Action == CPeriodic::NoteOnAction)
        {
            m_MidiNotePlayer.NoteOn(m_Step.s_MidiNote, m_Step.s_Velocity);
        }
        else if(Action == CPeriodic::NoteOffAction)
        {
            m_MidiNotePlayer.NoteOff();
        }
        return Action;
    }

    EEditMode GetEditMode() const
    {
        return m_EditMode;
    }

    // TODO getters for display: oct, not, vel, dur -> char
    char GetCurrentNote() const
    {
        return MidiNoteToNote(m_Step.s_MidiNote);
    }

    int GetCurrentOctave() const
    {
        return MidiNoteToOctave(m_Step.s_MidiNote);
    }

    int GetCurrentVelocity() const
    {
        return m_Step.s_Velocity;
    }

    int GetCurrentDuration() const
    {
        return m_Step.s_Duration;
    }
    
    int GetCurrentTempoBpm() const
    {
      return m_Period.GetTempoBpm();
    }
        
    bool GetState() const
    {
      return m_Period.GetState();
    }

private:
    bool m_ButtonPressed;
    int m_RotaryPosition;

    EEditMode m_EditMode;

    CPeriodic m_Period;
    SStep m_Step;
    CMidiNotePlayer m_MidiNotePlayer;
    CMidiNoteDisplay m_MidiNoteDisplay;
};


#endif // STEPCONTROLLER_H

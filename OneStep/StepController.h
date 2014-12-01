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
        : m_EditModeButtonPressed(false)
        , m_RotaryPosition(0)//?? initial value??
        , m_SelectStepButtonPressed(false)
        , m_EditMode(Note)
        , m_Period()
        , m_PlayStep(0)
        , m_EditStep(0)
        , m_Step()
        , m_MidiNotePlayer()
        , m_MidiNoteDisplay()
    {}
    
    void Begin()
    {
      m_MidiNotePlayer.Begin();
      m_MidiNoteDisplay.Begin();
      // display initial values:
      for(int DisplayStep = 0; DisplayStep<NumSteps; ++DisplayStep)
      {
        m_MidiNoteDisplay.Update(DisplayStep, m_Step[DisplayStep].s_MidiNote, m_Step[DisplayStep].s_Velocity, m_Step[DisplayStep].s_Duration);
      }
    }

    int Update(int RotaryPosition, bool EditModeButtonPressed, bool SelectStepButtonPressed, unsigned long TimeStampMilliSeconds)
    {
        // button off -> on => toggle edit mode (octave, note, velocity, Duration, Tempo)
        if(m_EditModeButtonPressed != EditModeButtonPressed)
        {
            if(EditModeButtonPressed)
            {
                m_EditMode = static_cast<EEditMode>( (m_EditMode+1)%EditModeSize );
            }
            m_EditModeButtonPressed = EditModeButtonPressed;
        }
        
        if(m_SelectStepButtonPressed!=SelectStepButtonPressed)
        {
          if(SelectStepButtonPressed)
          {
            m_EditStep = (m_EditStep+1)%NumSteps;
          }
          m_SelectStepButtonPressed = SelectStepButtonPressed;
                      
          // update display
          m_MidiNoteDisplay.Update(m_EditStep, m_Step[m_EditStep].s_MidiNote, m_Step[m_EditStep].s_Velocity, m_Step[m_EditStep].s_Duration);
        }

        // RotaryPosition changed => apply change to current edit mode
        if(m_RotaryPosition != RotaryPosition)
        {
            int RotaryPositionChange = RotaryPosition - m_RotaryPosition;
            switch(m_EditMode)
            {
            case Octave:
                m_Step[m_EditStep].UpdateOctave(RotaryPositionChange);
                break;
            case Note:
                m_Step[m_EditStep].UpdateNote(RotaryPositionChange);
                break;
            case Velocity:
                m_Step[m_EditStep].UpdateVelocity(RotaryPositionChange);
                break;
            case Duration:
                m_Step[m_EditStep].UpdateDuration(RotaryPositionChange);
                break;
            case Tempo:
                m_Period.UpdateTempo(RotaryPositionChange);
                break;
            case EditModeSize:
            default:
                break;
            }

            m_RotaryPosition = RotaryPosition;

            // update display
            m_MidiNoteDisplay.Update(m_EditStep, m_Step[m_EditStep].s_MidiNote, m_Step[m_EditStep].s_Velocity, m_Step[m_EditStep].s_Duration);
        }

        // timestamp => note on / note off required?
        int Action = m_Period.Update(TimeStampMilliSeconds, m_Step[m_PlayStep].s_Duration);
        // action => on / off / do nothing
        if(Action == CPeriodic::NoteOnAction)
        {
            // goto next step
            m_PlayStep = (m_PlayStep+1)%NumSteps;
            // 
            m_MidiNotePlayer.NoteOn(m_Step[m_PlayStep].s_MidiNote, m_Step[m_PlayStep].s_Velocity);
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
        
    bool GetState() const
    {
      return m_Period.GetState();
    }

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

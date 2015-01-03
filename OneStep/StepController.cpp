#include "StepController.h"

COneStepController::COneStepController()
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

void COneStepController::Begin()
{
    m_MidiNotePlayer.Begin();
    m_MidiNoteDisplay.Begin();
    // display initial values:
    for(int DisplayStep = 0; DisplayStep<NumSteps; ++DisplayStep)
    {
        m_MidiNoteDisplay.Update(DisplayStep, m_Step[DisplayStep].s_MidiNote, m_Step[DisplayStep].s_Velocity, m_Step[DisplayStep].s_Duration, m_Step[DisplayStep].s_Active);
    }
}

int COneStepController::Update(int RotaryPosition, bool EditModeButtonPressed, bool SelectStepButtonPressed, unsigned long TimeStampMilliSeconds)
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
            // toggle in interval [0, NumSteps[ use NumSteps as 'EditAll'
            m_EditStep = (m_EditStep+1)%(1+NumSteps);
        }
        m_SelectStepButtonPressed = SelectStepButtonPressed;

        // update display => edit step
        m_MidiNoteDisplay.UpdateEditStep(m_EditStep);
    }

    // RotaryPosition changed => apply change to current edit mode
    if(m_RotaryPosition != RotaryPosition)
    {
        int MinEditStep = (m_EditStep == EditAll) ? 0 : m_EditStep;
        int MaxEditStep = (m_EditStep == EditAll) ? NumSteps-1 : m_EditStep;

        int RotaryPositionChange = RotaryPosition - m_RotaryPosition;
        switch(m_EditMode)
        {
        case Octave:
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateOctave(RotaryPositionChange);
            }
            break;
        case Note:
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateNote(RotaryPositionChange);
            }
            break;
        case Velocity:
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateVelocity(RotaryPositionChange);
            }
            break;
        case Duration:
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateDuration(RotaryPositionChange);
            }
            break;
        case Active:
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateActive(RotaryPositionChange);
            }
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
        for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
        {
            m_MidiNoteDisplay.Update(EditStep, m_Step[EditStep].s_MidiNote, m_Step[EditStep].s_Velocity, m_Step[EditStep].s_Duration, m_Step[EditStep].s_Active);
        }
    }

    // timestamp => note on / note off required?
    int Action = m_Period.Update(TimeStampMilliSeconds, m_Step[m_PlayStep].s_Duration);
    // action => on / off / do nothing
    if(Action == CPeriodic::NoteOnAction)
    {
        // goto next step
        m_PlayStep = (m_PlayStep+1)%NumSteps;
        //
        if(m_Step[m_PlayStep].s_Active)
        {
            m_MidiNotePlayer.NoteOn(m_Step[m_PlayStep].s_MidiNote, m_Step[m_PlayStep].s_Velocity);
        }
    }
    else if(Action == CPeriodic::NoteOffAction)
    {
        m_MidiNotePlayer.NoteOff();
    }
    return Action;
}

COneStepController::EEditMode COneStepController::GetEditMode() const
{
    return m_EditMode;
}

bool COneStepController::GetState() const
{
    return m_Period.GetState();
}

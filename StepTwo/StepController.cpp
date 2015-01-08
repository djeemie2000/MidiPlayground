#include "StepController.h"

COneStepController::COneStepController()
    : m_EditModeButtonPressed(false)
    , m_Rotary1Position(0)//?? initial value??
    , m_Rotary2Position(0)//?? initial value??
    , m_Rotary3Position(0)//?? initial value??
    , m_SelectStepButtonPressed(false)
    , m_EditMode(NoteParameters)
    , m_Period()
    , m_PlayStep(0)
    , m_EditStep(0)
    , m_Step()
    , m_Stepping()
    , m_NumUpdates(0)
    , m_MidiNotePlayer()
    , m_Display()
{}

void COneStepController::Begin()
{
    for(int Step = 0; Step<NumSteps; ++Step)
    {
        m_MidiNotePlayer[Step].Begin();
    }
    m_Display.Begin();
    // display initial values:
    m_Display.Update(m_EditMode, m_Step, NumSteps, m_EditStep, m_Period.GetTempoBpm(), m_Stepping.GetStepSize(), m_Stepping.GetStepIntervalBegin(), m_Stepping.GetStepIntervalLength(), 0, 0);
}

int COneStepController::Update(int Rotary1Position, int Rotary2Position, int Rotary3Position,
                               bool EditModeButtonPressed,
                               bool SelectStepButtonPressed,
                               unsigned long TimeStampMilliSeconds)
{
    ++m_NumUpdates;

    bool UpdateDisplay = false;

    // button off -> on => toggle edit mode
    if(m_EditModeButtonPressed != EditModeButtonPressed)
    {
        if(EditModeButtonPressed)
        {
            m_EditMode = static_cast<EEditMode>( (m_EditMode+1)%EditModeSize );
        }
        m_EditModeButtonPressed = EditModeButtonPressed;

        UpdateDisplay = true;
    }

    if(m_SelectStepButtonPressed!=SelectStepButtonPressed)
    {
        if(SelectStepButtonPressed)
        {
            // toggle in interval [0, NumSteps[ + use NumSteps as 'EditAll'
            m_EditStep = (m_EditStep+1)%(1+NumSteps);
        }
        m_SelectStepButtonPressed = SelectStepButtonPressed;

        // update display => edit step
        UpdateDisplay = true;
    }

    int MinEditStep = (m_EditStep == EditAll) ? 0 : m_EditStep;
    int MaxEditStep = (m_EditStep == EditAll) ? NumSteps-1 : m_EditStep;
    // TODO set Step.s_Edit and use this in code below

    if(m_Rotary1Position != Rotary1Position)
    {
        int RotaryPositionChange = Rotary1Position - m_Rotary1Position;
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed velocity
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateVelocity(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {
            // changed duration
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateDuration(RotaryPositionChange);
            }
        }
            break;
        case TempoParameters:
        {
            // changed StepSize
            m_Stepping.UpdateStepSize(RotaryPositionChange);
        }
            break;
        case SteppingParameters:
        {
            // changed StepIntervalBegin
            m_Stepping.UpdateStepSize(RotaryPositionChange);
        }
            break;
        case ActivationParameters:
        {
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateActive(RotaryPositionChange);
            }
        }
            break;
        case EditModeSize:
        default:
            break;
        }

        m_Rotary1Position = Rotary1Position;
        UpdateDisplay = true;
    }

    if(m_Rotary2Position != Rotary2Position)
    {
        int RotaryPositionChange = Rotary2Position - m_Rotary2Position;
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed Note
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateNote(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateNumSubSteps(RotaryPositionChange);
            }
        }
            break;
        case TempoParameters:
        {
            // changed Bpm BpB
            if(0<RotaryPositionChange)
            {
                // Bpm x 2
                m_Period.UpdateTempo(m_Period.GetTempoBpm());
            }
            else if(RotaryPositionChange<0)
            {
                // Bmp / 2
                m_Period.UpdateTempo(-m_Period.GetTempoBpm()/2);
            }
        }
            break;
        case SteppingParameters:
        {
            // changed StepIntervalBegin
            m_Stepping.UpdateStepIntervalBegin(RotaryPositionChange);
        }
            break;
        case ActivationParameters:
        {
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateActive(RotaryPositionChange);
            }
        }
            break;
        case EditModeSize:
        default:
            break;
        }

        m_Rotary2Position = Rotary2Position;
        UpdateDisplay = true;
    }

    if(m_Rotary3Position != Rotary3Position)
    {
        int RotaryPositionChange = Rotary3Position - m_Rotary3Position;
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed Octave
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateOctave(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateGateMode(RotaryPositionChange);
            }
        }
            break;
        case TempoParameters:
        {
            // changed Bpm
            m_Period.UpdateTempo(RotaryPositionChange);//relative change???
        }
            break;
        case SteppingParameters:
        {
            // changed StepIntervalLength
            m_Stepping.UpdateStepIntervalLength(RotaryPositionChange);
        }
            break;
        case ActivationParameters:
        {
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateActive(RotaryPositionChange);
            }
        }
        case EditModeSize:
        default:
            break;
        }

        m_Rotary3Position = Rotary3Position;
        UpdateDisplay = true;
    }

    //        case Active:
    //            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
    //            {
    //                m_Step[EditStep].UpdateActive(RotaryPositionChange);
    //            }
    //            break;

    // always update when in debug mode
    if(m_EditMode==DebugMode)
    {
        UpdateDisplay = true;
    }

    if(UpdateDisplay)
    {
        m_Display.Update(m_EditMode, m_Step, NumSteps, m_EditStep, m_Period.GetTempoBpm(), m_Stepping.GetStepSize(), m_Stepping.GetStepIntervalBegin(), m_Stepping.GetStepIntervalLength(), TimeStampMilliSeconds, m_NumUpdates);
    }

    // timestamp => note on / note off required?
    int Action = m_Period.Update(TimeStampMilliSeconds, m_Step[m_PlayStep].s_Duration);
    // action => on / off / do nothing
    if(Action == CPeriodic::NoteOnAction)
    {
        // advance to next play step
        m_PlayStep = m_Stepping.Advance(m_PlayStep);//m_PlayStep = (m_PlayStep+1)%NumSteps;
        //
        if(m_Step[m_PlayStep].s_Active)
        {
            m_MidiNotePlayer[m_PlayStep].NoteOn(m_Step[m_PlayStep].s_MidiNote, m_Step[m_PlayStep].s_Velocity);
        }
    }
    else if(Action == CPeriodic::NoteOffAction)
    {
        m_MidiNotePlayer[m_PlayStep].NoteOff();
    }
    return Action;
}

bool COneStepController::GetStepState(int Step) const
{
    return (m_PlayStep==Step) && m_Period.GetState();
}

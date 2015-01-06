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
//    for(int DisplayStep = 0; DisplayStep<NumSteps; ++DisplayStep)
//    {
//        m_MidiNoteDisplay.UpdateNoteParameters(DisplayStep, m_Step[DisplayStep].s_MidiNote, m_Step[DisplayStep].s_Velocity, m_Step[DisplayStep].s_Duration, m_Step[DisplayStep].s_Active);
//    }
    m_Display.Update(m_EditMode, m_Step, NumSteps, m_EditStep, m_Period.GetTempoBpm(), 0, 0);
}

int COneStepController::Update(int Rotary1Position, int Rotary2Position, int Rotary3Position,
                               bool EditModeButtonPressed,
                               bool SelectStepButtonPressed,
                               unsigned long TimeStampMilliSeconds)
{
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

    if(m_Rotary1Position != Rotary1Position)
    {
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed velocity
            int RotaryPositionChange = Rotary1Position - m_Rotary1Position;
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateVelocity(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {
            // changed duration
            int RotaryPositionChange = Rotary1Position - m_Rotary1Position;
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateDuration(RotaryPositionChange);
            }
        }
            break;
        case TempoParameters:
            break;
        case SteppingParameters:
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
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed Note
            int RotaryPositionChange = Rotary2Position - m_Rotary2Position;
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateNote(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {

        }
            break;
        case TempoParameters:
        {
            // changed Bpm BpB
            int RotaryPositionChange = Rotary2Position - m_Rotary2Position;
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
        switch(m_EditMode)
        {
        case NoteParameters:
        {
            // changed Octave
            int RotaryPositionChange = Rotary3Position - m_Rotary3Position;
            for(int EditStep = MinEditStep; EditStep<=MaxEditStep; ++EditStep)
            {
                m_Step[EditStep].UpdateOctave(RotaryPositionChange);
            }
        }
            break;
        case SubStepParameters:
        {

        }
            break;
        case TempoParameters:
        {
            // changed Bpm
            int RotaryPositionChange = Rotary3Position - m_Rotary3Position;
            m_Period.UpdateTempo(RotaryPositionChange);//relative change???
        }
            break;
        case SteppingParameters:
            break;
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

    if(UpdateDisplay)
    {
        m_Display.Update(m_EditMode, m_Step, NumSteps, m_EditStep, m_Period.GetTempoBpm(), 0, 0);
    }

    // timestamp => note on / note off required?
    int Action = m_Period.Update(TimeStampMilliSeconds, m_Step[m_PlayStep].s_Duration);
    // action => on / off / do nothing
    if(Action == CPeriodic::NoteOnAction)
    {
        // goto next play step
        m_PlayStep = (m_PlayStep+1)%NumSteps;
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

bool COneStepController::GetState() const
{
    return m_Period.GetState();
}

bool COneStepController::GetStepState(int Step) const
{
    return (m_PlayStep==Step) && m_Period.GetState();
}

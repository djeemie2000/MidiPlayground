#include "Step.h"
#include "Crop.h"

SStep::SStep()
    : s_MidiNote(0x40)//64
    , s_Velocity(MaxVelocity)
    , s_Duration(DurationScale/2)
    , s_Active(true)
    , s_NumSubSteps(1)
    , s_GateMode(RepeatGateMode)
    , s_Edit(false)
{}

void SStep::UpdateOctave(int Change)
{
    // TODO do not change note when cropping
    const int NumMidiNotesPerOctave = 12;
    s_MidiNote = static_cast<uint8_t>(Crop(s_MidiNote + Change * NumMidiNotesPerOctave, MinMidiNote, MaxMidiNote));
}

void SStep::UpdateNote(int Change)
{
    s_MidiNote = static_cast<uint8_t>(Crop(s_MidiNote + Change, MinMidiNote, MaxMidiNote));
}

void SStep::UpdateVelocity(int Change)
{
    s_Velocity = static_cast<uint8_t>(Crop(s_Velocity + Change, MinVelocity, MaxVelocity));
}

void SStep::UpdateDuration(int Change)
{
    s_Duration = static_cast<uint8_t>(Crop(s_Duration + Change, MinDuration, MaxDuration));
}

void SStep::UpdateActive(int Change)
{
    // zero => unchanged
    // positive => on
    // negative => off
    if(0<Change)
    {
        s_Active = true;
    }
    else if(Change<0)
    {
        s_Active = false;
    }
}

void SStep::UpdateNumSubSteps(int Change)
{
    s_NumSubSteps = Crop(s_NumSubSteps + Change, MinNumSubSteps, MaxNumSubSteps);
}

void SStep::UpdateGateMode(int Change)
{
    // R <-> L <-> P
    if(0<Change)
    {
        if(s_GateMode==RepeatGateMode)
        {
            s_GateMode = LengthGateMode;
        }
        else if(s_GateMode==LengthGateMode)
        {
            s_GateMode = PulseGateMode;
        }
    }
    else if(Change<0)
    {
        if(s_GateMode==LengthGateMode)
        {
            s_GateMode = RepeatGateMode;
        }
        else if(s_GateMode==PulseGateMode)
        {
            s_GateMode = LengthGateMode;
        }
    }
}

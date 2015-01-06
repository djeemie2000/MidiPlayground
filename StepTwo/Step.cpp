#include "Step.h"
#include "Crop.h"

SStep::SStep()
    : s_MidiNote(0x40)//64
    , s_Velocity(MaxVelocity)
    , s_Duration(0x40)//64
    , s_Active(true)
    , s_NumSubSteps(1)
    , s_GateMode(RepeatGateMode)
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

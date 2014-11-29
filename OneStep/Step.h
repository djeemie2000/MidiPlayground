#ifndef STEP_H
#define STEP_H

//#include <cstdint>

//using std::min;
//using std::max;

int Crop(int Value, int Min, int Max)
{
    return max(min(Value, Max), Min);
}

struct SStep
{
    static const int MinMidiNote = 24;//C0
    static const int MaxMidiNote = 127;//G11
    static const int MinVelocity = 0;
    static const int MaxVelocity = 127;
    static const int MinDuration = 1;
    static const int MaxDuration = 127;


    uint8_t s_MidiNote;
    uint8_t s_Velocity;
    uint8_t s_Duration;

    SStep()
     : s_MidiNote(0x40)//64
     , s_Velocity(MaxVelocity)
     , s_Duration(0x40)//64
    {}

    void UpdateOctave(int Change)
    {
        const int NumMidiNotesPerOctave = 12;
        s_MidiNote = static_cast<uint8_t>(Crop(s_MidiNote + Change * NumMidiNotesPerOctave, MinMidiNote, MaxMidiNote));
    }

    void UpdateNote(int Change)
    {
        s_MidiNote = static_cast<uint8_t>(Crop(s_MidiNote + Change, MinMidiNote, MaxMidiNote));
    }

    void UpdateVelocity(int Change)
    {
        s_Velocity = static_cast<uint8_t>(Crop(s_Velocity + Change, MinVelocity, MaxVelocity));
    }

    void UpdateDuration(int Change)
    {
        s_Duration = static_cast<uint8_t>(Crop(s_Duration + Change, MinDuration, MaxDuration));
    }
};

#endif // STEP_H
#ifndef STEP_H
#define STEP_H

#include <Arduino.h>

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
    bool    s_Active;

    SStep();

    void UpdateOctave(int Change);
    void UpdateNote(int Change);
    void UpdateVelocity(int Change);
    void UpdateDuration(int Change);
    void UpdateActive(int Change);
};

#endif // STEP_H

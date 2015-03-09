#ifndef STEP_H
#define STEP_H

#include <Arduino.h>

struct SStep
{
    static const int MinMidiNote = 24;//C0
    static const int MaxMidiNote = 127;//G11
    static const int VelocityScale = 8;
    static const int MinVelocity = 0;
    static const int MaxVelocity = VelocityScale -1;
    static const int DurationScale = 8;
    static const int MinDuration = 1;
    static const int MaxDuration = DurationScale - 1;
    static const int MinNumSubSteps = 1;
    static const int MaxNumSubSteps = 8;

    static const char RepeatGateMode = 'R';
    static const char LengthGateMode = 'L';
    static const char PulseGateMode = 'P';

    static const char ContinueStepMode = '+';
    static const char InvertStepMode = '-';
    static const char SkipStepMode = ' ';
    static const char ResetStepMode = '0';
    static const char RandomStepMode = '?';

    uint8_t s_MidiNote;
    uint8_t s_Velocity;
    uint8_t s_Duration;
    bool    s_Active;
    int     s_NumSubSteps;
    char    s_GateMode;
    bool    s_Edit;
    char    s_StepMode;

    SStep();

    void UpdateOctave(int Change);
    void UpdateNote(int Change);
    void UpdateVelocity(int Change);
    void UpdateDuration(int Change);
    void UpdateActive(int Change);
    void UpdateNumSubSteps(int Change);
    void UpdateGateMode(int Change);
    void UpdateEdit(int Change);
    void UpdateStepMode(int Change);
};

#endif // STEP_H

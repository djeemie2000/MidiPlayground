#ifndef MIDINOTEDISPLAY_H
#define MIDINOTEDISPLAY_H

#include <Arduino.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include "EditMode.h"
#include "Step.h"

class CMidiNoteDisplay
{
public:
    CMidiNoteDisplay();
    void Begin();
    void SetEditMode(EEditMode EditMode);
    void UpdateNoteParameters(int Step, uint8_t MidiNote, uint8_t Velocity, uint8_t Duration, bool Active);
    void UpdateEditStep(int Step);

    /*
     * Call update upon change of edit mode and/or parameters.
     */
    void Update(EEditMode EditMode,
                const SStep* Steps, int NumSteps,// note and (sub)step
                int EditStep,
                int Bpm,//tempo
                // TODO stepping
                unsigned long MilliSeconds, unsigned long NumUpdates);

private:
    LiquidCrystal_I2C	m_lcd; 

    int m_EditStep;
    EEditMode m_EditMode;
};

#endif // MIDINOTEDISPLAY_H

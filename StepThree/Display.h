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

    /*
     * Call update upon change of edit mode and/or parameters.
     */
    void Update(EEditMode EditMode,
                const SStep* Steps, int NumSteps, // note and (sub)step
                int EditStep,
                int Bpm, //tempo
                int StepSize, // stepping
                unsigned long MilliSeconds, unsigned long NumUpdates);//debug

private:
    LiquidCrystal_I2C	m_lcd; 

    EEditMode m_EditMode;
};

#endif // MIDINOTEDISPLAY_H

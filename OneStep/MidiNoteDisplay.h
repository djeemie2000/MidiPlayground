#ifndef MIDINOTEDISPLAY_H
#define MIDINOTEDISPLAY_H

//#include <cstdint>
#include <Arduino.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

class CMidiNoteDisplay
{
public:
    CMidiNoteDisplay();
    void Begin();
    void Update(int Step, uint8_t MidiNote, uint8_t Velocity, uint8_t Duration, bool Active);
    void UpdateEditStep(int Step);
private:
    LiquidCrystal_I2C	m_lcd; 
    int m_EditStep;
};

#endif // MIDINOTEDISPLAY_H


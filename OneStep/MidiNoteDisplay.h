

#ifndef MIDINOTEDISPLAY_H
#define MIDINOTEDISPLAY_H

//#include <cstdint>
#include <arduino.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>



char MidiNoteToNote(uint8_t MidiNote)
{
    static const char Notes[] = { 'C', 'c', 'D', 'd', 'E', 'F', 'f', 'G', 'g', 'A', 'a', 'B' };
    return Notes[MidiNote%12];
}

int MidiNoteToOctave(uint8_t MidiNote)
{
     return MidiNote/12;
}

class CMidiNoteDisplay
{
public:
    CMidiNoteDisplay();
    void Begin();
    void Update(uint8_t MidiNote, uint8_t Velocity, uint8_t Duration);
private:
    LiquidCrystal_I2C	m_lcd; 
};
 
CMidiNoteDisplay::CMidiNoteDisplay()
: m_lcd(0x27,2,1,0,4,5,6,7) // 0x27 is the I2C bus address for an unmodified backpack
{
}

void CMidiNoteDisplay::Begin()
{    
  // activate LCD module
  m_lcd.begin (16,2); // for 16 x 2 LCD module
  m_lcd.setBacklightPin(3,POSITIVE);
  m_lcd.setBacklight(HIGH);//backlight on
  m_lcd.cursor();
  //m_lcd.blink();
}

void CMidiNoteDisplay::Update(uint8_t MidiNote, uint8_t Velocity, uint8_t Duration)
{
  // display update
  m_lcd.setCursor(0,0); // set cursor to 0,0
  m_lcd.print(MidiNoteToNote(MidiNote));
  m_lcd.print(MidiNoteToOctave(MidiNote));
      
  m_lcd.setCursor(0, 1);
  m_lcd.print("V");
  m_lcd.print(Velocity, DEC);
  m_lcd.print(" D");
  m_lcd.print(Duration, DEC);
  
  m_lcd.setCursor(1,0); // set cursor to 0,0
  
}

#endif // MIDINOTEDISPLAY_H

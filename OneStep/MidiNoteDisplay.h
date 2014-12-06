

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
    void Update(int Step, uint8_t MidiNote, uint8_t Velocity, uint8_t Duration);
    void UpdateEditStep(int Step);
private:
    LiquidCrystal_I2C	m_lcd; 
    int m_EditStep;
};
 
CMidiNoteDisplay::CMidiNoteDisplay()
: m_lcd(0x27,2,1,0,4,5,6,7) // 0x27 is the I2C bus address for an unmodified backpack
, m_EditStep(0)
{
}

void CMidiNoteDisplay::Begin()
{    
  // activate LCD module
  m_lcd.begin (16,2); // for 16 x 2 LCD module
  m_lcd.setBacklightPin(3,POSITIVE);
  m_lcd.setBacklight(HIGH);//backlight on
  m_lcd.noCursor();// show cursor
  m_lcd.blink();//noBlink();// no blinking cursor
  UpdateEditStep(m_EditStep);
  
  uint8_t CustomCharacter[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  m_lcd.createChar(0, CustomCharacter);
  for(int Bar = 1; Bar<8; ++Bar)
  {
    CustomCharacter[8-Bar] = 0x1F;//5 msb bits
    m_lcd.createChar(Bar, CustomCharacter);
  }
}

void CMidiNoteDisplay::Update(int Step, uint8_t MidiNote, uint8_t Velocity, uint8_t Duration)
{
  if(0<=Step && Step<8)
  {
    int x = 2*Step;
    // display update
    m_lcd.setCursor(x,0); // set cursor to 0,0
    m_lcd.print(MidiNoteToNote(MidiNote));
    m_lcd.print(MidiNoteToOctave(MidiNote));
        
    m_lcd.setCursor(x, 1);
    uint8_t VelocityRescaled = Velocity/16;
    m_lcd.write(VelocityRescaled);
    uint8_t DurationRescaled = Duration/16;
    m_lcd.write(DurationRescaled);
    
    // set cursor underneath edited step octave
    UpdateEditStep(m_EditStep);
  } 
}

void CMidiNoteDisplay::UpdateEditStep(int Step)
{
    if(0<=Step && Step<8)
    {
      if(m_EditStep==8)
      {
        m_lcd.cursor();
      }
      m_EditStep = Step;
      // set cursor underneath edited step octave
      m_lcd.setCursor(m_EditStep*2+1, 0); 
    }
    else if(Step==8)
    {
      m_EditStep = Step;
      // edit all / none
      m_lcd.noBlink();
    }
}

#endif // MIDINOTEDISPLAY_H


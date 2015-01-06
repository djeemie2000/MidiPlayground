#include "Display.h"

namespace
{

char MidiNoteToNote(uint8_t MidiNote)
{
    static const char Notes[] = { 'C', 'c', 'D', 'd', 'E', 'F', 'f', 'G', 'g', 'A', 'a', 'B' };
    return Notes[MidiNote%12];
}

int MidiNoteToOctave(uint8_t MidiNote)
{
     return MidiNote/12;
}

}

CMidiNoteDisplay::CMidiNoteDisplay()
: m_lcd(0x27,2,1,0,4,5,6,7) // 0x27 is the I2C bus address for an unmodified backpack
, m_EditStep(0)
, m_EditMode(NoteParameters)
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

  uint8_t CustomCharacter[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
  m_lcd.createChar(0, CustomCharacter);
  for(int Bar = 1; Bar<8; ++Bar)
  {
    CustomCharacter[7-Bar] = 0x1F;//5 msb bits
    m_lcd.createChar(Bar, CustomCharacter);
  }
}

void CMidiNoteDisplay::SetEditMode(EEditMode EditMode)
{
    m_EditMode = EditMode;
    //TODO update display
}

void CMidiNoteDisplay::UpdateNoteParameters(int Step, uint8_t MidiNote, uint8_t Velocity, uint8_t Duration, bool Active)
{
  if(0<=Step && Step<8)
  {
    int x = 2*Step;
    // display update
    m_lcd.setCursor(x,0); // set cursor to 0,0
    m_lcd.print(MidiNoteToNote(MidiNote));
    m_lcd.print(MidiNoteToOctave(MidiNote));

    m_lcd.setCursor(x, 1);
    if(Active)
    {
      uint8_t VelocityRescaled = Velocity/16;
      m_lcd.write(VelocityRescaled);
      uint8_t DurationRescaled = Duration/16;
      m_lcd.write(DurationRescaled);
    }
    else
    {
      m_lcd.print("  ");
    }
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
        m_lcd.blink();
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

void CMidiNoteDisplay::Update(EEditMode EditMode, const SStep *Steps, int NumSteps, int EditStep, int Bpm, unsigned long MilliSeconds, unsigned long NumUpdates)
{
    switch(EditMode)
    {
    case NoteParameters:
        m_lcd.home();
        m_lcd.blink();
        // first line: Note/Octave
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(MidiNoteToNote(Steps[idx].s_MidiNote));
            m_lcd.print(MidiNoteToOctave(Steps[idx].s_MidiNote));
        }
        // second line: Velocity bar / active
        m_lcd.setCursor(0,1);
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            uint8_t VelocityRescaled = Steps[idx].s_Velocity/16;
            m_lcd.write(VelocityRescaled);
            m_lcd.print(Steps[idx].s_Active ? "+" : " ");
        }
        // cursor
        if(EditStep==NumSteps)
        {
            m_lcd.noBlink();
        }
        else
        {
            m_lcd.setCursor(2*EditStep+1, 0);
            m_lcd.blink();
        }
        break;
    case SubStepParameters:
        m_lcd.home();
        // first line: GateMode/NumSteps
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(Steps[idx].s_GateMode);
            m_lcd.print(Steps[idx].s_NumSubSteps%10);
        }
        // second line: Duration bar / active
        m_lcd.setCursor(0,1);
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            uint8_t DurationRescaled = Steps[idx].s_Duration/16;
            m_lcd.write(DurationRescaled);
            m_lcd.print(Steps[idx].s_Active ? "+" : " ");
        }
        // cursor
        if(EditStep==NumSteps)
        {
            m_lcd.noBlink();
        }
        else
        {
            m_lcd.setCursor(2*EditStep+1, 0);
            m_lcd.blink();
        }
        break;
    case TempoParameters:
        m_lcd.home();
        m_lcd.noBlink();
        m_lcd.print("BPM ");
        m_lcd.println(Bpm);
        break;
    case SteppingParameters:
        m_lcd.home();
        m_lcd.noBlink();
        m_lcd.println("Step 1 ");
        m_lcd.print("[1, 8]");
        break;
    case DebugMode:
        m_lcd.home();
        m_lcd.noBlink();
        m_lcd.print("mSec ");
        m_lcd.println(MilliSeconds);
        m_lcd.print("# ");
        m_lcd.println(NumUpdates);
        break;
    default:
        break;
    }
}


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
, m_EditMode(NoteParameters)
{
}

void CMidiNoteDisplay::Begin()
{
  // activate LCD module
  m_lcd.begin (16,2); // for 16 x 2 LCD module
  m_lcd.setBacklightPin(3,POSITIVE);
  m_lcd.setBacklight(HIGH);//backlight on

  uint8_t CustomCharacter[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
  m_lcd.createChar(0, CustomCharacter);
  for(int Bar = 1; Bar<8; ++Bar)
  {
    CustomCharacter[7-Bar] = 0x1F;//5 msb bits
    m_lcd.createChar(Bar, CustomCharacter);
  }

  m_lcd.print("****StepSeq ****");
  m_lcd.setCursor(0,1);
  m_lcd.print("****  V0.2  ****");
}

void CMidiNoteDisplay::Update(EEditMode EditMode, const SStep *Steps, int NumSteps, int EditStep,
                              int Bpm,
                              int StepSize, int StepIntervalBegin, int StepIntervalLength,
                              unsigned long MilliSeconds, unsigned long NumUpdates)
{
    if(EditMode!=m_EditMode)
    {
        // changed edit mode => clear all display
        m_lcd.clear();
        m_EditMode = EditMode;
    }

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
            uint8_t VelocityRescaled = Steps[idx].s_Velocity*8/SStep::VelocityScale;
            m_lcd.write(VelocityRescaled);
            m_lcd.print(Steps[idx].s_Active ? "x" : "-");
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
            uint8_t DurationRescaled = Steps[idx].s_Duration*8/SStep::DurationScale;
            m_lcd.write(DurationRescaled);
            m_lcd.print(Steps[idx].s_Active ? "x" : "-");
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
        m_lcd.print(Bpm);
        break;
    case SteppingParameters:
    case ActivationParameters:
        m_lcd.home();
        m_lcd.noBlink();
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            if(idx<StepIntervalBegin || StepIntervalBegin+StepIntervalLength<=idx)
            {
                m_lcd.print("-");
            }
            else
            {
                m_lcd.print("x");
            }
        }
        m_lcd.print(" Step=");
        m_lcd.print(StepSize);
        m_lcd.setCursor(0,1);
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            if(Steps[idx].s_Active)
            {
                m_lcd.print("x");
            }
            else
            {
                m_lcd.print("-");
            }
        }
        // cursor
        if(EditStep==NumSteps)
        {
            m_lcd.noBlink();
        }
        else
        {
            if(EditMode==SteppingParameters)
            {
              m_lcd.setCursor(EditStep, 0);
            }
            if(EditMode==ActivationParameters)
            {
              m_lcd.setCursor(EditStep, 1);
            }
            m_lcd.blink();
        }
        break;
    case DebugMode:
        m_lcd.home();
        m_lcd.noBlink();
        m_lcd.print("mSec ");
        m_lcd.print(MilliSeconds);
        m_lcd.setCursor(0,1);
        m_lcd.print("#  0x");
        m_lcd.print(NumUpdates, HEX);
        break;
    default:
        break;
    }
}

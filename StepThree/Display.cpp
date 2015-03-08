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
  m_lcd.begin (20,4); // for 20 x 4 LCD module
  m_lcd.setBacklightPin(3,POSITIVE);
  m_lcd.setBacklight(HIGH);//backlight on

  uint8_t CustomCharacter[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  m_lcd.createChar(0, CustomCharacter);
  for(int Bar = 1; Bar<8; ++Bar)
  {
    CustomCharacter[7-Bar] = 0x1F;//5 msb bits
    m_lcd.createChar(Bar, CustomCharacter);
  }

  m_lcd.setCursor(0,0);
  m_lcd.print("****StepSeq ****");
  m_lcd.setCursor(0,1);
  m_lcd.print("****  V0.3  ****");
  delay(5000);
  m_lcd.clear();
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
        // lines: 1 Note , 2 Octave, 3 velocity (bar), 4 Edit
        m_lcd.print("Note");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(MidiNoteToNote(Steps[idx].s_MidiNote));
        }
        m_lcd.setCursor(0,1);
        m_lcd.print("Oct ");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(MidiNoteToOctave(Steps[idx].s_MidiNote));
        }
        m_lcd.setCursor(0,2);
        m_lcd.print("Vel ");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            uint8_t VelocityRescaled = Steps[idx].s_Velocity*8/SStep::VelocityScale;
            m_lcd.write(char(VelocityRescaled));
        }

        break;
    case SubStepParameters:
        m_lcd.home();
        // lines : 1 GateMode 2 NumSteps 3 Duration 4 Edit
        m_lcd.print("Gate");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(Steps[idx].s_GateMode);
        }
        m_lcd.setCursor(0,1);
        m_lcd.print("#   ");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            m_lcd.print(Steps[idx].s_NumSubSteps%10);
        }
        m_lcd.setCursor(0,2);
        m_lcd.print("Dur ");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            uint8_t DurationRescaled = Steps[idx].s_Duration*8/SStep::DurationScale;
            m_lcd.write(char(DurationRescaled));
        }

        break;
    case TempoParameters:
        break;
    case SteppingParameters:
    case ActivationParameters:
        m_lcd.home();
        // lines : 1 interval, 2 active, 3 step
        m_lcd.print("Int ");
        for(int idx = 0; idx<NumSteps; ++idx)
        {
            if(idx==StepIntervalBegin)
            {
                m_lcd.print("B");
            }
            else if(idx+1==StepIntervalBegin+StepIntervalLength)
            {
                m_lcd.print("E");
            }
            else if(idx<StepIntervalBegin || StepIntervalBegin+StepIntervalLength<=idx)
            {
                m_lcd.print("-");
            }
            else
            {
                m_lcd.print("x");
            }
        }
        m_lcd.setCursor(0,1);
        m_lcd.print("Act ");
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
        m_lcd.setCursor(0,2);
        m_lcd.print("Step ");
        m_lcd.print(StepSize);
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

    // Bpm + Edit (line 4):
    m_lcd.setCursor(0,3);
    m_lcd.print(Bpm);
    m_lcd.print(" ");
    for(int idx = 0; idx<NumSteps; ++idx)
    {
        if(Steps[idx].s_Edit)
        {
            m_lcd.print("E");
        }
        else
        {
            m_lcd.print(" ");
        }
    }
    // cursor
    if(EditStep==NumSteps)
    {
        m_lcd.noBlink();
    }
    else
    {
        m_lcd.setCursor(4+EditStep, 3);
        m_lcd.blink();
    }
}

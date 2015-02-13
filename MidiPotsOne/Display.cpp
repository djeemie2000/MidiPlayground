#include "Display.h"
#include "ControlChange.h"

CDisplay::CDisplay()
: m_lcd(0x27,2,1,0,4,5,6,7) // 0x27 is the I2C bus address for an unmodified backpack
{
}

void CDisplay::Begin()
{
  // activate LCD module
  m_lcd.begin (20,4); // for 20 x 4 LCD module
  m_lcd.setBacklightPin(3,POSITIVE);
  m_lcd.setBacklight(HIGH);//backlight on

  m_lcd.setCursor(0,0);
  m_lcd.print("**MidiController**");
  m_lcd.setCursor(0,1);
  m_lcd.print("****  V0.1  ****");
  delay(5000);
  m_lcd.clear();
  m_lcd.blink();
}

void CDisplay::Update(EEditMode EditMode, const SControlChange *ControlChange, int NumControllers)
{
    // lines: 1 Value , 2 Controller
    m_lcd.setCursor(0,0);
    m_lcd.print("Val ");
    for(int Ctrl = 0; Ctrl<4; ++Ctrl)
    {
        m_lcd.setCursor(4+4*Ctrl, 0);
        if(100<=ControlChange[Ctrl].s_Value)
        {
            m_lcd.print(" ");
        }
        else if(10<=ControlChange[Ctrl].s_Value)
        {
            m_lcd.print("  ");
        }
        else
        {
            m_lcd.print("   ");
        }
        m_lcd.print(ControlChange[Ctrl].s_Value);
    }

    m_lcd.setCursor(0,1);
    m_lcd.print("Ctrl");
    for(int Ctrl = 0; Ctrl<4; ++Ctrl)
    {
        m_lcd.setCursor(4+4*Ctrl, 1);
        if(100<=ControlChange[Ctrl].s_Controller)
        {
            m_lcd.print(" ");
        }
        else if(10<=ControlChange[Ctrl].s_Controller)
        {
            m_lcd.print("  ");
        }
        else
        {
            m_lcd.print("   ");
        }
        m_lcd.print(ControlChange[Ctrl].s_Controller);
    }

    if(EditMode==ControllerMode)
    {
        m_lcd.setCursor(4,0);
    }
    else if(EditMode==AssignControllerMode)
    {
        m_lcd.setCursor(4,1);
    }

}

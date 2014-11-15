/* 
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin  
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3) 
 */

// include the library code:
#include <LiquidCrystal.h>
#include "MidiSerial.h"
//TODO class SSoftMidiSerial -> Begin(int RxPin, int TxPin); //always 31250 baudrate

const int Button1Pin = 8;
const int Button2Pin = 9;
const int Pot1Pin = A0;
const int Pot2Pin = A1;

// initialize the library with the numbers of the interface pins
const int NumPots = 2;

void DisplayInteger(LiquidCrystal& lcd, int Value, int x, int y)
{
  // does not handle negative numbers!
  // max 3 digits
  lcd.setCursor(x,y);
  if(Value<10)
  { // 1 digit
    lcd.print("00");
  }
  else if(Value<100)
  {
    // 2 digits
    lcd.print("0");
  }
  lcd.print(Value);
}

class CController
{
public:
  CController()
: 
    m_Lcd(12, 11, 5, 4, 3, 2)
      , m_MidiSerial()
      {
        for(int idx = 0; idx<NumPots; ++idx)
        {
          m_Controller[idx].s_Param = 0x50 + idx;
          m_Controller[idx].s_Value = 64;
        }
      }

  void Setup()
  {
    m_MidiSerial.Begin(9600);    

    // set up the LCD's number of columns and rows: 
    m_Lcd.begin(16, 2);

    m_Lcd.setCursor(0,0);
    m_Lcd.print("C");
    m_Lcd.setCursor(0,1);
    m_Lcd.print("V");
    // default: change value  
    m_Lcd.setCursor(0,1);
    m_Lcd.blink();

    //TODO display initial!
    for(int idx = 0; idx<NumPots; ++idx)
    {
      DisplayInteger(m_Lcd, m_Controller[idx].s_Param, idx*4+1, 0); 
      DisplayInteger(m_Lcd, m_Controller[idx].s_Value, idx*4+1, 1); 
    }
  }

  void Update(bool AssignPressed, int PotValue[NumPots])
  {
    // first, check if pots have been moved
    for(int idx = 0; idx<NumPots; ++idx)
    {
      if(PotValue[idx]!=m_PrevPotValue[idx])
      {
        m_PrevPotValue[idx] = PotValue[idx];

        if(AssignPressed)
        {
          if(m_Controller[idx].s_Param != PotValue[idx])
          {
            m_Controller[idx].s_Param = PotValue[idx];
            // controller param has changed => adjust display
            DisplayInteger(m_Lcd, m_Controller[idx].s_Param, idx*4+1, 0);     
          }
        }
        else
        {
          if(m_Controller[idx].s_Value != PotValue[idx])
          {
            m_Controller[idx].s_Value = PotValue[idx];
            // controller value has changed => adjust display, send midi cc
            m_MidiSerial.ControlChange(1, m_Controller[idx].s_Param, m_Controller[idx].s_Value);
            DisplayInteger(m_Lcd, m_Controller[idx].s_Value, idx*4+1, 1);        
          }

        }
      }
    }

    if(AssignPressed)
    {
      // indicate controller change active
      m_Lcd.setCursor(0,0);      
    }
    else
    {
      // indicate Value change active
      m_Lcd.setCursor(0,1);
    }
  }

private:
  struct SMidiController
  {
    byte s_Param;
    byte s_Value;
  };

  int             m_PrevPotValue[NumPots];
  LiquidCrystal   m_Lcd;
  CMidiSerial     m_MidiSerial;
  SMidiController m_Controller[NumPots];
};

CController Controller;

void setup() {
  pinMode(Button1Pin, INPUT_PULLUP);
  pinMode(Button2Pin, INPUT_PULLUP);

  Controller.Setup();
}

void loop() {
  int PotValue[NumPots];
  PotValue[0] = analogRead(Pot1Pin)/8;// [0,1023] to [0,127]
  PotValue[1] = analogRead(Pot2Pin)/8;// [0,1023] to [0,127]
  bool AssignButtonPressed = (LOW==digitalRead(Button1Pin));

  Controller.Update(AssignButtonPressed, PotValue);
}








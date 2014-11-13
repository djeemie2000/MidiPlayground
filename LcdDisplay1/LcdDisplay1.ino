/*
  LiquidCrystal Library - Blink
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD and makes the 
 cursor block blink.
 
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
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe 
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/LiquidCrystalBlink
 
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

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
CMidiSerial MidiSerial;
int MidiControllerParam = 50;
int MidiControllerValue = 64;

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

  }

  void Update(bool AssignPressed, int PotValue[NumPots])
  {
    if(AssignPressed)
    {
      // adjust parameters
      for(int idx = 0; idx<NumPots; ++idx)
      {
        if(m_Controller[idx].s_Param != PotValue[idx])
        {
          m_Controller[idx].s_Param = PotValue[idx];
          // controller param has changed => adjust display

        }
      }
      m_Lcd.setCursor(0,0);      
    }
    else
    {
      // adjust value
      for(int idx = 0; idx<NumPots; ++idx)
      {
        if(m_Controller[idx].s_Value != PotValue[idx])
        {
          m_Controller[idx].s_Value = PotValue[idx];
          // controller value has changed => adjust display, send midi cc
          MidiSerial.ControlChange(1, MidiControllerParam, MidiControllerValue);
          DisplayInteger(m_Lcd, MidiControllerValue, idx*4+1, 1);        
        }

      }
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

  LiquidCrystal   m_Lcd;
  CMidiSerial     m_MidiSerial;
  SMidiController m_Controller[NumPots];

};


void setup() {
  pinMode(Button1Pin, INPUT_PULLUP);
  pinMode(Button2Pin, INPUT_PULLUP);

  MidiSerial.Begin(9600);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("V");
  // default: change value  
  lcd.setCursor(0,1);
  lcd.blink();

  // show initial values
  DisplayInteger(lcd, MidiControllerParam, 1, 0);
  DisplayInteger(lcd, MidiControllerValue, 1, 1);
}


void loop() {

  int Pot1Value = analogRead(Pot1Pin)/8;// [0,1023] to [0,127]
  bool AssignButtonPressed = (LOW==digitalRead(Button1Pin));

  if(AssignButtonPressed)
  { // assign midi controller param
    if(Pot1Value != MidiControllerParam)
    {
      MidiControllerParam = Pot1Value;      
      //update display
      DisplayInteger(lcd, MidiControllerParam, 1, 0);
    }

    lcd.setCursor(0,0);
  }
  else
  { // change midi controller value
    if(Pot1Value != MidiControllerValue)
    {
      MidiControllerValue = Pot1Value;
      // send midi control change
      MidiSerial.ControlChange(1, MidiControllerParam, MidiControllerValue);
      //update display
      DisplayInteger(lcd, MidiControllerValue, 1, 1);
    }

    lcd.setCursor(0,1);
  }
}



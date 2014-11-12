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

class CMidiSerial
{
public:
  CMidiSerial(){}
  
  void Begin(int Baudrate)
  {
    Serial.begin(Baudrate);
  }
  void ControlChange(byte Channel, byte Controller, byte Value)
  {
    byte Operation = 0xB0 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Controller & 0x7F);
    Serial.write(Value & 0x7F);
  }
  void NoteOn(byte Channel, byte Note, byte Velocity)
  {
    byte Operation = 0x90 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Note & 0x7F);
    Serial.write(Velocity & 0x7F);    
  }
  void NoteOff(byte Channel, byte Note, byte Velocity)
  {
    byte Operation = 0x80 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Note & 0x7F);
    Serial.write(Velocity & 0x7F);    
  }
  //TODO pitchbend
};

//TODO class SSoftMidiSerial -> Begin(int RxPin, int TxPin); //always 31250 baudrate


const int Button1Pin = 8;
const int Button2Pin = 9;
const int Pot1Pin = A0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
CMidiSerial MidiSerial;
int MidiControllerParam = 50;
int MidiControllerValue = 64;

void DisplayInteger(int Value, int x, int y)
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
  DisplayInteger(MidiControllerParam, 1, 0);
  DisplayInteger(MidiControllerValue, 1, 1);
}


void loop() {

  int Pot1Value = analogRead(Pot1Pin)/8;// [0,1023] to [0,127]


  bool Button1Pressed = (LOW==digitalRead(Button1Pin));
  if(Button1Pressed)
  { // assign midi controller param
    if(Pot1Value != MidiControllerParam)
    {
      MidiControllerParam = Pot1Value;      
      //update display
      DisplayInteger(MidiControllerParam, 1, 0);
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
      DisplayInteger(MidiControllerValue, 1, 1);
    }

    lcd.setCursor(0,1);
  }
}



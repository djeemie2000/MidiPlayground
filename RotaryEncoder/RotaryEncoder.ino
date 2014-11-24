/*
 MIDI Note player
 
 This sketch shows how to use the serial transmit pin (pin 1) to send MIDI Note data.
 If this circuit is connected to a MIDI synth, it will play 
 the Notes F#-0 (0x1E) to F#-5 (0x5A) in sequence.
 
 
 The circuit:
 * digital in 1 connected to MIDI jack pin 5
 * MIDI jack pin 2 connected to ground
 * MIDI jack pin 4 connected to +5V through 220-ohm resistor
 Attach a MIDI cable to the jack, then to a MIDI synth, and play music.
 
 created 13 Jun 2006
 modified 13 Aug 2012
 by Tom Igoe 
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Midi
 
 */

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

const int ButtonPin = 7; 

//#define ENC_A 14
//#define ENC_B 15
#define ENC_PORT PINC //PINC is the bank where pin 14 ad 15 are the first two pins
const int RotaryPin1 = 14;
const int RotaryPin2 = 15;

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack
uint8_t RotaryCodes;
int RotaryValue;
int Misses;

void setup() 
{
  RotaryCodes = 0;
  RotaryValue = 0;
  Misses = 0;
  
  pinMode(ButtonPin, INPUT_PULLUP);
  pinMode(RotaryPin1, INPUT_PULLUP);
  pinMode(RotaryPin2, INPUT_PULLUP);
  
  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);//backlight on
}

void loop() 
{  
  bool ButtonPressed = (LOW==digitalRead(ButtonPin));
  if(ButtonPressed)
  {
    RotaryValue = 0;
  }
    
//  for(int Repeat = 0; Repeat<20; ++Repeat)
  {
    uint8_t CurrentRotaryCode = ( ENC_PORT & 0x03 );  //add current state. mask is 0x00000011 because using ports 14 and 15 on PORTC bus
    if(CurrentRotaryCode!=(RotaryCodes&0x03))
    {//change!
      RotaryCodes = RotaryCodes << 2;
      RotaryCodes |= CurrentRotaryCode;
    
      static int tbl[16] =
      { 0, +1, -1, 0,
        // position 3 = 00 to 11, can't really do anythin, so 0
        -1, 0, 0/*-2*/, +1,
        // position 2 = 01 to 10, assume a bounce, should be 01 -> 00 -> 10
        +1, 0/*+2*/, 0, -1,
        // position 1 = 10 to 01, assume a bounce, should be 10 -> 00 -> 01
        0, -1, +1, 0
        // position 0 = 11 to 10, can't really do anything
      };
      
      uint8_t TblIndex = RotaryCodes & 0x0F;
      int Mult = 1;
    //  uint8_t TblIndex = (RotaryCodes & 0x03) | ((RotaryCodes>>6<<2) & 0x0F);
      if(TblIndex==3 || TblIndex==6 || TblIndex==9 ||TblIndex==12)
      {
        ++Misses;
        ++Mult;
        // try using previous?
        TblIndex = (RotaryCodes>>2) & 0x0F;
        if(TblIndex==3 || TblIndex==6 || TblIndex==9 ||TblIndex==12)
        {
          ++Misses;
          ++Mult; 
          TblIndex = (RotaryCodes>>4) & 0x0F;
        }
      }
      int Mod = tbl[TblIndex]*Mult;
      RotaryValue += Mod;  
    }
  }
  
  // display update
  lcd.home (); // set cursor to 0,0
  lcd.print("Btn ");  
  lcd.print(ButtonPressed?1:0);
  
//  ++RotaryValue;
  lcd.print(" Val ");
  lcd.print(RotaryValue);
  lcd.print(" ");
  lcd.print(" ");
  
  lcd.setCursor(0,1);
  lcd.print(RotaryCodes, HEX);
  lcd.setCursor(3,1);
  lcd.print(Misses);
  
}


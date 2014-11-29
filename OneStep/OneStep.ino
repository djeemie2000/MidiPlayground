#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "StepController.h"
#include <MidiSerial.h>

#define ENC_PORT PIND //PIND is the bank where pin 2 and 3 are on
const int EncoderPinA = 2;
const int EncoderPinB = 3;
const int InteruptPin = EncoderPinA;
const int PressButtonPin = 5;

const int OctaveLedPin = 8;
const int NoteLedPin = 9;
const int VelocityLedPin = 10;
const int DurationLedPin = 11;
const int TempoLedPin = 12;

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack
volatile uint32_t RotaryCode;
volatile int Position;

COneStepController Controller;
CMidiSerial MidiSerial;

void setup() 
{
  RotaryCode = 0x00;
  Position = 0;
   
  pinMode(PressButtonPin, INPUT_PULLUP);
  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);
  
  pinMode(OctaveLedPin, OUTPUT);
  pinMode(NoteLedPin, OUTPUT);
  pinMode(VelocityLedPin, OUTPUT);
  pinMode(DurationLedPin, OUTPUT);
  pinMode(TempoLedPin, OUTPUT);
  
  //interupt handling for int.0 = pin2
  attachInterrupt(0, OnInterupt, CHANGE);
  
  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);//backlight on
}

void loop() 
{  
  bool ButtonPressed = (LOW==digitalRead(PressButtonPin));
  
  unsigned long TimeStamp = millis();
  Controller.Update(Position, ButtonPressed, TimeStamp);
   
  // update leds
  int EditSelection = Controller.GetEditMode();
  digitalWrite(OctaveLedPin, EditSelection==0 ? HIGH : LOW);
  digitalWrite(NoteLedPin, EditSelection==1 ? HIGH : LOW);
  digitalWrite(VelocityLedPin, EditSelection==2 ? HIGH : LOW);
  digitalWrite(DurationLedPin, EditSelection==3 ? HIGH : LOW);
  digitalWrite(TempoLedPin, EditSelection==4 ? HIGH : LOW);

  // display update
  lcd.setCursor(0,0); // set cursor to 0,0
  lcd.print("P ");
  lcd.print(Position);
  lcd.print("      ");
  
  lcd.setCursor(0, 1);
  lcd.print(Controller.GetCurrentNote());
  lcd.print(Controller.GetCurrentOctave());
  lcd.print(" V");
  lcd.print(Controller.GetCurrentVelocity(), DEC);
  lcd.print(" D");
  lcd.print(Controller.GetCurrentDuration(), DEC);
  // tempo
  lcd.print(" ");
  lcd.print(Controller.GetCurrentTempoBpm(), DEC);
}

void OnInterupt()
{
  // read rotary
  delayMicroseconds(500);// wait for bounces to stabilize. choose a delay that is not too long, and not too small
  uint8_t CurrentRotaryCode = ( ENC_PORT >> 2 & 0x03  );    
  if(CurrentRotaryCode != (RotaryCode & 0x000003))
  {
    if( CurrentRotaryCode == 0x03)// || CurrentRotaryCode == 0x00 )
    {
      ++Position;
    }
    else if(CurrentRotaryCode == 0x01)// || CurrentRotaryCode == 0x01)
    {
      --Position;
    }
    RotaryCode = (RotaryCode << 4) | CurrentRotaryCode;
  }
}


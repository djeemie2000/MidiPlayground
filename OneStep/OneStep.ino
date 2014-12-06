//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "StepController.h"
#include <MidiSerial.h>

#define ENC_PORT PIND //PIND is the bank where pin 2 and 3 are on
const int EncoderPinA = 2;
const int EncoderPinB = 3;
const int InteruptPin = EncoderPinA;
const int EditModeButtonPin = 4;

const int SelectStepButtonPin = 5;

const int OctaveLedPin = 8;
const int NoteLedPin = 9;
const int VelocityLedPin = 10;
const int DurationLedPin = 11;
const int ActiveLedPin = 6;
const int TempoLedPin = 12;

const int TempoPotPin = A0;

volatile uint32_t RotaryCode;
volatile int Position;

COneStepController Controller;

void setup() 
{
  RotaryCode = 0x00;
  Position = 0;
   
  pinMode(EditModeButtonPin, INPUT_PULLUP);
  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);

  pinMode(SelectStepButtonPin, INPUT_PULLUP);
  
  pinMode(TempoPotPin, INPUT);
  
  pinMode(OctaveLedPin, OUTPUT);
  pinMode(NoteLedPin, OUTPUT);
  pinMode(VelocityLedPin, OUTPUT);
  pinMode(DurationLedPin, OUTPUT);
  pinMode(ActiveLedPin, OUTPUT);
  pinMode(TempoLedPin, OUTPUT);
  
  //interupt handling for int.0 = pin2
  attachInterrupt(0, OnInterupt, CHANGE);

  Controller.Begin();
}

void loop() 
{  
  bool EditModeButtonPressed = (LOW==digitalRead(EditModeButtonPin));
  bool SelectStepButtonPressed = (LOW==digitalRead(SelectStepButtonPin));  
  int TempoPotValue = analogRead(TempoPotPin);
  unsigned long TimeStamp = millis();
  int Action = Controller.Update(Position, EditModeButtonPressed, SelectStepButtonPressed, TimeStamp);
   
  // update leds
  int EditSelection = Controller.GetEditMode();
  digitalWrite(OctaveLedPin, EditSelection==0 ? HIGH : LOW);
  digitalWrite(NoteLedPin, EditSelection==1 ? HIGH : LOW);
  digitalWrite(VelocityLedPin, EditSelection==2 ? HIGH : LOW);
  digitalWrite(DurationLedPin, EditSelection==3 ? HIGH : LOW);
  digitalWrite(ActiveLedPin, EditSelection==4 ? HIGH : LOW);
 
  if(Action==1)
  { // note on
    digitalWrite(TempoLedPin, HIGH);
  }
  else if(Action == -1)
  { // note off
    digitalWrite(TempoLedPin, LOW);
  }
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


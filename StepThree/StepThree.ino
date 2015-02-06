//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <MidiSerial.h>

#include "RotaryEncoder.h"
#include "ShiftRegisterOutputs.h"
#include "CCapacitiveTouchPad.h"
#include "StepController.h"

const int Encoder1PinA = 2;
const int Encoder1PinB = 3;
const int Encoder2PinA = 4;
const int Encoder2PinB = 5;
const int Encoder3PinA = 6;
const int Encoder3PinB = 7;

const int OutputLatchPin = 10;
const int OutputClockPin = 11;
const int OutputDataPin = 12;

const int TempoPotPin = A0;
const int TouchPadIrgPin = 8;


CRotaryEncoder Encoder1;
CRotaryEncoder Encoder2;
CRotaryEncoder Encoder3;
CShiftRegisterOutputs StepOutputs;//TODO led matrix
COneStepController Controller;
CCapacitiveTouchPad TouchPad;

void setup()
{
  Encoder1.Begin(Encoder1PinA, Encoder1PinB);
  Encoder2.Begin(Encoder2PinA, Encoder2PinB);
  Encoder3.Begin(Encoder3PinA, Encoder3PinB);

  StepOutputs.Begin(OutputLatchPin, OutputClockPin, OutputDataPin);

  TouchPad.Begin(TouchPadIrgPin);

  Controller.Begin();
  
  delay(5000);//show startup screen for a while
}

void loop() 
{  
  Encoder1.Read();
  Encoder2.Read();
  Encoder3.Read();

  TouchPad.Read();
  bool EditModeButtonPressed = TouchPad.Get(0);
  bool SelectStepButtonPressed = TouchPad.Get(4);

  //int TempoPotValue = analogRead(TempoPotPin);
  unsigned long TimeStamp = millis();

  Controller.Update(Encoder1.GetPosition(), Encoder2.GetPosition(), Encoder3.GetPosition(),
                    EditModeButtonPressed,
                    SelectStepButtonPressed,
                    TimeStamp);
   
  // update leds
  // TODO use led matrix
  for(int Step = 0; Step<COneStepController::NumSteps; ++Step)
  {
      StepOutputs.Set(Step, Controller.GetStepState(Step));
  }
  StepOutputs.Write();
}

//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <MidiSerial.h>

#include "RotaryEncoder.h"
#include "ShiftRegisterOutputs.h"
#include "CapacitiveTouchPad.h"
#include "LedControlMS.h"
#include "StepController.h"

const int Encoder1PinA = 2;
const int Encoder1PinB = 3;
const int Encoder2PinA = 4;
const int Encoder2PinB = 5;
const int Encoder3PinA = 6;
const int Encoder3PinB = 7;

const int LedLoadPin = 10;
const int LedClockPin = 11;
const int LedDataPin = 12;

const int TempoPotPin = A0;
const int TouchPadIrgPin = 8;


CRotaryEncoder Encoder1;
CRotaryEncoder Encoder2;
CRotaryEncoder Encoder3;
CShiftRegisterOutputs StepOutputs;//TODO led matrix
COneStepController Controller;
CCapacitiveTouchPad TouchPad;
/*
 pin 12 is connected to the DataIn
 pin 11 is connected to the CLK
 pin 10 is connected to LOAD
 We have only 1 MAX72XX.
 */
LedControl LedMatrix(LedDataPin, LedClockPin, LedLoadPin,1);


void setup()
{
  Encoder1.Begin(Encoder1PinA, Encoder1PinB);
  Encoder2.Begin(Encoder2PinA, Encoder2PinB);
  Encoder3.Begin(Encoder3PinA, Encoder3PinB);

  TouchPad.Begin(TouchPadIrgPin);

  LedMatrix.shutdown(0,false);
  LedMatrix.clearAll();
  LedMatrix.setIntensity(0, 1);

  Controller.Begin();
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
   
  // update leds on ledmatrix
  const int EditLedRow = 0;
  const int IntervalLedRow = 2;
  const int ActiveLedRow = 3;
  const int PlayingLedRow = 7;
  for(int Step = 0; Step<COneStepController::NumSteps; ++Step)
  {
      LedMatrix.setLed(0, EditLedRow, Step, Controller.GetStepEdit(Step));
      LedMatrix.setLed(0, IntervalLedRow, Step, Controller.GetStepInInterval(Step));
      LedMatrix.setLed(0, ActiveLedRow, Step, Controller.GetStepActive(Step));
      LedMatrix.setLed(0, PlayingLedRow, Step, Controller.GetStepState(Step));
  }
}

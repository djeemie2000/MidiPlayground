//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <MidiSerial.h>
#include <LedControlMS.h>
#include "CapacitiveTouchPad.h"
#include "Controller.h"

const int TouchPadIrgPin = 8;

const int LedLoadPin = 10;
const int LedClockPin = 11;
const int LedDataPin = 12;


CController Controller;
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
  TouchPad.Begin(TouchPadIrgPin);

  Controller.Begin();

  LedMatrix.shutdown(0,false);
  LedMatrix.clearAll();
  LedMatrix.setIntensity(0, 1);
}

void loop() 
{
  TouchPad.Read();

  bool Value[CController::NumTriggers];
  for(int Pad = 0; Pad<CController::NumTriggers; ++Pad)
  {
      Value[Pad] = TouchPad.Get(Pad);
  }

  Controller.Update(Value);

  // current touchpad status to 3x4 led matrix:
  // 3 7 11
  // 2 6 10
  // 1 5 9
  // 0 4 8
  for(int Pad = 0; Pad<CController::NumTriggers; ++Pad)
  {
      int y = Pad%4;
      int x = Pad/4;
      LedMatrix.setLed(0, 2*x, 2*y, Value[Pad]);
      LedMatrix.setLed(0, 2*x, 2*y+1, Value[Pad]);
      LedMatrix.setLed(0, 2*x+1, 2*y, Value[Pad]);
      LedMatrix.setLed(0, 2*x+1, 2*y+1, Value[Pad]);
  }
  
  delay(2);// less delay??
}

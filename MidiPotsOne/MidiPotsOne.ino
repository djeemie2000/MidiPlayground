//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <MidiSerial.h>

#include "CapacitiveTouchPad.h"
#include "Controller.h"

const int AnalogInPin1 = A0;
const int AnalogInPin2 = A1;
const int AnalogInPin3 = A2;
const int AnalogInPin4 = A3;

const int TouchPadIrgPin = 8;


CController Controller;
CCapacitiveTouchPad TouchPad;

void setup()
{
  pinMode(AnalogInPin1, INPUT);
  pinMode(AnalogInPin2, INPUT);
  pinMode(AnalogInPin3, INPUT);
  pinMode(AnalogInPin4, INPUT);

  TouchPad.Begin(TouchPadIrgPin);

  Controller.Begin();
}

void loop() 
{
  int Value[4];
  Value[0] = analogRead(AnalogInPin1);
  Value[1] = analogRead(AnalogInPin2);
  Value[2] = analogRead(AnalogInPin3);
  Value[3] = analogRead(AnalogInPin4);

  TouchPad.Read();
  bool AssignButtonPressed = TouchPad.Get(4);

  Controller.Update(Value, 4, AssignButtonPressed);
  
  delay(2);
}

//Note: apparently, need to include here all library related stuff that is used in other files of the project
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <MidiSerial.h>

#include "RotaryEncoder.h"
#include "ShiftRegisterOutputs.h"
#include "StepController.h"

const int Encoder1PinA = 2;
const int Encoder1PinB = 3;
const int Encoder2PinA = 4;
const int Encoder2PinB = 5;
const int Encoder3PinA = 6;
const int Encoder3PinB = 7;

const int EditModeButtonPin = 8;
const int SelectStepButtonPin = 9;

const int OutputLatchPin = 10;
const int OutputClockPin = 11;
const int OutputDataPin = 12;

const int TempoPotPin = A0;


CRotaryEncoder Encoder1;
CRotaryEncoder Encoder2;
CRotaryEncoder Encoder3;
CShiftRegisterOutputs StepOutputs;
COneStepController Controller;


void setup()
{   
  pinMode(EditModeButtonPin, INPUT_PULLUP);
  pinMode(SelectStepButtonPin, INPUT_PULLUP);
  
  pinMode(TempoPotPin, INPUT);
  
  Encoder1.Begin(Encoder1PinA, Encoder1PinB);
  Encoder2.Begin(Encoder2PinA, Encoder2PinB);
  Encoder3.Begin(Encoder3PinA, Encoder3PinB);

  StepOutputs.Begin(OutputLatchPin, OutputClockPin, OutputDataPin);

  Controller.Begin();
  
  delay(5000);//show startup screen for a while
}

void loop() 
{  
  Encoder1.Read();
  Encoder2.Read();
  Encoder3.Read();
  bool EditModeButtonPressed = (LOW==digitalRead(EditModeButtonPin));
  bool SelectStepButtonPressed = (LOW==digitalRead(SelectStepButtonPin));  
  //int TempoPotValue = analogRead(TempoPotPin);
  unsigned long TimeStamp = millis();

  Controller.Update(Encoder1.GetPosition(), Encoder2.GetPosition(), Encoder3.GetPosition(),
                    EditModeButtonPressed,
                    SelectStepButtonPressed,
                    TimeStamp);
   
  // update leds
  for(int Step = 0; Step<COneStepController::NumSteps; ++Step)
  {
      StepOutputs.Set(Step, Controller.GetStepState(Step));
  }
  StepOutputs.Write();
}

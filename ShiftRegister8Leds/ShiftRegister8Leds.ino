//**************************************************************//
//****************************************************************

#include "ShiftRegisterOutputs.h"

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

CShiftRegisterOutputs Outputs;


void setup() 
{
  
  Outputs.Begin(latchPin, clockPin, dataPin);
  //set pins to output so you can control the shift register
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);
}

void loop() 
{
  for (int numberToDisplay = 0; numberToDisplay < 8; numberToDisplay++) 
  {
    Outputs.Set(numberToDisplay, true);
    Outputs.Write();
    // pause before next value:
    delay(500);
  }
  
  for (int numberToDisplay = 0; numberToDisplay < 8; numberToDisplay++) 
  {
    Outputs.Set(numberToDisplay, false);
    Outputs.Write();
    // pause before next value:
    delay(500);
  }

}

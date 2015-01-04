//shift register experiment
//coded by zxlee
//iamzxlee.wordpress.com

#include "ShiftRegisterInputs.h"
  
const int EnablePin = 2;
const int LoadPin = 3;
const int ClockPin = 4;
const int DataPin = 5;
 
CShiftRegisterInputs Inputs;

void setup()
{
  Inputs.Begin(EnablePin, LoadPin, ClockPin, DataPin);
  Serial.begin(9600);
}
 
void loop()
{
  Inputs.Read();

  for(int i=7;i>=0;i--)
  {
    if(Inputs.Get(i))
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
  }
  Serial.println();
 
 
   delay(250);
}


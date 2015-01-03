#include "RotaryEncoder.h"

const int PinA = 2;
const int PinB = 3;

CRotaryEncoder Encoder;
int PrevChangeCount;

void setup()
{
  Encoder.Begin(PinA, PinB);
  Serial.begin(115200);
  PrevChangeCount = 0;
}

void PrintState()
{  
  Serial.print((Encoder.GetRotaryHistory()/4)%4);
  Serial.print(" ");
  Serial.print(Encoder.GetRotaryHistory()%4);
  Serial.print(" #");
  Serial.print(Encoder.GetChangeCount());
  Serial.print(" -> ");
  Serial.println(Encoder.GetPosition());
}

void loop()
{
  Encoder.Read();

  if(PrevChangeCount != Encoder.GetChangeCount())
  {
    PrevChangeCount = Encoder.GetChangeCount();
    PrintState();
  }
  delay(5);//will this prevent proper operation?
  // 10 mSec will sometimes cause skips
  // 5 mSec or below will give good performance
  // read more often => better performance
}


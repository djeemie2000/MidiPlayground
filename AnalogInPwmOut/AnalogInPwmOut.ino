#include "TimerOne.h"

const int AnalogInPin1 = A0;
const int PwmOutPin = 10;//must be 9 or 10

unsigned int g_DebugCounter;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x04);
}


void setup()
{
  pinMode(AnalogInPin1, INPUT);
  pinMode(PwmOutPin, OUTPUT);
  
  Serial.begin(115200);
  
  setupFastAnalogRead();

  Timer1.initialize(50);// ?? 20 kHz ?? 

  g_DebugCounter = 0;
}


void loop()
{
  int Value = analogRead(AnalogInPin1); //10 bits!
  Timer1.pwm(PwmOutPin, Value);// 10 bits!

  ++g_DebugCounter;
//  if(g_DebugCounter%16000==0)
//  {
//    Serial.print(g_DebugCounter);
//    Serial.print(" ");
//    Serial.print(millis());
//    Serial.print(" ");
//    Serial.println(Value);
//  }
}



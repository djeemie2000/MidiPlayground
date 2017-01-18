#include "werkstatt.h"

const int AnalogInPin1 = A0;
const int NoiseOutPin = 4;

unsigned long g_DebugCounter;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x04);
}

noise g_Noise(NoiseOutPin); 

void setup() 
{
  setupFastAnalogRead();

  Serial.begin(115200);
  Serial.println("BitNoise...");
}

void Debug()
{
  // for debugging / timing purposes
  ++g_DebugCounter;
  unsigned long Interval = 1024*16;
  if(Interval<g_DebugCounter)
  {
    Serial.println();
    Serial.println(millis());
    Serial.println(g_DebugCounter);
    g_DebugCounter = 0;
  }
}

void loop() 
{
  int Color1 = analogRead(AnalogInPin1);
  g_Noise.generate(Color1); 
  
  //Debug();
}


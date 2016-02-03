#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"

const int AnalogInPin = A0;

void TestAccuracy()
{
  Serial.println("Test accuracy...");
  for(int Value = 0; Value<4096; Value += 64)
  {
     mcp48dac::SetOutput(Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);
    delay(1);//needed?
    int ValueIn = analogRead(AnalogInPin);
    Serial.print("Out= ");
    Serial.print(Value);
    Serial.print(" In= ");
    Serial.println(ValueIn);
    delay(400);
  }
}

void TestSpeed()
{
  Serial.println("Test speed...");
  unsigned long Before = millis();
  const long NumRepeats = 40000;
  for(long Value = 0; Value<NumRepeats; ++Value)
  {
   mcp48dac::SetOutput(Value%4096, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  Serial.print(NumRepeats);
  Serial.print(" x = ");
  Serial.print(Duration);
  Serial.println(" mSec");
  
}

void TestSaw()
{
  for(int Value = 0; Value<4096; Value+=1)
  {
    mcp48_setOutput(Value);
  }
}

void WriteDac()
{
  static int DacValue = 0;
  DacValue = (DacValue + 16)%4096;
  mcp48_setOutput(DacValue);
}

void TestSawInterrupt()
{
  const int SamplingFrequency = 40000;
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Serial.print("Fs=");
  Serial.print(SamplingFrequency);
  Serial.print(" Period=");
  Serial.print(PeriodMicroSeconds);
  Serial.println(" uSec");
  
  Serial.println("Starting..");
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
  delay(5000);
  Timer1.detachInterrupt();
  delay(1000);
}

void TestAccuracy_NewApi()
{
  Serial.println("Test accuracy new API...");
  for(int Value = 64; Value<4096; Value += 128)
  {
   mcp48dac::SetOutput(Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);
   mcp48dac::SetOutput(Value, mcp48dac::Channel_B, mcp48dac::Gain_x2, 0);   

   //mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 1);
   //mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_B, mcp48dac::Gain_x2, 1);   

   Serial.print("Out= ");
    Serial.println(Value);
    delay(500);
  }
}


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("Uno mcp4822 test...");

  //analogReadResolution(12);
  
  mcp48dac::Begin(1);
  delay(100);
}

void loop() 
{
  TestSpeed();
  TestAccuracy_NewApi();
}


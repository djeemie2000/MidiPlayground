#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "TimerOne.h"

const int AnalogInPin = A0;

void TestAccuracy()
{
  Serial.println("Test accuracy...");
  for(int Value = 0; Value<4096; Value += 64)
  {
    mcp48_setOutput(Value);
    //mcp48_setOutput(0, GAIN_1, 1, Value);
    delay(1);//???
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
  const int NumRepeats = 40000;
  for(int Value = 0; Value<NumRepeats; ++Value)
  {
    mcp48_setOutput(Value%4096);
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

   mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 1);
   mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_B, mcp48dac::Gain_x2, 1);   

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
  Serial.println("Teensy mcp4822 test...");

  analogReadResolution(12);
  
  //mcp48_begin();  
  //mcp48_setOutput(0);
  mcp48dac::Begin(2);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  //TestSpeed();
  TestAccuracy_NewApi();
  //delay(2000);
  //TestSaw();
  //TestSawInterrupt();
}


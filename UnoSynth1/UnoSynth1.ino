#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"

const int SamplingFrequency = 32000;

int g_DacValue;
int g_InterruptCounter;

void WriteDac()
{
  mcp48_setOutput(g_DacValue);
  
  g_DacValue += 32;
  if(4096<=g_DacValue)
  {
    g_DacValue = 0;
  }

  ++g_InterruptCounter;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("MCP4822 test 2...");

  mcp48_begin();

  g_DacValue = 0;
  g_InterruptCounter = 0;

  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency; 
  Serial.print("Fs=");
  Serial.print(SamplingFrequency);
  Serial.print(" Period=");
  Serial.print(PeriodMicroSeconds);
  Serial.println(" uSec");
  
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
}

void TestAccuracy()
{
  //high-res saw wave
  for (unsigned int DacValue = 0; DacValue < 4096; DacValue += 32)
  {
    mcp48_setOutput(0, GAIN_1, 1, DacValue);
    //mcp48_setOutput(DacValue);

    int ValueA = analogRead(A0);
    int ValueB = analogRead(A1);
    Serial.print("DAC out=");
    Serial.print(DacValue);
    Serial.print(" ADC in=");
    Serial.print(ValueA);
    Serial.print(" ");
    Serial.print(ValueB);
    Serial.println();

    delay(250);
  }
}

void TestSpeed()
{
  unsigned long Before = millis();

  //high-res saw wave
  for (int Repeat = 0; Repeat < 10; ++Repeat)
  {
    for (unsigned int DacValue = 0; DacValue < 4096; DacValue += 1)
    {
      //mcp48_setOutput(0, GAIN_1, 1, DacValue);
      mcp48_setOutput(DacValue);
    }
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;
  unsigned long Number = 4096 * 10;

  Serial.print("DAC out x ");
  Serial.print(Number);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void TestSaw()
{
  Serial.println("Saw wave out...");
  //high-res saw wave
  while (true)
  {
    for(int Increase = 2; Increase < 16; Increase += 2)
    {
      int NumRepeats = 30 * Increase;
      for(int Repeat = 0; Repeat < NumRepeats; Repeat++)
      {
        for (unsigned int DacValue = 0; DacValue < 4096; DacValue += Increase)
        {
          //mcp48_setOutput(0, GAIN_1, 1, DacValue);
          mcp48_setOutput(DacValue);
        }
      }
    }
    Serial.println(millis());
  }
}

void TestSquare()
{ 
  Serial.println("Square wave out...");
  //high-res square wave
  while (true)
  {
    for(int Increase = 2; Increase < 16; Increase += 2)
    {
      int NumRepeats = 30 * Increase;
      for(int Repeat = 0; Repeat < NumRepeats; Repeat++)
      {
        for (unsigned int DacValue = 0; DacValue < 4096; DacValue += Increase)
        {
          mcp48_setOutput(0, GAIN_1, 1, DacValue<2048?0:4095);
          //mcp48_setOutput(DacValue);
        }
      }
    }
  }
}


void loop()
{
    //TestAccuracy();
    //TestSpeed();
  //  delay(500);
  //TestSaw();
  //TestSquare();
  if(SamplingFrequency<=g_InterruptCounter)
  {
    int Tmp = g_InterruptCounter;
    g_InterruptCounter = 0;
    Serial.print(Tmp);
    Serial.print(" x t=");
    Serial.println(millis());
  }
//  delay(10);
}


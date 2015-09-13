#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "IntMultiSaw.h"

const int SamplingFrequency = 32000;

int g_DacValue;
int g_InterruptCounter;
const int DacResolution = 12;
const int NumSaws = 4;
CIntMultiSaw<int, NumSaws, DacResolution> g_MultiSaw;

int CalcDacValue()
{
  return 2048 + g_MultiSaw();

  //  g_DacValue += 32;
  //  if(4096<=g_DacValue)
  //  {
  //    g_DacValue = 0;
  //  }
  //
  //  return g_DacValue;
}

void WriteDac()
{
  mcp48_setOutput( CalcDacValue() );
  ++g_InterruptCounter;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("MCP4822 test 2...");

  mcp48_begin();

  g_DacValue = 0;
  g_InterruptCounter = 0;

  uint64_t FreqHz = 100;
  uint64_t FreqMilliHz = FreqHz * 1000;
  g_MultiSaw.SetFrequency(SamplingFrequency, FreqMilliHz);
  for (int idx = 0; idx < NumSaws; ++idx)
  {
    g_MultiSaw.SetPhaseShift(0, idx);
  }

  // debugging:
  //  TestCalcDacValue();

  TestMcpSpeed();
  TestCalcDacValueSpeed();
  TestCalcPhaseSpeed();
  delay(1000);
  
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Serial.print("Fs=");
  Serial.print(SamplingFrequency);
  Serial.print(" Period=");
  Serial.print(PeriodMicroSeconds);
  Serial.println(" uSec");


  Serial.println("Starting...");
  Timer1.initialize(PeriodMicroSeconds);
  //Timer1.attachInterrupt(WriteDac);
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

void TestMcpSpeed()
{
  unsigned long Before = millis();

  int DacValue = 2048;
  for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    //mcp48_setOutput(0, GAIN_1, 1, DacValue);
    mcp48_setOutput(DacValue);
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;

  Serial.print("DAC out x ");
  Serial.print(SamplingFrequency);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void TestCalcDacValueSpeed()
{
  unsigned long Before = millis();

  int DacValue = 2048;
  for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    CalcDacValue();
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;

  Serial.print("DAC calc x ");
  Serial.print(SamplingFrequency);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void TestCalcPhaseSpeed()
{
  unsigned long Before = millis();

  CIntegerPhaseGenerator<int, 12> PhaseGen;
  unsigned long FreqHz = 100;
  unsigned long FreqMilliHz = FreqHz*1000;
  PhaseGen.SetFrequency(SamplingFrequency, FreqMilliHz);

  int Shift[] = { 15, 145, 105, 123, 15, 19, 32, 33 };
  
  int Tmp = 0;
  for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    PhaseGen.Update();
//    for(int idx = 0; idx<4; ++idx)
//    {
//      Tmp += PhaseGen.Shifted(Shift[idx]);
//    }
    int Tmp2 = ( PhaseGen.Shifted(*(Shift+0)) 
                + PhaseGen.Shifted(*(Shift+1)) 
                + PhaseGen.Shifted(*(Shift+2)) 
                + PhaseGen.Shifted(*(Shift+3))
                + PhaseGen.Shifted(*(Shift+4)) 
                + PhaseGen.Shifted(*(Shift+5)) 
                + PhaseGen.Shifted(*(Shift+6)) 
                + PhaseGen.Shifted(*(Shift+7)) 
                );
    Tmp += Tmp2>>3;
  }
  Serial.println(Tmp);

  unsigned long After = millis();
  unsigned long Duration = After - Before;

  Serial.print("DAC calc x ");
  Serial.print(SamplingFrequency);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}


void TestCalcDacValue()
{
  Serial.print("CalcDacValue test...");
  for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    int DacValue = CalcDacValue();
    Serial.println(DacValue);
    delay(500);
  }
}


void loop()
{
  // basic LFO triangular
//  static int LoopCounter = 0;
//  static int Incr = 1;
//  int PhaseShift = LoopCounter;
//  for (int idx = 0; idx < NumSaws; ++idx)
//  {
//    g_MultiSaw.SetPhaseShift((PhaseShift*idx)%2048, idx);
//  }
//  if(1024<LoopCounter)
//  {
//    Incr = -Incr;
//    LoopCounter = 0;
//  }
//  LoopCounter += Incr;
//  delay(1); //??
//  
  if (SamplingFrequency <= g_InterruptCounter)
  {
    int Tmp = g_InterruptCounter;
    g_InterruptCounter = 0;
    Serial.print(Tmp);
    Serial.print(" x t=");
    Serial.println(millis());

    //uint64_t FreqHz = 55 * (1+(LoopCounter%4));
    //uint64_t FreqMilliHz = FreqHz * 1000;
    //g_MultiSaw.SetFrequency(SamplingFrequency, FreqMilliHz);
  }

}


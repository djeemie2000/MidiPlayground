#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "IntMultiSaw.h"

const int SamplingFrequency = 26000;//32000;

int g_DacValue;
int g_InterruptCounter;
const int DacResolution = 12;
const int NumSaws = 8;
CIntMultiSaw8<int, DacResolution> g_MultiSaw;

CIntegerPhaseGenerator<int, 12> g_Phase;
int g_PhaseShift[NumSaws];

int CalcDacValue()
{
  return 2048 + g_MultiSaw();
}

int CalcDacValue2()
{
  g_Phase.Update();
  int Tmp = ( g_Phase.Shifted(*(g_PhaseShift+0)) 
                + g_Phase.Shifted(*(g_PhaseShift+1)) 
                + g_Phase.Shifted(*(g_PhaseShift+2)) 
                + g_Phase.Shifted(*(g_PhaseShift+3))
                + g_Phase.Shifted(*(g_PhaseShift+4)) 
                + g_Phase.Shifted(*(g_PhaseShift+5)) 
                + g_Phase.Shifted(*(g_PhaseShift+6)) 
                + g_Phase.Shifted(*(g_PhaseShift+7)) 
                ) >> 3;
  return 2048 + Tmp;
}


void WriteDac()
{
  mcp48_setOutput( CalcDacValue() );
  ++g_InterruptCounter;
}

//------------------

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

void LogSpeedTest(int Repeats, unsigned long Before, unsigned long After)
{
  Serial.print("Repeat x ");
  Serial.print(Repeats);
  Serial.print("  = ");
  unsigned long Duration = After-Before;
  Serial.print(Duration);
  Serial.println(" mSec");
}

void TestMcpSpeed()
{
  Serial.println("Test mcp speed..");
  unsigned long Before = millis();
  int DacValue = 2048;
  for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    //mcp48_setOutput(0, GAIN_1, 1, DacValue);
    mcp48_setOutput(DacValue);
  }
  unsigned long After = millis();
  LogSpeedTest(SamplingFrequency, Before, After);
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
  LogSpeedTest(SamplingFrequency, Before, After);
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
    Tmp += CalcDacValue2();
//    PhaseGen.Update();
//    int Tmp2 = ( PhaseGen.Shifted(*(Shift+0))
//                + PhaseGen.Shifted(*(Shift+1))
//                + PhaseGen.Shifted(*(Shift+2))
//                + PhaseGen.Shifted(*(Shift+3))
//                + PhaseGen.Shifted(*(Shift+4))
//                + PhaseGen.Shifted(*(Shift+5))
//                + PhaseGen.Shifted(*(Shift+6))
//                + PhaseGen.Shifted(*(Shift+7))
//                );
//    Tmp += Tmp2>>3;
  }
  Serial.println(Tmp);

  unsigned long After = millis();
  LogSpeedTest(SamplingFrequency, Before, After);
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

//------------------

void setup()
{
  Serial.begin(115200);
  Serial.println("UnoSynth1...");

  mcp48_begin();

  g_DacValue = 0;
  g_InterruptCounter = 0;

  uint64_t FreqHz = 110;
  uint64_t FreqMilliHz = FreqHz * 1000;
  g_MultiSaw.SetFrequency(SamplingFrequency, FreqMilliHz);
  g_Phase.SetFrequency(SamplingFrequency, FreqMilliHz);
  for (int idx = 0; idx < NumSaws; ++idx)
  {
    g_MultiSaw.SetPhaseShift(0, idx);
    g_PhaseShift[idx] = 0;
  }

  // debugging:
  //  TestCalcDacValue();

  TestCalcPhaseSpeed();
  TestMcpSpeed();
  TestCalcDacValueSpeed();
  delay(1000);
  
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Serial.print("Fs=");
  Serial.print(SamplingFrequency);
  Serial.print(" Period=");
  Serial.print(PeriodMicroSeconds);
  Serial.println(" uSec");

  Serial.println("Starting...");
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
}

void loop()
{
  // basic LFO triangular
  static int LoopCounter = 0;
  static int Incr = 1;
  int PhaseShift = LoopCounter;
  for (int idx = 2; idx < NumSaws; ++idx)
  {
    int Sign = idx%2 ? 1 : -1;
    int Shift = ((PhaseShift + 120*idx) % 4096) - 2048;
    g_MultiSaw.SetPhaseShift(Sign*Shift, idx);
    g_PhaseShift[idx] = Sign*Shift;
  }
  LoopCounter += Incr;
  delay(3); //??
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


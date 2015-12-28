#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "IntSimpleOscillator.h"

const int SamplingFrequency = 32000;

int g_InterruptCounter;
const int DacResolution = 12;
isl::CSimpleOscillator<DacResolution> g_Oscillator(SamplingFrequency);

int ClampUnsigned12(int In)
{
  if(In<0)
  {
    return 0;
  }

  if(4096<=In)
  {
    return 4095;
  }
  
  return In;
}

int CalcDacValue()
{
  return ClampUnsigned12( 2048 + g_Oscillator() );
}

void WriteDac()
{
  mcp48_setOutput( CalcDacValue() );
  ++g_InterruptCounter;
}

//------------------

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
 
  for(int WaveForm = 0; WaveForm<8; ++WaveForm)
  {
    Serial.print("Speed test Waveform ");
    Serial.println(WaveForm);
    g_Oscillator.SelectOperator(WaveForm);
    unsigned long Before = millis();
    for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
    {
      CalcDacValue();
    }
    unsigned long After = millis();
    LogSpeedTest(SamplingFrequency, Before, After);
  }
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
  Serial.println("UnoSynth2...");

  mcp48_begin();

  g_InterruptCounter = 0;

  uint64_t FreqHz = 220;
  uint64_t FreqMilliHz = FreqHz * 1000;
  g_Oscillator.SetFrequency(FreqMilliHz);

  // debugging:
  g_Oscillator.SelectOperator(1);
  //  TestCalcDacValue();//might change waveform, ...

  TestMcpSpeed();
  //TestCalcDacValueSpeed();
  delay(1000);
  
  Serial.println("Starting...");
  g_Oscillator.SelectOperator(1);
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
}

void loop()
{
  if (SamplingFrequency <= g_InterruptCounter)
  {
    int Tmp = g_InterruptCounter;
    g_InterruptCounter = 0;
    Serial.print(Tmp);
    Serial.print(" x t=");
    Serial.println(millis());

    static int Cntr = 0;
    static int Wveform = 0;
    
    ++Cntr;
    uint64_t FreqHz = 80 * (1+(Cntr%6));
    uint64_t FreqMilliHz = FreqHz * 1000;
    Serial.print("Freq=");
    Serial.println(int(FreqHz));
    g_Oscillator.SetFrequency(FreqMilliHz);

    if(0==Cntr%6)
    {
      ++Wveform;
      Serial.print("WaveForm ");
      Serial.println(Wveform%7);
      g_Oscillator.SelectOperator(Wveform%7);
    }
  }

}


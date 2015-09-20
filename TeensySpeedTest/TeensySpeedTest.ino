#include <cstdint>
#include "IntSimpleOscillator.h"

const int SamplingFrequency = 32000;

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

void TestDacOutSpeed()
{
    Serial.println("Speed test DAC out ");
    unsigned long Before = millis();
    for (int Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
    {
      analogWrite(A14, Repeat%4096);
    }
    unsigned long After = millis();
    LogSpeedTest(SamplingFrequency, Before, After);
}


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);//??
  
  Serial.println("Teensy speed test...");  
  pinMode(13, OUTPUT);
  analogWriteResolution(12);

  uint64_t FreqHz = 220;
  uint64_t FreqMilliHz = FreqHz * 1000;
  g_Oscillator.SetFrequency(FreqMilliHz);

  TestCalcDacValueSpeed();
  delay(1000);

  TestDacOutSpeed();
  delay(1000);
  
  Serial.println("Starting...");


}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  Serial.println("HelloWorld");
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  
}

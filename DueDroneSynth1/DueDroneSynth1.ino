#include "DueTimer.h"
#include "Oscillator.h"
#include "FrequencyMultipliers.h"

int OutPin = 53;

const int SamplingFrequency = 20000;



const int NumOscillators = 4;

COscillator Oscillator[NumOscillators];

void myHandler()
{
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    Oscillator[idx].Update();
  }
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency*1000/FrequencyMilliHz;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("DroneSynth...");

  analogReadResolution(10);

  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    int OscOutPin = OutPin-2*idx;
    Oscillator[idx].Begin(OscOutPin);
    Oscillator[idx].SetPeriod(CalcPeriod(250000));//250 Hz
    int GateInPin = OscOutPin-1;
    pinMode(GateInPin, INPUT);
    int CVInPin = A0 + 1 + idx;
    pinMode(CVInPin, INPUT);
  }

  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000*1000/SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds);
}

void loop()
{
  const int BaseFrequencyMilliHz = 80 * 1000; // base frequency, detune allows for 2 octaves below the base freq
  for(int Repeat = 0; Repeat<500; ++Repeat)
  {
    //
      int CVInPin = A0;
      int Value = (analogRead(CVInPin)/4)%256;
      int FreqMult = FrequencyMultipliers_4x64[Value];
      int OscillatorFrequencyMilliHz = BaseFrequencyMilliHz * FreqMult / FrequencyMultiplierScale;

      // debug info
      if(Repeat==0)
      {
        Serial.print("global: ");
        Serial.print(Value);
        Serial.print(" -> ");
        Serial.print(FreqMult);
        Serial.print(" -> ");
        Serial.print(OscillatorFrequencyMilliHz);
        Serial.println();
      }


    for(int idx = 0; idx<NumOscillators; ++idx)
    {
      //
      int GateInPin = OutPin-2*idx-1;
      int Amplitude = digitalRead(GateInPin);
      Oscillator[idx].SetAmplitude(Amplitude);
      //
      int DetuneCVInPin = A0 + 1 + idx;
      int DetuneValue = analogRead(DetuneCVInPin)%1024;
      int DetuneMult = FrequencyMultipliers_4x256_signed[DetuneValue];
      int DetunedOscillatorFrequencyMilliHz = OscillatorFrequencyMilliHz * DetuneMult / FrequencyMultiplierScale;
      //
      Oscillator[idx].SetPeriod(CalcPeriod(DetunedOscillatorFrequencyMilliHz));

      // debug info
      if(Repeat==0)
      {
        Serial.print("detune ");
        Serial.print(idx);
        Serial.print(": ");
        Serial.print(DetuneValue);
        Serial.print(" -> ");
        Serial.print(DetuneMult);
        Serial.print(" -> ");
        Serial.print(DetunedOscillatorFrequencyMilliHz);
        Serial.println();
      }

    }

    delay(1);//??
  }
}

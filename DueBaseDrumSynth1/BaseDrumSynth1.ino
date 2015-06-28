#include <DueTimer.h>

int OutPin = 53;

const int SamplingFrequency = 20000;

int Period = 1;
int Phase = 0;

void UpdateOscillator(int& _Phase, int _Period, int Pin)
{
  _Phase++;
  _Phase = _Phase<_Period ? _Phase : 0;
  int OutputOn = _Phase<128*_Period/256 ? HIGH : LOW;
  digitalWrite(Pin, OutputOn);   
}

void myHandler()
{
  UpdateOscillator(Phase, Period, OutPin);
}

void SetFrequency(int Frequency)
{
  Period = SamplingFrequency/Frequency;
}

void setup()
{
  SetFrequency(250);  
  pinMode(OutPin, OUTPUT);

  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000*1000/SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds); 
}

void loop()
{
  for(int Frequency = 120; 30<Frequency; Frequency=126*Frequency/128)
  {
    SetFrequency(Frequency);
    delay(7);
  }  
}


#include <DueTimer.h>

int OutPin = 53;

const int SamplingFrequency = 20000;

class COscillator
{
  public:
    COscillator(int Pin);

    void Begin();
    void SetPeriod(int Period);
    void Update();
  private:
    int m_Pin;
    int m_Phase;
    int m_Period;
};

COscillator::COscillator(int Pin)
 : m_Pin(Pin)
 , m_Phase(0)
 , m_Period(1) 
{
}

void COscillator::Begin()
{
    pinMode(m_Pin, OUTPUT);
}

void COscillator::SetPeriod(int Period)
{
  m_Period = Period;
}

void COscillator::Update()
{
  m_Phase++;
  m_Phase = m_Phase<m_Period ? m_Phase : 0;
  int OutputOn = m_Phase<m_Period/2 ? HIGH : LOW;
  digitalWrite(m_Pin, OutputOn); 
}

COscillator Oscillator(OutPin);
COscillator Oscillator2(OutPin-2);

void myHandler()
{
  Oscillator.Update();
  Oscillator2.Update();
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency*1000/FrequencyMilliHz;
}

void setup()
{
  Oscillator.Begin();
  Oscillator2.Begin();
  Oscillator.SetPeriod(CalcPeriod(250000));
  Oscillator2.SetPeriod(CalcPeriod(250000));
  
  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000*1000/SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds); 
}

void loop()
{
  for(int FrequencyHz = 60; FrequencyHz<61; ++FrequencyHz)
  {
    Oscillator.SetPeriod(CalcPeriod(FrequencyHz*1000));
    Oscillator2.SetPeriod(CalcPeriod(2*FrequencyHz*1000+1));
    delay(1000);
  }  
}


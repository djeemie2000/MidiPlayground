#include <DueTimer.h>

int OutPin = 53;

const int SamplingFrequency = 20000;

class COscillator
{
  public:
    COscillator();

    void Begin(int Pin);
    void SetPeriod(int Period);
    void Update();
  private:
    int m_Pin;
    int m_Phase;
    int m_Period;
};

COscillator::COscillator()
 : m_Pin(-1)
 , m_Phase(0)
 , m_Period(1) 
{
}

void COscillator::Begin(int Pin)
{
    m_Pin = Pin;
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
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    Oscillator[idx].Begin(OutPin-2*idx);
    Oscillator[idx].SetPeriod(CalcPeriod(250000));
  }
  
  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000*1000/SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds); 
}

void loop()
{
  int FrequencyHz = 60;
  for(int DetuneMilliHz = 1; DetuneMilliHz <= 2000; DetuneMilliHz+= 50)
  {
    for(int idx = 0; idx<NumOscillators; ++idx)
    {
      Oscillator[idx].SetPeriod(CalcPeriod((1+idx/2)*FrequencyHz*1000+idx*DetuneMilliHz));
    }
    delay(1000);
  }  
}


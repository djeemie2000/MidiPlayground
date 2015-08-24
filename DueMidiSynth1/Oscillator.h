#pragma once

class COscillator
{
  public:
    COscillator();

    void Begin(int Pin);
    void SetPeriod(int Period);
    void SetAmplitude(int Amplitude);
    void Update();
  private:
    int m_Pin;
    int m_Phase;
    int m_Period;
    int m_Amplitude;// 0 or 1
};

COscillator::COscillator()
 : m_Pin(-1)
 , m_Phase(0)
 , m_Period(1)
 , m_Amplitude(1)
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

void COscillator::SetAmplitude(int Amplitude)
{
  m_Amplitude = Amplitude;
}

void COscillator::Update()
{
  m_Phase++;
  m_Phase = m_Phase<m_Period ? m_Phase : 0;
  int OutputOn = (0<m_Amplitude && m_Phase<m_Period/2) ? HIGH : LOW;
  digitalWrite(m_Pin, OutputOn);
}

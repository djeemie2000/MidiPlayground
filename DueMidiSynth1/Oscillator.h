#pragma once

class COscillator
{
  public:
    COscillator();

    void Begin(int Pin);
    void SetPeriod(int Period);
    void SetAmplitude(int Amplitude);
    void SetPulseWidth(int PulseWidth);//range [1,255]
    void Update();
  private:
    int m_Pin;
    int m_Phase;
    int m_Period;
    int m_Amplitude;// 0 or 1
    int m_PulseWidth;
};

COscillator::COscillator()
 : m_Pin(-1)
 , m_Phase(0)
 , m_Period(1)
 , m_Amplitude(1)
 , m_PulseWidth(128)
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

void COscillator::SetPulseWidth(int PulseWidth)
{
  m_PulseWidth = PulseWidth;
}

void COscillator::Update()
{
  m_Phase++;
  m_Phase = m_Phase<m_Period ? m_Phase : 0;
  int OutputOn = (0<m_Amplitude && m_Phase<m_PulseWidth*m_Period/256) ? HIGH : LOW;
  digitalWrite(m_Pin, OutputOn);
}

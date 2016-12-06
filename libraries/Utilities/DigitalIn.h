#pragma once

class CDigitalIn
{
  public:
    CDigitalIn()
    : m_Pin(0)
    , m_State(false)
    , m_PrevState(false)
    {}

    void Begin(int Pin)
    {
      m_Pin = Pin;
      pinMode(m_Pin, INPUT);
    }

    bool Read()
    {
      m_PrevState = m_State;
      m_State = HIGH==digitalRead(m_Pin);
      //Serial.print(m_Pin);
      //Serial.println(m_State?1:0);
    }

    bool IsHigh() const
    {
      return m_State;
    }

    bool IsLow() const
    {
      return !m_State;
    }
    
    bool IsRising() const
    {
      return !m_PrevState && m_State;
    }
    
    bool IsFalling() const
    {
      return m_PrevState && !m_State;
    }

    bool IsChanging() const
    {
        return m_PrevState != m_State;
    }
    
  private:
    int m_Pin;
    bool m_State;
    bool m_PrevState;
};



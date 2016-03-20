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
      pinMode(m_Pin, INPUT_PULLUP);
    }

    bool Read()
    {
      m_PrevState = m_State;
      m_State = LOW==digitalRead(m_Pin);
      //Serial.print(m_Pin);
      //Serial.println(m_State?1:0);
    }

    bool IsOn() const
    {
      return m_State;
    }
    
    bool IsPressed() const
    {
      return !m_PrevState && m_State;
    }
    
    bool IsReleased() const
    {
      return m_PrevState && !m_State;
    }
    
  private:
    int m_Pin;
    bool m_State;
    bool m_PrevState;
};



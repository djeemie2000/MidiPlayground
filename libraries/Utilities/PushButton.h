#pragma once

class CPushButton
{
  public:
    CPushButton()
    : m_Pin(0)
    , m_PressedState(HIGH)
    , m_State(false)
    , m_PrevState(false)
    , m_Millis(0)
    , m_PrevMillis(0)
    , m_ChangedMillis(0)
    {}

    void Begin(int Pin, int PressedState, unsigned long Millis)
    {
      m_Pin = Pin;
      m_PressedState = PressedState;
      pinMode(m_Pin, INPUT);//?? INPUT_PULLUP?
      m_Millis = Millis;
      m_PrevMillis = Millis;
      m_ChangedMillis = Millis;
    }

    bool Read(unsigned long Millis)
    {
      m_PrevState = m_State;
      m_State = m_PressedState==digitalRead(m_Pin);
      m_PrevMillis = m_Millis;
      m_Millis = Millis;
      if(m_State != m_PrevState)
      {
          m_ChangedMillis = Millis;
      }
      //Serial.print(m_Pin);
      //Serial.println(m_State?1:0);
    }

    bool IsPushed() const
    {
      return m_State;
    }

    bool IsClicked() const
    {
      return !m_PrevState && m_State;
    }
    
    bool IsReleased() const
    {
      return m_PrevState && !m_State;
    }

    bool IsChanging() const
    {
        return m_PrevState != m_State;
    }

    bool LongPushed(unsigned long Duration) const
    {
        return m_State && (m_ChangedMillis+Duration<m_Millis);
    }

    bool LongClicked(unsigned long Duration) const
    {
        return m_State && (m_ChangedMillis+Duration<m_Millis) && !(m_ChangedMillis+Duration<m_PrevMillis);
    }
    
  private:
    int m_Pin;
    int m_PressedState;
    bool m_State;
    bool m_PrevState;
    unsigned long m_Millis;
    unsigned long m_PrevMillis;
    unsigned long m_ChangedMillis;
};



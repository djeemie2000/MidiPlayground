#include "RotaryEncoder.h"
#include <Arduino.h>

CRotaryEncoder::CRotaryEncoder()
 : m_PinA(-1)
 , m_PinB(-1)
 , m_RotaryHistory(0)
 , m_Position(0)
 , m_ChangeCount(0)
{
}

void CRotaryEncoder::Begin(int PinA, int PinB)
{
    m_PinA = PinA;
    m_PinB = PinB;

    pinMode(m_PinA, INPUT);
    pinMode(m_PinB, INPUT);
}

void CRotaryEncoder::Read()
{
    unsigned int NewRotaryValue = 2*(1-digitalRead(m_PinA)) + (1-digitalRead(m_PinB));
    unsigned int PrevRotaryValue = m_RotaryHistory%4;
    if(NewRotaryValue != PrevRotaryValue)
    {
      // changed
      ++m_ChangeCount;

      if(!NewRotaryValue)
      {
        // new pos => increase or decrease?
        if(PrevRotaryValue == 2)
        {
          ++m_Position;
        }
        else if(PrevRotaryValue == 1)
        {
          --m_Position;
        }
        else if(PrevRotaryValue == 3)
        {
          // skipped one
          unsigned int PrevPrevRotaryValue = (m_RotaryHistory/4)%4;
          if(PrevPrevRotaryValue == 1)
          {
            ++m_Position;
          }
          else if(PrevPrevRotaryValue == 2)
          {
            --m_Position;
          }
        }
      }

      m_RotaryHistory = 4*m_RotaryHistory + NewRotaryValue;
    }
}

int CRotaryEncoder::GetPosition() const
{
    return m_Position;
}

void CRotaryEncoder::Reset()
{
    m_Position = 0;
    m_RotaryHistory = 0;
    m_ChangeCount = 0;
}

int CRotaryEncoder::GetChangeCount() const
{
    return m_ChangeCount;
}

unsigned int CRotaryEncoder::GetRotaryHistory() const
{
    return m_ChangeCount;
}

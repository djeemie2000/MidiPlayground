#pragma once

namespace isl
{

template<class T, int Scale>
class CIntegerAHREnvelope
{
public:
  CIntegerAHREnvelope()
   : m_ScaledValue(0)
   , m_State(NoneState)
   , m_AttackSlope(InternalMax)
   , m_ReleaseSlope(InternalMax)
  {}

  void SetAttack(T SamplingFrequency, T AttackMilliSeconds)
  {
    T AttackTimeSamples = SamplingFrequency*AttackMilliSeconds/1000;
    m_AttackSlope = InternalMax/AttackTimeSamples;
  }

  void SetRelease(T SamplingFrequency, T ReleaseMilliSeconds)
  {
    T ReleaseTimeSamples = SamplingFrequency*ReleaseMilliSeconds/1000;
    m_ReleaseSlope = InternalMax/ReleaseTimeSamples;
  }

  void NoteOn()
  {
    m_State = AttackState;
  }

  void NoteOff()
  {
    m_State = ReleaseState;
  }

  T operator()()
  {
    if(m_State == AttackState)
    {
      m_ScaledValue += m_AttackSlope;
      if(InternalMax<m_ScaledValue)
      {
        m_ScaledValue = InternalMax;
        m_State = HoldState;
      }
    }
    else if(m_State == ReleaseState)
    {
      m_ScaledValue -= m_ReleaseSlope;
      if(m_ScaledValue<0)
      {
        m_ScaledValue = 0;
        m_State = NoneState;
      }
    }
    return m_ScaledValue>>(InternalScale-Scale);
  }

private:
  static const int InternalScale = 30;
  static const int InternalMax = 1<<InternalScale;

  static const int NoneState = 0;
  static const int AttackState = 1;
  static const int HoldState = 2;
  static const int ReleaseState = 3;

  T   m_ScaledValue;
  int m_State;// none A H R
  T   m_AttackSlope;
  T   m_ReleaseSlope;
};

}

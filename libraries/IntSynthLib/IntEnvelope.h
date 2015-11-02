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

/*
 * attack-only envelope.
 * when maximum is reached, holds indefinitely until next note on is triggered
 */
template<class T, int Scale>
class CAEnvelope
{
public:

  CAEnvelope()
   : m_Value(MaxValue)
   , m_AttackSlope(MaxValue)
  {}

  void SetSlope( T Slope)
  {
      if(0<Slope && Slope<MaxValue)
      {
          m_AttackSlope = Slope;
      }
  }

  T CalcSlopeUpscaled(uint64_t SamplingFrequency, T AttackMilliSeconds)
  {
      uint64_t Mult = MaxValue;
      uint64_t Slope = 0<AttackMilliSeconds ? 1000 * Mult / (SamplingFrequency * AttackMilliSeconds) : MaxValue;
      return Slope;
  }

  void SetAttack(uint64_t SamplingFrequency, T AttackMilliSeconds)
  {
      m_AttackSlope = CalcSlopeUpscaled(SamplingFrequency, AttackMilliSeconds);
  }

  void NoteOn()
  {
    m_Value = 0;
  }

  void NoteOff()
  {
    //ignored
  }

  T operator()(T In)
  {
    T Amplification = m_Value;
    if(m_Value<MaxValue-m_AttackSlope)
    {
        m_Value += m_AttackSlope;
    }
    else
    {
        m_Value = MaxValue;
    }

    return ((Amplification>>Rescale)*In)>>Scale;
  }

private:
  static const int MaxNumBits = sizeof(T)*8-2;//in case of signed
  static const int Rescale = MaxNumBits - Scale;
  static const int MaxValue = 1<<MaxNumBits;

  T   m_Value;
  T   m_AttackSlope;
};

}

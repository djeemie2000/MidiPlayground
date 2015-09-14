#pragma once

#include "IntPhaseGenerator.h"

namespace isl
{

template<class T, int NumSaws, int Scale>
class CIntMultiSaw
{
public:
    CIntMultiSaw()
     : m_Phase(0)
    {
        for(int idx = 0; idx<NumSaws; ++idx)
        {
            m_PhaseShift[idx] = 0;
        }
    }

    void SetFrequency(uint64_t SamplingFrequency, uint64_t FrequencyMilliHz)
    {
        m_Phase.SetFrequency(SamplingFrequency, FrequencyMilliHz);
    }

    void SetPhaseShift(T PhaseShift, int Index)
    {
        m_PhaseShift[Index] = PhaseShift;
    }

    T operator()()
    {
        m_Phase();

        T Out = m_Phase.Shifted(m_PhaseShift[0]);
        for(int idx = 1; idx<NumSaws; ++idx)
        {
            Out += m_Phase.Shifted(m_PhaseShift[idx]);
        }
        return Out/NumSaws;
    }

    T operator()(T* PhaseShift)
    {
        m_Phase();
        T Out = 0;
        for(int idx = 0; idx<NumSaws; ++idx)
        {
            Out += m_Phase.Shifted(PhaseShift[idx]);
        }
        return Out/NumSaws;
    }


private:
    CIntegerPhaseGenerator<T, Scale> m_Phase;
    T m_PhaseShift[NumSaws];
};


template<class T, int Scale>
class CIntMultiSaw8
{
public:
  static const int NumSaws = 8;

    CIntMultiSaw8()
     : m_Phase(0)
    {
        for(int idx = 0; idx<NumSaws; ++idx)
        {
            m_PhaseShift[idx] = 0;
        }
    }

    void SetFrequency(uint64_t SamplingFrequency, uint64_t FrequencyMilliHz)
    {
        m_Phase.SetFrequency(SamplingFrequency, FrequencyMilliHz);
    }

    void SetPhaseShift(T PhaseShift, int Index)
    {
        m_PhaseShift[Index] = PhaseShift;
    }

    T operator()()
    {
        m_Phase.Update();

        return ( m_Phase.Shifted(*m_PhaseShift)
                + m_Phase.Shifted(m_PhaseShift[1])
                + m_Phase.Shifted(m_PhaseShift[2])
                + m_Phase.Shifted(m_PhaseShift[3])
                + m_Phase.Shifted(m_PhaseShift[4])
                + m_Phase.Shifted(m_PhaseShift[5])
                + m_Phase.Shifted(m_PhaseShift[6])
                + m_Phase.Shifted(m_PhaseShift[7]) ) >> 3;
    }

    T operator()(T* PhaseShift)
    {
      m_Phase.Update();

      return ( m_Phase.Shifted(PhaseShift[0])
              + m_Phase.Shifted(PhaseShift[1])
              + m_Phase.Shifted(PhaseShift[2])
              + m_Phase.Shifted(PhaseShift[3])
              + m_Phase.Shifted(PhaseShift[4])
              + m_Phase.Shifted(PhaseShift[5])
              + m_Phase.Shifted(PhaseShift[6])
              + m_Phase.Shifted(PhaseShift[7]) ) >> 3;
    }


private:
    CIntegerPhaseGenerator<T, Scale> m_Phase;
    T m_PhaseShift[NumSaws];
};

}

#pragma once

#include "IntPhaseGenerator.h"

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

        T Out = 0;
        for(int idx = 0; idx<NumSaws; ++idx)
        {
            Out += m_Phase.Shifted(m_PhaseShift[idx]);
            //Serial.print(idx);
            //Serial.print(" ");
            //Serial.print(m_PhaseShift[idx]);
            //Serial.print(" ");
            //Serial.print(Out);
            //Serial.println();
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

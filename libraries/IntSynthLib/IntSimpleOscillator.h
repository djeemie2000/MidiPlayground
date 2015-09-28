#pragma once

#include "IntPhaseGenerator.h"
#include "IntOperators.h"
#include "IntOperatorFactory.h"

namespace isl
{

template<int Scale>
class CSimpleOscillator
{
public:
    CSimpleOscillator()
        : m_SamplingFrequency(40000)
        , m_PhaseGen(0)
        , m_CurrentOperator()
    {
        SelectOperator(0);
    }

    CSimpleOscillator(uint64_t SamplingFrequency)
        : m_SamplingFrequency(SamplingFrequency)
        , m_PhaseGen(0)
        , m_CurrentOperator()
    {
        SelectOperator(0);
    }

    void SetSamplingFrequency(uint64_t SamplingFrequency)
    {
        m_SamplingFrequency = SamplingFrequency;
    }

    void SelectOperator(int Selected)
    {
        m_CurrentOperator = COperatorFactory<Scale>::GetOperator(Selected);
    }

    void Sync()
    {
        m_PhaseGen.Sync();
    }

    void SetFrequency(uint64_t FrequencyMilliHz)
    {
        m_PhaseGen.SetFrequency(m_SamplingFrequency, FrequencyMilliHz);
    }

    int operator()()
    {
        return m_CurrentOperator(m_PhaseGen());
    }

private:
    uint64_t m_SamplingFrequency;
    CIntegerPhaseGenerator<int, Scale> m_PhaseGen;
    IntOperator m_CurrentOperator;
};

}//namespace isl

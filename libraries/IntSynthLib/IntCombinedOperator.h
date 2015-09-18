#pragma once

#include "IntConversions.h"
#include "IntOperators.h"
#include "IntPhaseGenerator.h"
#include "IntCombinors.h"

namespace isl
{

template<int Scale>
class CIntCombinedOperator
{
public:
    CIntCombinedOperator(uint64_t SamplingFrequency)
        : m_SamplingFrequencyHz(SamplingFrequency)
        , m_PhaseGenA()
        , m_PhaseGenB()
        , m_OperatorA( IntTriangle<Scale> )
        , m_OperatorB( IntTriangle<Scale> )
        , m_Combinor( IntHardLimitAdd<int, Scale> )
        , m_FrequencyMilliHz(110*1000)
        , m_FrequencyMultiplierValue(1<<8)
        , m_FrequencyMultiplierScale(8)
    {
    }

    void Sync()
    {
        m_PhaseGenA.Sync();
        m_PhaseGenB.Sync();
    }

    void SetFrequency(uint64_t FrequencyMilliHz)
    {
        m_FrequencyMilliHz = FrequencyMilliHz;

        UpdateFrequency();
    }

    void SetFrequencyMultiplier(int Multiplier, int MultiplierScale)
    {
        m_FrequencyMultiplierValue = Multiplier;
        m_FrequencyMultiplierScale = MultiplierScale;

        UpdateFrequency();
    }

    void SelectOperatorA(int Idx)
    {

    }

    void SelectOperatorB(int Idx)
    {

    }

    void SelectCombinor(int Idx)
    {

    }

    int operator()()
    {
        return m_Combinor(m_OperatorA(m_PhaseGenA()), m_OperatorB(m_PhaseGenB()));
    }

private:
    void UpdateFrequency()
    {
        m_PhaseGenA.SetFrequency(m_SamplingFrequencyHz, m_FrequencyMilliHz);
        m_PhaseGenB.SetFrequency(m_SamplingFrequencyHz, m_FrequencyMilliHz*m_FrequencyMultiplierValue >> m_FrequencyMultiplierScale);
    }

    const uint64_t m_SamplingFrequencyHz;

    CIntegerPhaseGenerator<int, Scale> m_PhaseGenA;
    CIntegerPhaseGenerator<int, Scale> m_PhaseGenB;
    IntOperator m_OperatorA;
    IntOperator m_OperatorB;
    IntCombinor m_Combinor;

    int m_FrequencyMultiplierValue;
    int m_FrequencyMultiplierScale;
    uint64_t m_FrequencyMilliHz;
};

}//namespace isl

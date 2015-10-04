#pragma once

#include "IntConversions.h"
#include "IntOperators.h"
#include "IntPhaseGenerator.h"
#include "IntCombinors.h"
#include "IntCombinorFactory.h"

namespace isl
{

template<int Scale>
class CIntCombinedOperator
{
public:
    static const int NumOperators = 8;

    CIntCombinedOperator(uint64_t SamplingFrequency)
        : m_SamplingFrequencyHz(SamplingFrequency)
        , m_PhaseGenA()
        , m_PhaseGenB()
        , m_PhaseGenC()
        , m_OperatorA( GetOperator(0) )
        , m_OperatorB( GetOperator(0) )
        , m_OperatorC( GetOperator(0) )
        , m_Combinor1( IntAdd<int, Scale> )
        , m_Combinor2( IntAdd<int, Scale> )
        , m_FrequencyMilliHz(110*1000)
        , m_FrequencyMultiplierValueA(1<<8)
        , m_FrequencyMultiplierScaleA(8)
        , m_FrequencyMultiplierValueB(1<<8)
        , m_FrequencyMultiplierScaleB(8)
        , m_FrequencyMultiplierValueC(1<<8)
        , m_FrequencyMultiplierScaleC(8)
        , m_AmplitudeValueA(1<<8)
        , m_AmplitudeScaleA(8)
        , m_AmplitudeValueB(1<<8)
        , m_AmplitudeScaleB(8)
        , m_AmplitudeValueC(1<<8)
        , m_AmplitudeScaleC(8)
    {
        UpdateFrequency();
        SelectCombinor1(0);
        SelectCombinor2(0);
        SelectOperatorA(0);
        SelectOperatorB(0);
        SelectOperatorC(0);
    }

CIntCombinedOperator()
        : m_SamplingFrequencyHz(40000)
        , m_PhaseGenA()
        , m_PhaseGenB()
        , m_PhaseGenC()
        , m_OperatorA( GetOperator(0) )
        , m_OperatorB( GetOperator(0) )
        , m_OperatorC( GetOperator(0) )
        , m_Combinor1( IntAdd<int, Scale> )
        , m_Combinor2( IntAdd<int, Scale> )
        , m_FrequencyMilliHz(110*1000)
        , m_FrequencyMultiplierValueA(1<<8)
        , m_FrequencyMultiplierScaleA(8)
        , m_FrequencyMultiplierValueB(1<<8)
        , m_FrequencyMultiplierScaleB(8)
        , m_FrequencyMultiplierValueC(1<<8)
        , m_FrequencyMultiplierScaleC(8)
        , m_AmplitudeValueA(1<<8)
        , m_AmplitudeScaleA(8)
        , m_AmplitudeValueB(1<<8)
        , m_AmplitudeScaleB(8)
        , m_AmplitudeValueC(1<<8)
        , m_AmplitudeScaleC(8)
    {
        UpdateFrequency();
        SelectCombinor1(0);
        SelectCombinor2(0);
        SelectOperatorA(0);
        SelectOperatorB(0);
        SelectOperatorC(0);
    }

    void SetSamplingFrequency(uint64_t SamplingFrequency)
    {
	m_SamplingFrequencyHz = SamplingFrequency;
    }

    void Sync()
    {
        m_PhaseGenA.Sync();
        m_PhaseGenB.Sync();
        m_PhaseGenC.Sync();
    }

    void SetFrequency(uint64_t FrequencyMilliHz)
    {
        m_FrequencyMilliHz = FrequencyMilliHz;

        UpdateFrequency();
    }

    void SetFrequencyMultiplierA(int Multiplier, int MultiplierScale)
    {
        m_FrequencyMultiplierValueA = Multiplier;
        m_FrequencyMultiplierScaleA = MultiplierScale;

        m_PhaseGenA.SetFrequency(m_SamplingFrequencyHz,
                                 m_FrequencyMilliHz*m_FrequencyMultiplierValueA >> m_FrequencyMultiplierScaleA);
    }

    void SetFrequencyMultiplierB(int Multiplier, int MultiplierScale)
    {
        m_FrequencyMultiplierValueB = Multiplier;
        m_FrequencyMultiplierScaleB = MultiplierScale;

        m_PhaseGenB.SetFrequency(m_SamplingFrequencyHz,
                                 m_FrequencyMilliHz*m_FrequencyMultiplierValueB >> m_FrequencyMultiplierScaleB);
    }

    void SetFrequencyMultiplierC(int Multiplier, int MultiplierScale)
    {
        m_FrequencyMultiplierValueC = Multiplier;
        m_FrequencyMultiplierScaleC = MultiplierScale;

        m_PhaseGenC.SetFrequency(m_SamplingFrequencyHz,
                                 m_FrequencyMilliHz*m_FrequencyMultiplierValueC >> m_FrequencyMultiplierScaleC);
    }

    void SetAmplitudeA(int Amplitude, int AmplitudeScale)
    {
        m_AmplitudeValueA = Amplitude;
        m_AmplitudeScaleA = AmplitudeScale;
    }

    void SetAmplitudeB(int Amplitude, int AmplitudeScale)
    {
        m_AmplitudeValueB = Amplitude;
        m_AmplitudeScaleB = AmplitudeScale;
    }

    void SetAmplitudeC(int Amplitude, int AmplitudeScale)
    {
        m_AmplitudeValueC = Amplitude;
        m_AmplitudeScaleC = AmplitudeScale;
    }

    void SelectOperatorA(int Idx)
    {
        m_OperatorA = GetOperator(Idx);
    }

    void SelectOperatorB(int Idx)
    {
        m_OperatorB = GetOperator(Idx);
    }

    void SelectOperatorC(int Idx)
    {
        m_OperatorC = GetOperator(Idx);
    }

    void SelectCombinor1(int Idx)
    {
        m_Combinor1 = CCombinorFactory<Scale>::GetOperator(Idx);
    }

    void SelectCombinor2(int Idx)
    {
        m_Combinor2 = CCombinorFactory<Scale>::GetOperator(Idx);
    }

    int operator()()
    {
        return m_Combinor2( m_Combinor1(m_AmplitudeValueA*m_OperatorA(m_PhaseGenA())>>m_AmplitudeScaleA, m_AmplitudeValueB*m_OperatorB(m_PhaseGenB())>>m_AmplitudeScaleB),
                            m_AmplitudeValueC*m_OperatorC(m_PhaseGenC())>>m_AmplitudeScaleC );
    }

    //static std::vector<std::string> GetOperatorNames()
    //{
    //    return { "Triangle", "Sin", "Sinh", "Saw+", "Saw-", "Pulse+", "Pulse-", "Quadratic" };
    //}

    //static std::vector<std::string> GetCombinorNames()
    //{
    //    return CCombinorFactory<Scale>::AvailableOperatorNames();
    //}

private:
    static IntOperator GetOperator(int Idx)
    {
        if(Idx == 0)
        {
            return IntTriangle<Scale>;
        }
        else if(Idx == 1)
        {
            return IntFullPseudoSin<Scale>;
        }
        else if(Idx == 2)
        {
            return IntSemiPseudoSin<Scale>;
        }
        else if(Idx == 3)
        {
            return IntSawUp<Scale>;
        }
        else if(Idx == 4)
        {
            return IntSawDown<Scale>;
        }
        else if(Idx == 5)
        {
            return IntPulsePos<Scale>;
        }
        else if(Idx == 6)
        {
            return IntPulseNeg<Scale>;
        }
        else if(Idx == 7)
        {
            return IntQuadratic<Scale>;
        }
        return IntNoOp<Scale>;
    }

    void UpdateFrequency()
    {
        m_PhaseGenA.SetFrequency(m_SamplingFrequencyHz, m_FrequencyMilliHz*m_FrequencyMultiplierValueA >> m_FrequencyMultiplierScaleA);
        m_PhaseGenB.SetFrequency(m_SamplingFrequencyHz, m_FrequencyMilliHz*m_FrequencyMultiplierValueB >> m_FrequencyMultiplierScaleB);
        m_PhaseGenC.SetFrequency(m_SamplingFrequencyHz, m_FrequencyMilliHz*m_FrequencyMultiplierValueC >> m_FrequencyMultiplierScaleC);
    }

    uint64_t m_SamplingFrequencyHz;

    CIntegerPhaseGenerator<int, Scale> m_PhaseGenA;
    CIntegerPhaseGenerator<int, Scale> m_PhaseGenB;
    CIntegerPhaseGenerator<int, Scale> m_PhaseGenC;
    IntOperator m_OperatorA;
    IntOperator m_OperatorB;
    IntOperator m_OperatorC;
    IntCombinor m_Combinor1;
    IntCombinor m_Combinor2;

    uint64_t m_FrequencyMilliHz;
    int m_FrequencyMultiplierValueA;
    int m_FrequencyMultiplierScaleA;
    int m_FrequencyMultiplierValueB;
    int m_FrequencyMultiplierScaleB;
    int m_FrequencyMultiplierValueC;
    int m_FrequencyMultiplierScaleC;

    int m_AmplitudeValueA;
    int m_AmplitudeScaleA;
    int m_AmplitudeValueB;
    int m_AmplitudeScaleB;
    int m_AmplitudeValueC;
    int m_AmplitudeScaleC;
};

}//namespace isl

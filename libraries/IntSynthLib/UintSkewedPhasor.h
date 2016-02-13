#pragma once

namespace uisl
{

template<class T, int Scale>
T CalcTriangle(T Phase)
{
    return (Phase & (1<<Scale)) ?  (~Phase) & 0x0FFF :  Phase & 0x0FFF;
}

template<class T, int Scale>
T CalcSawUp(T Phase)
{
    return Phase & 0x0FFF;
}

template<class T, int Scale>
T CalcSawDown(T Phase)
{
    return (~Phase) & 0x0FFF;
}

template<class T, int Scale>
T CalcPosPulse(T Phase)
{
    return (Phase & (1<<Scale)) ?  0 :  (1<<Scale) -1;
}

template<class T, int Scale>
T CalcNegPulse(T Phase)
{
    return (Phase & (1<<Scale)) ?  (1<<Scale) -1 : 0;
}

template<uint32_t OutputScale = 12, uint32_t InternalPhaseScale = 24>
class CSkewedPhasor
{
public:
    CSkewedPhasor()
        : m_Phase(0)
        , m_PhaseIncr0(8)
        , m_PhaseIncr1(8)
    {}

    // Note: phase increase is in InternalPhaseScale
    void SetPhaseIncrease(uint32_t PhaseIncr, uint32_t Skew)
    {
	const uint32_t MaxSkew = 1024;
        m_PhaseIncr0 = (PhaseIncr*MaxSkew)/(2*Skew);
        m_PhaseIncr1 = (PhaseIncr*MaxSkew)/(2*(MaxSkew-Skew));
    }

    uint32_t operator()()
    {
        uint32_t Phase = m_Phase >> PhaseUpScale;
//	uint32_t IncrSelect = (m_Phase & (uint32_t(1)<<InternalPhaseScale)); 
//	uint32_t PhaseIncr = IncrSelect ? m_PhaseIncr1 : m_PhaseIncr0;
	uint32_t PhaseIncr = (Phase & (1<<OutputScale)) ? m_PhaseIncr1 : m_PhaseIncr0;
	m_Phase += PhaseIncr;

	return Phase;

//	Serial.print(m_PhaseIncr0, HEX);
//	Serial.print(" ");
//	Serial.print(m_PhaseIncr1, HEX);
//	Serial.print(" ");
//	Serial.print(IncrSelect, HEX);
//	Serial.print(" ");
//	Serial.print(PhaseIncr, HEX);
//	Serial.print(" ");
//	Serial.print(Phase&0xFFF, DEC);
//	Serial.print(" ");
//	Serial.println(Phase, HEX);

//        return Phase;
    }

    uint32_t UpdateTriangle()
    {
        //uint32_t Phase = m_Phase >> PhaseUpScale;
        //uint32_t Value = (Phase & (1<<OutputScale)) ?  (~Phase) & 0x0FFF :  Phase & 0x0FFF;
        //m_Phase += (m_Phase & (1<<PhaseScale)) ? m_PhaseIncr1 : m_PhaseIncr0;
        //return Value;
        return CalcTriangle<uint32_t, OutputScale>(this->operator()());
    }

//    void DebugLog(uint32_t PhaseIncr, uint32_t Skew)
//    {
//       SetPhaseIncrease(PhaseIncr, Skew);
//
//        // debugging:
//        std::cout << "Skew " << Skew << " -> " << std::endl;
//
//        std::cout << "Incr " << m_PhaseIncr0 << " " << m_PhaseIncr1 << std::endl;
//
//       uint32_t Period = (2<<InternalPhaseScale)/PhaseIncr;
//        uint32_t P1 = Period*Skew/1024;
//        uint32_t P2 = Period*(1024-Skew)/1024;
//        std::cout << P1 << " + " << P2 << " = " << Period << std::endl;
//
//        uint32_t Period1 = (1<<InternalPhaseScale)/m_PhaseIncr0;
//        uint32_t Period2 = (1<<InternalPhaseScale)/m_PhaseIncr1;
//        uint32_t Period12 = Period1 + Period2;
//
//       std::cout << Period1 << " + " << Period2 << " = " << Period12 << " vs " << Period << std::endl;
//    }

     uint32_t GetPhaseIncr0() const { return m_PhaseIncr0;	}
     uint32_t GetPhaseIncr1() const { return m_PhaseIncr1;	}
	

private:
    static const uint32_t PhaseUpScale = InternalPhaseScale-OutputScale;

    uint32_t m_Phase;
    uint32_t m_PhaseIncr0;
    uint32_t m_PhaseIncr1;
};

}//namespace uisl


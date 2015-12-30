#pragma once

namespace isl
{

template<class T, int Scale, int GainScale>
class CWaveFolder
{
public:
    CWaveFolder()
        : m_UnityGain(1<<GainScale)
        , m_MaxValue(1<<Scale)
        , m_RemainderMask(2*m_MaxValue-1)
        , m_Gain(m_UnityGain)
    {}

    // gain should be in [1<<GainScale, ...[
    void SetGain(T Gain)
    {
        if(m_UnityGain<=Gain)
        {
            m_Gain = Gain;
        }
    }

    // Fold should be in [0, 1<<GainScale[
    void SetFold(T Fold)
    {
        if(0<=Fold && Fold<m_UnityGain)
        {
            m_Gain = m_UnityGain*m_UnityGain/(m_UnityGain-Fold);
        }
    }

    T Fold(T In)
    {
        T Tmp = In*m_Gain>>GainScale;
        T Remainder = Tmp & m_RemainderMask;
        T Out = Remainder < m_MaxValue ? Remainder : 2*m_MaxValue-Remainder;
        return Out;
    }

    T operator()(T In)
    {
        return 0<In ? Fold(In) : -Fold(-In);
    }

private:
    const T m_UnityGain;
    const T m_MaxValue;
    const T m_RemainderMask;
    T m_Gain;
};

}

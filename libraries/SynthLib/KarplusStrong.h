#ifndef KARPLUSSTRONG
#define KARPLUSSTRONG

#include "Noise.h"
#include "DelayLine2.h"
#include "Trigger.h"
#include "OnePoleFilter.h"

template<class T>
class CKarplusStrong
{
public:
    CKarplusStrong(int SamplingFrequency, T MinFrequency)
        : m_MinFrequency(MinFrequency)
        , m_SamplingFrequency(SamplingFrequency)
        , m_Period(m_SamplingFrequency/MinFrequency)
        , m_Cntr(0)// not excited
        , m_DelayLine(m_Period, 0)//capacity of delayline determined by lowest frequency
        , m_Trigger()
        , m_LPF()
    {
        m_LPF.SetStages(1);
    }

    T operator()(T Trigger, T Frequency, T Cutoff, T Exite)
    {
        if(m_Trigger.IsTriggerOn(Trigger))
        {
            Excite(Frequency);
        }

        T Period = m_MinFrequency<Frequency ? m_SamplingFrequency/Frequency : m_SamplingFrequency/m_MinFrequency;// 1Hz

        T WriteValue = 0<m_Cntr ? Exite : m_LPF(m_DelayLine.Read(Period), Cutoff);
        m_DelayLine.Write(WriteValue);
        if(m_Cntr)
        {
            --m_Cntr;
        }

        return WriteValue;
    }

    void SetPoles(int Poles)
    {
        m_LPF.SetStages(Poles);
    }

private:
    void Excite(T Frequency)
    {
        m_Period = m_SamplingFrequency/Frequency;
        m_Cntr = m_Period;
    }

    const T m_MinFrequency;
    int m_SamplingFrequency;
    int m_Period;
    int m_Cntr;
    CDelayLine2<T> m_DelayLine;//used as circular buffer
    CTriggerIn<T> m_Trigger;
    CMultiStageFilter<float, COnePoleLowPassFilter<float>, 4> m_LPF;
};

#endif // KARPLUSSTRONG

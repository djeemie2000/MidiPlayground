#pragma once


template<class T, int Scale = 12>
class CIntegerPhaseGenerator
{
public:
  static const int Rescale = 32 - Scale;

    CIntegerPhaseGenerator(T Initial = 0)
        : m_InitialPhase(Initial)
        , m_Phase(Initial)
        , m_PhaseStepUpscaled(1<<Scale)
    {}

    void Sync()
    {
        m_Phase = m_InitialPhase;
    }

    T CalcPhaseStepUpscaled(int SamplingFrequency, int FrequencyMilliHz)
    {
        // float Fs = SamplingFrequency;
        // float F = FrequencyMilliHz / 1000.0f;
        // float Mult = 1<<16;
        // Mult *= Mult;
        // float PhaseStep = F * Mult / Fs;
        uint64_t Fs = SamplingFrequency;
        uint64_t F = FrequencyMilliHz;// / 1000.0f;
        uint64_t Mult = 1<<16;
        Mult *= Mult;
        uint64_t PhaseStep = F * Mult / (1000 * Fs);
        return PhaseStep;
    }

    void SetFrequency(int SamplingFrequency, int FrequencyMilliHz)
    {
        m_PhaseStepUpscaled = CalcPhaseStepUpscaled(SamplingFrequency, FrequencyMilliHz);
    }

    void SetPhaseStep(T PhaseStepUpscaled)
    {
        m_PhaseStepUpscaled = PhaseStepUpscaled;
    }

    T operator()()
    {
        m_Phase += m_PhaseStepUpscaled;
        return m_Phase>>Rescale;//(32-Scale);
    }

    T Shifted(T PhaseShift)
    {
        return ( m_Phase + (PhaseShift<<Rescale) )>>Rescale;
    }

    T GetPhaseStep() const
    {
        return m_PhaseStepUpscaled;
    }

private:
    T m_InitialPhase;
    T m_Phase;
    T m_PhaseStepUpscaled;
};

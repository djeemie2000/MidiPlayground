#pragma once


template<int Scale = 12>
class CIntegerPhaseGenerator
{
public:
    CIntegerPhaseGenerator(int Initial = 0)
        : m_InitialPhase(Initial)
        , m_Phase(Initial)
        , m_PhaseStepUpscaled(1<<Scale)
    {}

    void Sync()
    {
        m_Phase = m_InitialPhase;
    }

    void SetFrequency(int SamplingFrequency, int FrequencyMilliHz)
    {
        float Fs = SamplingFrequency;
        float F = FrequencyMilliHz / 1000.0f;
        float Mult = 1<<16;
        Mult *= Mult;
        float PhaseStep = F * Mult / Fs;
        m_PhaseStepUpscaled = PhaseStep;
    }

    void SetPhaseStep(int PhaseStepUpscaled)
    {
        m_PhaseStepUpscaled = PhaseStepUpscaled;
    }

    int operator()()
    {
        m_Phase += m_PhaseStepUpscaled;
        return m_Phase>>(32-Scale);
    }

    int GetPhaseStep() const
    {
        return m_PhaseStepUpscaled;
    }

private:
    int m_InitialPhase;
    int m_Phase;
    int m_PhaseStepUpscaled;
};

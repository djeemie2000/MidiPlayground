#pragma once

namespace isl
{

template<class T, int Scale = 12>
class CIntegerPhaseGenerator
{
public:
  static const int MaxNumBits = sizeof(T)*8;
  static const int Rescale = MaxNumBits - Scale;

    CIntegerPhaseGenerator(T Initial = 0)
        : m_InitialPhase(Initial)
        , m_Phase(Initial)
        , m_PhaseStepUpscaled(1<<Scale)
    {
    }

    void Sync()
    {
        m_Phase = m_InitialPhase;
    }

    T CalcPhaseStepUpscaled(uint64_t SamplingFrequency, uint64_t FrequencyMilliHz)
    {
//        float Fs = SamplingFrequency;
//        float F = FrequencyMilliHz / 1000.0f;
//        float Mult = T(1)<<16;
//        Mult *= Mult;
//        float PhaseStep = F * Mult / Fs;

        Serial.print("Max=");
        Serial.print(MaxNumBits);
        Serial.print(" Rescale= ");
        Serial.println(Rescale);

        uint64_t Fs = SamplingFrequency;
        uint64_t F = FrequencyMilliHz;
        uint64_t Mult = uint64_t(1)<<MaxNumBits;
        uint64_t PhaseStep = F * Mult / (1000 * Fs);

        Serial.print("Fs=");
        Serial.print(float(Fs));

        Serial.print(" F ");
        Serial.print(float(F));

        Serial.print(" M ");
        Serial.print(float(Mult));

        Serial.print(" PhStep=");
        Serial.println(T(PhaseStep));

        return PhaseStep;
    }

    void SetFrequency(uint64_t SamplingFrequency, uint64_t FrequencyMilliHz)
    {
        m_PhaseStepUpscaled = CalcPhaseStepUpscaled(SamplingFrequency, FrequencyMilliHz);
    }

    void SetPhaseStep(T PhaseStepUpscaled)
    {
        m_PhaseStepUpscaled = PhaseStepUpscaled;
    }

    void Update()
    {
        m_Phase += m_PhaseStepUpscaled;
    }

    T operator()()
    {
        m_Phase += m_PhaseStepUpscaled;
        return m_Phase>>Rescale;
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

}//namespace isl

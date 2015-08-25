#ifndef PHASEACCUMULATOR_H
#define PHASEACCUMULATOR_H

template<class T, int Min = -1, int Max = 1>
class CPhaseAccumulator
{
public:
    static const T MinPhase = static_cast<T>(Min);
    static const T MaxPhase = static_cast<T>(Max);
    static const T PhaseRange = MaxPhase - MinPhase;

    CPhaseAccumulator()
        : m_Phase(MinPhase)
    {
    }

    void Set(const T& Phase)
    {
        m_Phase = Phase;
    }

    void Sync()
    {
        m_Phase = MinPhase;
    }

    void Sync(T Phase)
    {
        m_Phase = MinPhase + Phase;
    }

    T operator()(const T& PhaseStep)
    {
        // PhaseStep can be positive or negative!
        m_Phase += PhaseStep;
        if(MaxPhase < m_Phase)
        {
            m_Phase -= PhaseRange;
        }
        else if(m_Phase<MinPhase)
        {
            m_Phase += PhaseRange;
        }
        return m_Phase;
    }


private:
    T m_Phase;
};


#endif // PHASEACCUMULATOR_H

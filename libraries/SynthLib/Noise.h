#ifndef NOISE_H
#define NOISE_H

//#include <cstdint>

template<class T>
class CNoise
{
public:
    CNoise()
     : m_Multiplier(static_cast<T>(2)/static_cast<T>(UINT32_MAX))
     , m_Random(22222)
    {
    }

    uint32_t Rand()
    {
        m_Random = (m_Random * 196314165) + 907633515;
        return m_Random;
    }

    T operator()()
    {
        m_Random = (m_Random * 196314165) + 907633515;
        return m_Multiplier*m_Random-1;
    }

private:
    // Calculate pseudo-random 32 bit number based on linear congruential method.
    // http://www.musicdsp.org/showone.php?id=59
    // static unsigned long random = 22222;
    // random = (random * 196314165) + 907633515;
    // return (float)random * kTwoOverUlongMax - 1.0f;
    const T m_Multiplier;
    uint32_t m_Random;
};

#endif // NOISE_H

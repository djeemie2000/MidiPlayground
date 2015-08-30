#ifndef DELAYLINE2_H
#define DELAYLINE2_H

#include <vector>

template<class T>
class CDelayLine2
{
public:
    CDelayLine2(int Capacity, T InitialValue)
     : m_Capacity(Capacity)
     , m_Values(Capacity, InitialValue)
     , m_WriteIndex(0)
    {

    }

    void Write(T In)
    {
        ++m_WriteIndex;
        if(m_Capacity<=m_WriteIndex)
        {
            m_WriteIndex = 0;
        }
        m_Values[m_WriteIndex] = In;
    }

    T Read(int Delay)
    {
        //int Idx = (m_WriteIndex - Delay) % (m_Capacity);///??? from owl CircularBuffer.hpp -> does not work!!
        //int Idx = Delay<=m_WriteIndex ? m_WriteIndex-Delay : m_WriteIndex+m_Capacity-Delay;
        int Idx = m_WriteIndex-Delay;
        return m_Values[0<=Idx ? Idx : Idx+m_Capacity];
    }

private:
    const int m_Capacity;
    std::vector<T> m_Values;
    int m_WriteIndex;
};

#endif // DELAYLINE2_H

#pragma once

namespace isl
{

template<class T, int Capacity>
class CDelayLine
{
public:
    CDelayLine()
     : m_Values()
     , m_WriteIndex(0)
    {
        //fill with
        for(int idx =0; idx<Capacity; ++idx)
        {
            m_Values[idx] = 0;//default value
        }
    }

    void Write(T In)
    {
        ++m_WriteIndex;
        if(Capacity<=m_WriteIndex)
        {
            m_WriteIndex = 0;
        }
        m_Values[m_WriteIndex] = In;
    }

    T Read(int Delay)
    {
        int Idx = m_WriteIndex-Delay;
        return m_Values[0<=Idx ? Idx : Idx+Capacity];
    }

private:
    T m_Values[Capacity];
    int m_WriteIndex;
};

}//namespace isl

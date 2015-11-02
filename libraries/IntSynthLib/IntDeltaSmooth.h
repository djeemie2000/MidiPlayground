#pragma once

namespace isl
{

template<class T, T Delta = 1>
class CDeltaSmooth
{
public:
    CDeltaSmooth(T Initial)
        : m_Value(Initial)
    {}

    void Reset(T Value)
    {
        m_Value = Value;
    }

    T operator()(T In)
    {
        if(m_Value<In)
        {
            m_Value += Delta;
        }
        else if(In<m_Value)
        {
            m_Value -= Delta;
        }
        // else m_Value == In

        return m_Value;
    }

private:
    T m_Value;
};

}

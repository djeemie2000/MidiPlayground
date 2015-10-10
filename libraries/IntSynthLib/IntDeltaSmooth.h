#pragma once

namespace isl
{

template<class T>
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
        m_Value += m_Value<In ? 1 : -1;
        return m_Value;
    }

private:
    T m_Value;
};

}

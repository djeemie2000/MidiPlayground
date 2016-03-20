#pragma once

template<class T>
class CBoundedValue
{
public:
    CBoundedValue()
        : m_Min(0)
        , m_Max(0)
        , m_Value(0)
    {}

    void SetInterval(T Min, T Max)
    {
      m_Min = Min;
      m_Max = Max;
      Set(m_Value);
    }

    void Set(int Value)
    {
        m_Value = Value;
        if(m_Value<m_Min)
        {
            m_Value = m_Min;
        }
        else if(m_Max<m_Value)
        {
            m_Value = m_Max;
        }
    }

    void SetChange(T Change)
    {
        Set(m_Value+Change);
    }

    T Get() const
    {
        return m_Value;
    }

    void operator =(T Value)
    {
      Set(Value);
    }

    operator T() const
    {
        return m_Value;
    }

private:
    T m_Min;
    T m_Max;
    T m_Value;
};


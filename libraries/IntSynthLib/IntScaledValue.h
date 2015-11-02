#pragma once

namespace isl
{

template<class T, int Scale>
class CScaledValue
{
public:
    explicit CScaledValue(T Value)
        : m_Value(Value)
    {}

    void SetValue(T Value)// sets (internal) value (regardless of scale)
    {
        m_Value = Value;
    }

    void ChangeValue(T Change)// changes (internal) value (regardless of scale)
    {
        m_Value += Change;
    }

    operator T() const
    {
        return m_Value >> Scale;
    }

    T Value() const { return m_Value; }

private:
    T m_Value;
};

template<class T, class U, int Scale>
T operator*(U Val1, const CScaledValue<T, Scale>& Val2)
{
    return (Val1*Val2.Value())>>Scale;
}

template<class T, class U, int Scale>
T operator*(const CScaledValue<T, Scale>& Val2, U Val1)
{
    return (Val1*Val2.Value())>>Scale;
}

template<class T, int Scale1, int Scale2>
CScaledValue<T, Scale1+Scale2> operator*(const CScaledValue<T, Scale1>& Val1, const CScaledValue<T, Scale2>& Val2)
{
    return CScaledValue<T, Scale1+Scale2>(Val1.Value()*Val2.Value());
}

}//namespace isl

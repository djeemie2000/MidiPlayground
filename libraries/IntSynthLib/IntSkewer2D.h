#pragma once

namespace isl
{

template<class T>
class CPhaseSkewer2D
{
public:
    CPhaseSkewer2D(T MinX, T MinY, T MaxX, T MaxY)
     : m_MinX(MinX)
     , m_MinY(MinY)
     , m_MaxX(MaxX)
     , m_MaxY(MaxY)
     , m_SkewX((MaxX+MinX)/2)
     , m_SkewY((MaxY+MinY)/2)
    {}

    void SetSkew(T SkewX, T SkewY)
    {
        m_SkewX = SkewX;
        m_SkewY = SkewY;
        m_MultLow = SkewY-m_MinY;
        m_DivLow = (SkewX-m_MinX) ? SkewX-m_MinX : 1;
        m_MultHigh = m_MaxY-SkewY;
        m_DivHigh = (m_MaxX-SkewX) ? m_MaxX-SkewX : 1;
    }

    T operator()(T Phase)
    {
        return Phase<m_SkewX ? m_MinY + m_MultLow*(Phase-m_MinX)/m_DivLow : m_SkewY + m_MultHigh*(Phase-m_SkewX)/m_DivHigh;
    }

private:
    const T m_MinX;
    const T m_MinY;
    const T m_MaxX;
    const T m_MaxY;

    T m_SkewX;
    T m_SkewY;
    T m_MultLow;
    T m_DivLow;
    T m_MultHigh;
    T m_DivHigh;
};

}//namespace isl

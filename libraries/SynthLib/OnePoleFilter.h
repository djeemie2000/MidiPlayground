#ifndef ONEPOLEFILTER_H
#define ONEPOLEFILTER_H

#include <cstdlib>

template<class T>
class COnePoleLowPassFilter
{
public:
    COnePoleLowPassFilter()
        : m_PrevOut(0)
        , m_A0(1)
        , m_B1(0)
    {}

    void SetParameter(T Parameter)
    {
        // parameter in [0,1] range
        // parameter is 1 => no filtering
        // parameter is 0 => lowest cutoff
        m_A0 = Parameter*Parameter;
        m_B1 = 1-m_A0;
    }

    T operator()(T In)
    {
        m_PrevOut = m_A0*In + m_B1*m_PrevOut;
        return m_PrevOut;
    }

    T operator()(T In, T Parameter)
    {
        m_PrevOut = Parameter*Parameter*In + (1-Parameter*Parameter)*m_PrevOut;
        return m_PrevOut;
    }

private:
    T m_PrevOut;
    T m_A0;
    T m_B1;
};

template<class T>
class COnePoleHighPassFilter
{
public:
    COnePoleHighPassFilter()
        : m_PrevOut(0)
        , m_PrevIn(0)
    {}

    T operator()(T In, T Parameter)
    {
        // http://www.dspguide.com/ch19/2.htm
        // http://www.dspguide.com/ch19/1.htm
        T Param = Parameter;//*Parameter;
        T CoeffA0 = (1+Param)/2;
        T CoeffA1 = -CoeffA0;
        T CoeffB1 = Param;
        m_PrevOut = CoeffA0*In + CoeffA1*m_PrevIn + CoeffB1*m_PrevOut;
        m_PrevIn = In;
        return m_PrevOut;
    }

private:
    T m_PrevOut;
    T m_PrevIn;
};

template<class T, class FilterType, int N>
class CMultiStageFilter
{
public:
    CMultiStageFilter()
        : m_Filter()
        , m_Stages(1)
        , m_Feedback(0)
        , m_PrevOut(0)
    {
        //m_Filter.fill(FilterType());
    }

    void SetStages(int Stages)
    {
        m_Stages = Stages;
    }

    void SetParameter(T Parameter)
    {
        for(int idx = 0; idx<N; ++idx)
        {
            m_Filter[idx].SetParameter(Parameter);
        }
    }

    void SetFeedback(T Feedback)
    {
        // expected to be in [0,1]
        m_Feedback = Feedback;
    }

    T operator()(T In)
    {
        T Out = In - m_Feedback*m_PrevOut;
        int Stage = 0;
        while(Stage<m_Stages)
        {
            Out = m_Filter[Stage](Out);
            ++Stage;
        }
        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
        return m_PrevOut;
    }

    T operator()(T In, T Parameter)
    {
        T Out = In - m_Feedback*m_PrevOut;
        int Stage = 0;
        while(Stage<m_Stages)
        {
            Out = m_Filter[Stage](Out, Parameter);
            ++Stage;
        }
        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
        return m_PrevOut;
    }

    T operator()(T In, T Parameter, T Feedback)
    {
        T Out = In - Feedback*m_PrevOut;
        int Stage = 0;
        while(Stage<m_Stages)
        {
            Out = m_Filter[Stage](Out, Parameter);
            ++Stage;
        }
        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
        return m_PrevOut;
    }

private:
    FilterType m_Filter[8];
    int m_Stages;
    T m_Feedback;
    T m_PrevOut;

};

#endif // ONEPOLEFILTER_H

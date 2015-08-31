#pragma once

template<class T, int Scale = 8>
class CIntegerOnePoleLowPassFilter
{
public:
    CIntegerOnePoleLowPassFilter(T Initial)
        : m_PrevOut(Initial)
        , m_A0(1<<Scale)
        , m_B1(0)
    {}

    void SetParameter(T Parameter)
    {
        // parameter in [0,1<<N] range
        // parameter is 1<<Scale => no filtering
        // parameter is 0 => lowest cutoff
        m_A0 = (Parameter*Parameter) >> Scale;// renormalize square back to range [0,1<<Scale]
        m_B1 = (1<<Scale)-m_A0;
    }

    T operator()(T In)
    {
        m_PrevOut = (m_A0*In + m_B1*m_PrevOut) >> Scale;
        return m_PrevOut;
    }

//    T operator()(T In, T Parameter)
//    {
//        m_PrevOut = Parameter*Parameter*In + (1-Parameter*Parameter)*m_PrevOut;
//        return m_PrevOut;
//    }

private:
    T m_PrevOut;
    T m_A0;
    T m_B1;
};

//template<class T>
//class COnePoleHighPassFilter
//{
//public:
//    COnePoleHighPassFilter()
//        : m_PrevOut(0)
//        , m_PrevIn(0)
//    {}

//    T operator()(T In, T Parameter)
//    {
//        // http://www.dspguide.com/ch19/2.htm
//        // http://www.dspguide.com/ch19/1.htm
//        T Param = Parameter;//*Parameter;
//        T A0 = (1+Param)/2;
//        T A1 = -A0;
//        T B1 = Param;
//        m_PrevOut = A0*In + A1*m_PrevIn + B1*m_PrevOut;
//        m_PrevIn = In;
//        return m_PrevOut;
//    }

//private:
//    T m_PrevOut;
//    T m_PrevIn;
//};

//template<class T, class FilterType, int N>
//class CMultiStageFilter
//{
//public:
//    CMultiStageFilter()
//        : m_Filter()
//        , m_Stages(1)
//        , m_Feedback(0)
//        , m_PrevOut(0)
//    {
//        m_Filter.fill(FilterType());
//    }

//    void SetStages(int Stages)
//    {
//        m_Stages = Stages;
//    }

//    void SetParameter(T Parameter)
//    {
//        for(auto& Filter : m_Filter)
//        {
//            Filter.SetParameter(Parameter);
//        }
//    }

//    void SetFeedback(T Feedback)
//    {
//        // expected to be in [0,1]
//        m_Feedback = Feedback;
//    }

//    T operator()(T In)
//    {
//        T Out = In - m_Feedback*m_PrevOut;
//        int Stage = 0;
//        while(Stage<m_Stages)
//        {
//            Out = m_Filter[Stage](Out);
//            ++Stage;
//        }
//        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
//        return m_PrevOut;
//    }

//    T operator()(T In, T Parameter)
//    {
//        T Out = In - m_Feedback*m_PrevOut;
//        int Stage = 0;
//        while(Stage<m_Stages)
//        {
//            Out = m_Filter[Stage](Out, Parameter);
//            ++Stage;
//        }
//        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
//        return m_PrevOut;
//    }

//    T operator()(T In, T Parameter, T Feedback)
//    {
//        T Out = In - Feedback*m_PrevOut;
//        int Stage = 0;
//        while(Stage<m_Stages)
//        {
//            Out = m_Filter[Stage](Out, Parameter);
//            ++Stage;
//        }
//        m_PrevOut = Out;//HardLimitSigned(Out);//TODO more efficient?
//        return m_PrevOut;
//    }

//private:
//    std::array<FilterType, N> m_Filter;
//    int m_Stages;
//    T m_Feedback;
//    T m_PrevOut;

//};

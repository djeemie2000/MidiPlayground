#pragma once

namespace isl
{

template<class T, int Scale = 8>
class CIntegerOnePoleLowPassFilter
{
public:
    CIntegerOnePoleLowPassFilter(T Initial)
        : m_PrevOut(Initial)
        , m_A0(1<<Scale)
        , m_B1(0)
    {}

    CIntegerOnePoleLowPassFilter()
          : m_PrevOut(0)
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

template<class T, int Scale>
class CIntegerFeedbackOperator
{
public:
  CIntegerFeedbackOperator()
   : m_PrevOut(0)
   , m_Feedback(0)
  {}

    void SetFeedback(T Feedback)
    {
      // feedback should be in 0, 2^Scale]
        m_Feedback = Feedback;
    }

    template<class OperatorType>
    T operator()(T In, OperatorType& Operator)
    {
      m_PrevOut = Operator( ((In<<Scale) - m_Feedback * m_PrevOut) >> Scale );
      return m_PrevOut;
    }

private:
  T m_PrevOut;
  T m_Feedback;
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

template<class T, class FilterType, int N>
class CIntegerMultiStageFilter
{
public:
    CIntegerMultiStageFilter()
        : m_Filter()
        , m_Stages(1)
    {
    }

    void SetStages(int Stages)
    {
        m_Stages = Stages;
    }

    void SetParameter(T Parameter)
    {
        for(int idx; idx<N; ++idx)
        {
            m_Filter[idx].SetParameter(Parameter);
        }
    }

    T operator()(T In)
    {
        T Out = In;// - m_Feedback*m_PrevOut;
        int Stage = 0;
        while(Stage<m_Stages)
        {
            Out = m_Filter[Stage](Out);
            ++Stage;
        }
          return Out;
    }

private:
    FilterType m_Filter[N];
    int m_Stages;
};

}

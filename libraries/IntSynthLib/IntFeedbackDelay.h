#pragma once

#include "IntDelayLine.h"

namespace isl
{

template<class T, int Capacity>
class CFeedbackDelay
{
public:
    CFeedbackDelay()
     : m_DelayLine()
     , m_Delay(0)
     , m_FeedbackValue(0)
     , m_FeedbackScale(8)
     , m_WetValue(0)
     , m_DryValue(256)
     , m_WetDryScale(8)
    {
    }

    void SetFeedback(T Feedback, int FeedbackScale)
    {
        m_FeedbackValue = Feedback;
        m_FeedbackScale = FeedbackScale;
    }

    void SetWetDry(T Wet, int WetDryScale)
    {
        m_WetValue = Wet;
        m_DryValue = (1<<WetDryScale)-Wet;
        m_WetDryScale = WetDryScale;
    }

    void SetDelay(int Delay)
    {
        if(0<=Delay && Delay<Capacity)
        {
            m_Delay = Delay;
        }
    }

    T operator()(T In)
    {
        T DelayValue = m_DelayLine.Read(m_Delay);
        T WriteValue = In + ( (DelayValue * m_FeedbackValue)>>m_FeedbackScale );
        m_DelayLine.Write(WriteValue);
        return (m_WetValue * DelayValue + m_DryValue * In)>>m_WetDryScale;
    }

private:
    CDelayLine<T, Capacity> m_DelayLine;
    int m_Delay;
    T m_FeedbackValue;
    int m_FeedbackScale;
    T m_WetValue;
    T m_DryValue;
    int m_WetDryScale;
};

}//namespace isl

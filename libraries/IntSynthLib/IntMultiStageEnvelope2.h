#pragma once

namespace isl
{

template<class T, int NumStages, int Scale>
class CMultiStageEnvelope2
{
public:
    enum EAction
    {
        AdvanceAction,
        HoldAction,
        ResetAction,
        SkipAction
    };

    CMultiStageEnvelope2()
     : m_Stage(0)
     , m_Counter(0)
     , m_Gate(false)
     , m_Stages()
    {}

    void SetTarget(int Stage, T Target)
    {
        m_Stages[Stage].s_Target = Target;
    }

    void SetDuration(int Stage, T Duration)
    {
        m_Stages[Stage].s_Duration = Duration;
    }

    void SetAction(int Stage, bool Gate, EAction Action)
    {
        if(Gate)
        {
            m_Stages[Stage].s_GateOnAction = Action;
        }
        else
        {
            m_Stages[Stage].s_GateOffAction = Action;
        }
    }

    void NoteOn()
    {
        m_Gate = true;
    }

    void NoteOff()
    {
        m_Gate = false;
    }

    void Reset()
    {
        m_Stage = 0;
        m_Counter = 0;
    }

    T operator()()
    {
        T Value = CalcValue();

        EAction CurrentAction = m_Gate ? m_Stages[m_Stage].s_GateOnAction
                                       : m_Stages[m_Stage].s_GateOffAction;

        if(CurrentAction==AdvanceAction)
        {
            Advance();
        }
        else if(CurrentAction==HoldAction)
        {
            // hold -> do nothing
        }
        else if(CurrentAction==ResetAction)
        {
            Reset();
        }
        else if(CurrentAction==SkipAction)
        {
            Skip();
        }

        return Value;
    }

    int GetStage() const
    {
        return m_Stage;
    }

    bool GetHold() const
    {
        return m_Gate ? m_Stages[m_Stage].s_GateOnAction == HoldAction
                      : m_Stages[m_Stage].s_GateOffAction == HoldAction;
    }

private:
    T CalcValue()
    {
        T Counter = m_Counter;
        // linear interpolation between stages
        T Duration = m_Stages[m_Stage].s_Duration;
        T Reference = m_Stages[m_Stage].s_Target;
        // target:
        int NextStage = m_Stage<LastStage ? m_Stage+1 : 0;
        T Target = m_Stages[NextStage].s_Target;

        T Value = Duration ? Reference + (Target-Reference)*Counter/Duration : Reference;

        return Value;
    }

    void Advance()
    {
        ++m_Counter;
        if(m_Stages[m_Stage].s_Duration<=m_Counter)
        {
            // advance stage
            ++m_Stage;
            if(NumStages<=m_Stage)
            {
                m_Stage = 0;
            }
            m_Counter = 0;
        }
    }

    void Skip()
    {
        // advance stage
        ++m_Stage;
        if(NumStages<=m_Stage)
        {
            m_Stage = 0;
        }
        m_Counter = 0;
    }

    const int LastStage = NumStages-1;

    struct SStage
    {
        T s_Target;
        T s_Duration;
        EAction s_GateOnAction;
        EAction s_GateOffAction;

        SStage()
         : s_Target(0)
         , s_Duration(0)
         , s_GateOnAction(AdvanceAction)
         , s_GateOffAction(HoldAction)
        {}
    };

    int m_Stage;
    T m_Counter;
    bool m_Gate;
    SStage  m_Stages[NumStages];
};

}

#pragma once

template<class T>
class CStepper
{
  public:
    CStepper()
    : m_NumSteps(1)
    , m_Step(0)
    {}

    void SetNumSteps(T NumSteps)
    {
      m_NumSteps = NumSteps;
    }

    T GetNumSteps() const
    {
      return m_NumSteps;
    }

    void Advance()
    {
      ++m_Step;
      if(m_NumSteps<=m_Step)
      {
        m_Step = 0;
      }
    }

    int GetStep() const
    {
      return m_Step;
    }

  private:
    T m_Step;
    T m_NumSteps;
};


#ifndef PERIODICTRIGGER_H
#define PERIODICTRIGGER_H

class CPeriodicTrigger
{
public:
  CPeriodicTrigger()
:
    m_Period(1)
      , m_Duration(1)
        , m_Counter(1)
        {
        }

  void Configure(int Period, int Duration)
  {
    m_Period = Period;
    m_Duration = Duration;
  }

  void Update()
  {
    if(1<m_Counter)
    {
      --m_Counter;
    }
    else
    {
      m_Counter = m_Period;
    }
  }

  bool TriggerOn() const
  {
    return (m_Counter == 1);
  }

  bool TriggerOff() const
  {
    int TriggerOff = m_Period - m_Duration;
    return (m_Counter == TriggerOff);
  }

private:
  int m_Period;
  int m_Duration;
  int m_Counter;
};

#endif // PERIODICTRIGGER_H

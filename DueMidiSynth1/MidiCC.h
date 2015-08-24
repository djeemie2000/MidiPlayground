#pragma once

class CMidiCC
{
  public:
    static const int NumControllers = 128;

    CMidiCC()
    {
      for(int idx = 0; idx<NumControllers; ++idx)
      {
        m_ControllerValues[idx] = 0;
      }
    }

  // note: no checks on controller range
    void SetController(int Controller, int Value)
    {
      m_ControllerValues[Controller] = Value;
    }

    int GetController(int Controller) const
    {
      return m_ControllerValues[Controller];
    }

  private:
    int m_ControllerValues[NumControllers];
};


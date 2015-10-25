#pragma once

template<int Size, class T = int>
class CControlChange
{
  public:
    typedef void(*ChangeCallback)(int,int);
  
    CControlChange()
     : m_Value()
     , m_ChangeThreshold(0)
     , m_ChangeCallback(0)
    {
      for(int idx = 0; idx<Size; ++idx)
      {
        m_Value[idx] = 0;
        m_Smooth[idx] = 0;
      }
    }

    void SetChangeCallback(ChangeCallback Callback)
    {
      m_ChangeCallback = Callback;
    }

    void SetChangeThreshold(T Threshold)
    {
      m_ChangeThreshold = Threshold;
    }

    void SetValue(int idx, T Value)
    {
      if(0<=idx && idx<=Size)
      {
        m_Smooth[idx] = (3*m_Smooth[idx]+Value)/4;
        
        if(m_ChangeThreshold<abs(m_Smooth[idx]-m_Value[idx]))
        {
          if(m_ChangeCallback)
          {
            m_ChangeCallback(idx, m_Smooth[idx]);
          }
          m_Value[idx] = m_Smooth[idx];
        }
      }      
    }

  T GetValue(int idx) const
  {
    if(0<=idx && idx<=Size)
    {
      return m_Value[idx];
    }
    return -1;
  }
    
  private:
    T m_Smooth[Size];
    T m_Value[Size];
    T m_ChangeThreshold;
    ChangeCallback m_ChangeCallback;
};


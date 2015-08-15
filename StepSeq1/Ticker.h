 #ifndef TICKER_H_INCLUDE
#define TICKER_H_INCLUDE

class CTicker
{
  public:
    static const int TickOn = 1;
    static const int TickNone = 0;
    static const int TickOff = -1;

    CTicker()
     : m_PeriodMilliSeconds(500)
     , m_NextTimeMilliSeconds(0)
     {}

     void SetPeriod(unsigned long PeriodMilliSeconds)
     {
      m_PeriodMilliSeconds = PeriodMilliSeconds;
     }

     void Reset()
     {
      m_NextTimeMilliSeconds = 0;
     }

      // returns true periodically
     int Tick()
     {
      unsigned long CurrentTimeMilliSeconds = millis();
      // init first time => tick returns TickOn
      if(m_NextTimeMilliSeconds==0)
      {
        m_NextTimeMilliSeconds = CurrentTimeMilliSeconds;
      }
      if(m_NextTimeMilliSeconds<=CurrentTimeMilliSeconds)
      {
        m_NextTimeMilliSeconds += m_PeriodMilliSeconds;
        return TickOn;
      }
      return TickNone;
     }
  private:
    unsigned long m_PeriodMilliSeconds;
    unsigned long m_NextTimeMilliSeconds;
};

#endif /* end of include guard:  */

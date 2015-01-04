#include <Arduino.h>

/*
Shift register with 8 outputs using a 74HC595 shift register.
*/
class CShiftRegisterOutputs
{
  public:
    CShiftRegisterOutputs();
    
    void Begin(int LatchPin, int ClockPin, int DataPin);
    void Set(int Output, bool State);
    void Write();
    
  private:
    int m_LatchPin;
    int m_ClockPin;
    int m_DataPin;
    uint8_t m_State;
};


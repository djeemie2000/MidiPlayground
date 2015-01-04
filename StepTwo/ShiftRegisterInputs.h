#ifndef SHIFTREGISTERINPUTS_H
#define SHIFTREGISTERINPUTS_H

#include <Arduino.h>

/*
Shift register with 8 inputs using a 74HC165 shift register.
*/
class CShiftRegisterInputs
{
public:
    CShiftRegisterInputs();

    void Begin(int EnablePin, int LoadPin, int ClockPin, int DataPin);
    bool Get(int Output) const;
    void Read();

  private:
    int m_EnablePin;
    int m_LoadPin;
    int m_ClockPin;
    int m_DataPin;
    uint8_t m_State;
};

#endif // SHIFTREGISTERINPUTS_H
